# Copyright 2020 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Writes metadata and label file to the object detector models."""

from typing import List, Optional

import flatbuffers
from tensorflow_lite_support.metadata import metadata_schema_py_generated as _metadata_fb
from tensorflow_lite_support.metadata.python import metadata as _metadata
from tensorflow_lite_support.metadata.python.metadata_writers import metadata_info
from tensorflow_lite_support.metadata.python.metadata_writers import metadata_writer
from tensorflow_lite_support.metadata.python.metadata_writers import writer_utils

_MODEL_NAME = "ObjectDetector"
_MODEL_DESCRIPTION = (
    "Identify which of a known set of objects might be present and provide "
    "information about their positions within the given image or a video "
    "stream.")
_INPUT_NAME = "image"
_INPUT_DESCRIPTION = "Input image to be detected."
_OUTPUT_LOCATION_NAME = "location"
_OUTPUT_LOCATION_DESCRIPTION = "The locations of the detected boxes."
_OUTPUT_CATRGORY_NAME = "category"
_OUTPUT_CATEGORY_DESCRIPTION = "The categories of the detected boxes."
_OUTPUT_SCORE_NAME = "score"
_OUTPUT_SCORE_DESCRIPTION = "The scores of the detected boxes."
_OUTPUT_NUMBER_NAME = "number of detections"
_OUTPUT_NUMBER_DESCRIPTION = "The number of the detected boxes."
_CONTENT_VALUE_DIM = 2
_BOUNDING_BOX_INDEX = (1, 0, 3, 2)
_GROUP_NAME = "detection_result"


def _create_1d_value_range(dim: int) -> _metadata_fb.ValueRangeT:
  """Creates the 1d ValueRange based on the given dimension."""
  value_range = _metadata_fb.ValueRangeT()
  value_range.min = dim
  value_range.max = dim
  return value_range


def _create_location_metadata(
    location_md: metadata_info.TensorMd) -> _metadata_fb.TensorMetadataT:
  """Creates the metadata for the location tensor."""
  location_metadata = location_md.create_metadata()
  content = _metadata_fb.ContentT()
  content.contentPropertiesType = (
      _metadata_fb.ContentProperties.BoundingBoxProperties)
  properties = _metadata_fb.BoundingBoxPropertiesT()
  properties.index = list(_BOUNDING_BOX_INDEX)
  properties.type = _metadata_fb.BoundingBoxType.BOUNDARIES
  properties.coordinateType = _metadata_fb.CoordinateType.RATIO
  content.contentProperties = properties
  content.range = _create_1d_value_range(_CONTENT_VALUE_DIM)
  location_metadata.content = content
  return location_metadata


# This is needed for both the output category tensor and the score tensor.
def _create_metadata_with_value_range(
    tensor_md: metadata_info.TensorMd) -> _metadata_fb.TensorMetadataT:
  """Creates tensor metadata with extra value range information."""
  tensor_metadata = tensor_md.create_metadata()
  tensor_metadata.content.range = _create_1d_value_range(_CONTENT_VALUE_DIM)
  return tensor_metadata


class MetadataWriter(metadata_writer.MetadataWriter):
  """Writes metadata into an object detector."""

  @classmethod
  def create_from_metadata_info(
      cls,
      model_buffer: bytearray,
      general_md: Optional[metadata_info.GeneralMd] = None,
      input_md: Optional[metadata_info.InputImageTensorMd] = None,
      output_location_md: Optional[metadata_info.TensorMd] = None,
      output_category_md: Optional[metadata_info.CategoryTensorMd] = None,
      output_score_md: Optional[metadata_info.TensorMd] = None,
      output_number_md: Optional[metadata_info.TensorMd] = None):
    """Creates MetadataWriter based on general/input/outputs information.

    Args:
      model_buffer: valid buffer of the model file.
      general_md: general infromation about the model.
      input_md: input image tensor informaton.
      output_location_md: output location tensor informaton. The location tensor
        is a multidimensional array of [N][4] floating point values between 0
        and 1, the inner arrays representing bounding boxes in the form [top,
        left, bottom, right].
      output_category_md: output category tensor information. The category
        tensor is an array of N integers (output as floating point values) each
        indicating the index of a class label from the labels file.
      output_score_md: output score tensor information. The score tensor is an
        array of N floating point values between 0 and 1 representing
        probability that a class was detected.
      output_number_md: output number of dections tensor information. This
        tensor is an integer value of N.

    Returns:
      A MetadataWriter object.
    """

    if general_md is None:
      general_md = metadata_info.GeneralMd(
          name=_MODEL_NAME, description=_MODEL_DESCRIPTION)

    if input_md is None:
      input_md = metadata_info.InputImageTensorMd(
          name=_INPUT_NAME,
          description=_INPUT_DESCRIPTION,
          color_space_type=_metadata_fb.ColorSpaceType.RGB)

    if output_location_md is None:
      output_location_md = metadata_info.TensorMd(
          name=_OUTPUT_LOCATION_NAME, description=_OUTPUT_LOCATION_DESCRIPTION)

    if output_category_md is None:
      output_category_md = metadata_info.CategoryTensorMd(
          name=_OUTPUT_CATRGORY_NAME, description=_OUTPUT_CATEGORY_DESCRIPTION)

    if output_score_md is None:
      output_score_md = metadata_info.TensorMd(
          name=_OUTPUT_SCORE_NAME, description=_OUTPUT_SCORE_DESCRIPTION)

    if output_number_md is None:
      output_number_md = metadata_info.TensorMd(
          name=_OUTPUT_NUMBER_NAME, description=_OUTPUT_NUMBER_DESCRIPTION)

    if output_category_md.associated_files is None:
      output_category_md.associated_files = []

    # Create output tensor group info.
    group = _metadata_fb.TensorGroupT()
    group.name = _GROUP_NAME
    group.tensorNames = [
        output_location_md.name, output_category_md.name, output_score_md.name
    ]

    # Create subgraph info.
    subgraph_metadata = _metadata_fb.SubGraphMetadataT()
    subgraph_metadata.inputTensorMetadata = [input_md.create_metadata()]
    subgraph_metadata.outputTensorMetadata = [
        _create_location_metadata(output_location_md),
        _create_metadata_with_value_range(output_category_md),
        _create_metadata_with_value_range(output_score_md),
        output_number_md.create_metadata()
    ]
    subgraph_metadata.outputTensorGroups = [group]

    # Create model metadata
    model_metadata = general_md.create_metadata()
    model_metadata.subgraphMetadata = [subgraph_metadata]

    b = flatbuffers.Builder(0)
    b.Finish(
        model_metadata.Pack(b),
        _metadata.MetadataPopulator.METADATA_FILE_IDENTIFIER)

    return cls(
        model_buffer,
        b.Output(),
        associated_files=[
            file.file_path for file in output_category_md.associated_files
        ])

  @classmethod
  def create_for_inference(cls, model_buffer: bytearray,
                           input_norm_mean: List[float],
                           input_norm_std: List[float],
                           label_file_paths: List[str]):
    """Creates mandatory metadata for TFLite Support inference.

    The parameters required in this method are mandatory when using TFLite
    Support features, such as Task library and Codegen tool (Android Studio ML
    Binding). Other metadata fields will be set to default. If other fields need
    to be filled, use the method `create_from_metadata_info` to edit them.

    Args:
      model_buffer: valid buffer of the model file.
      input_norm_mean: the mean value used in the input tensor normalization
        [1].
      input_norm_std: the std value used in the input tensor normalizarion [1].
      label_file_paths: paths to the label files [2] in the category tensor.
        Pass in an empty list, If the model does not have any label file.
      [1]:
        https://www.tensorflow.org/lite/convert/metadata#normalization_and_quantization_parameters
      [2]:
        https://github.com/tensorflow/tflite-support/blob/b80289c4cd1224d0e1836c7654e82f070f9eefaa/tensorflow_lite_support/metadata/metadata_schema.fbs#L108

    Returns:
      A MetadataWriter object.
    """
    input_md = metadata_info.InputImageTensorMd(
        name=_INPUT_NAME,
        description=_INPUT_DESCRIPTION,
        norm_mean=input_norm_mean,
        norm_std=input_norm_std,
        color_space_type=_metadata_fb.ColorSpaceType.RGB,
        tensor_type=writer_utils.get_input_tensor_types(model_buffer)[0])

    output_category_md = metadata_info.CategoryTensorMd(
        name=_OUTPUT_CATRGORY_NAME,
        description=_OUTPUT_CATEGORY_DESCRIPTION,
        label_files=[
            metadata_info.LabelFileMd(file_path=file_path)
            for file_path in label_file_paths
        ])

    return cls.create_from_metadata_info(
        model_buffer, input_md=input_md, output_category_md=output_category_md)
