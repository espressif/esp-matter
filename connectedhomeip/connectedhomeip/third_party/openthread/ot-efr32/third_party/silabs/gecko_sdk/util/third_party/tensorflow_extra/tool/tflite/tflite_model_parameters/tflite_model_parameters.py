# pylint: disable=unused-wildcard-import, wildcard-import
import os

from tflite_model import TfliteModel
from .flatbuffer_dictionary import FlatbufferDictionary



# This is the tag (or key) used to insert
# the serialized parameters into the .tflite model's metadata
TFLITE_METADATA_TAG = 'SL_PARAMSv1'


class TfliteModelParameters(FlatbufferDictionary):
    """ .tflite Model Parameters

    Model parameters are effectively a dictionary of key/value pairs where:
    - key - Name of parameter as a string
    - value - Value of parameter as a specific scalar data type

    The model parameters are serialized using a [Flatbuffer](https://google.github.io/flatbuffers) schema.

    The serialized parameters are inserted into a .tflite model's "metadata" section:
    https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/schema/schema.fbs#L1235

    The basic flow is:

    1. User Python script populates a TfliteModelParameters object
    2. Use TfliteModelParameters.add_to_tflite_file() to serialize parameters and add to .tflite model metadata
    3. At a later time, use TfliteModelParameters.load_from_tflite_file() to load parameters from .tflite model metadata
    4. Another user Python script accesses parameters in TfliteModelParameters

    NOTE: The TfliteModelParameters object inheriets FlatbufferDictionary 
          which inheriets the standard Python 'dict' class.

    """

    @staticmethod
    def load_from_tflite_file(tflite_path:str):
        """Load the TfliteModelParameters from the given .tflite model file's metadata"""

        if not os.path.exists(tflite_path):
            raise FileNotFoundError(f'.tflite model file not found at: {tflite_path}')

        with open(tflite_path, 'rb') as fp:
            tflite_flatbuffer = fp.read()

        return TfliteModelParameters.load_from_tflite_flatbuffer(tflite_flatbuffer)


    @staticmethod
    def load_from_tflite_flatbuffer(tflite_flatbuffer:bytes):
        """Load the TfliteModelParameters from the given .tflite model flatbuffer bytes"""

        # Load the .tflite model flatbuffer
        tflite_model = TfliteModel(tflite_flatbuffer)

        return TfliteModelParameters.load_from_tflite_model(tflite_model)


    @staticmethod
    def load_from_tflite_model(tflite_model: TfliteModel):
        """Load the TfliteModelParameters from the given TfliteModel object"""

        # Retrieve the model parameters from the metadata
        parameters_flatbuffer = tflite_model.get_metadata(TFLITE_METADATA_TAG)
        if parameters_flatbuffer is None:
            raise RuntimeError(f'Model parameters with tag {TFLITE_METADATA_TAG} not found in .tflite model file')

        # Load the TfliteModelParameters object from the flatbuffer
        params = TfliteModelParameters.deserialize(parameters_flatbuffer)
        return TfliteModelParameters(params)


    def add_to_tflite_file(self, tflite_path:str, output:str=None):
        """Add the model parameters to the given .tflite model file

        This adds the current parameters to the given .tflite model file's metadata.
        If output argument is given, then overwrite the given .tflite model file.

        Args:
            tflite_path: Path to .tflite model file 
            output: Optional, path to output .tflite model file
        """
        tflite_model = TfliteModel.load_flatbuffer_file(tflite_path)
        parameters_flatbuffer = self.serialize()
        tflite_model.add_metadata(TFLITE_METADATA_TAG, parameters_flatbuffer)

        tflite_model.save(output)


    def add_to_tflite_flatbuffer(self, tflite_flatbuffer:bytes) -> bytes:
        """Add the model parameters to the given .tflite flatbuffer and return
        the updated flatbuffer

        Args:
            tflite_flatbuffer: .tflite model flatbuffer to update with the model parameters

        Returns:
            The update .tflite model flatbuffer
        """
        parameters_flatbuffer = self.serialize()

        tflite_model = TfliteModel(tflite_flatbuffer)
        tflite_model.add_metadata(TFLITE_METADATA_TAG, parameters_flatbuffer)

        return tflite_model.flatbuffer_data


    def add_to_tflite_model(self, tflite_model:TfliteModel):
        """Add the model parameters to the given TfliteModel object

        Args:
            tflite_model: TfliteModel model object
        """
        parameters_flatbuffer = self.serialize()
        tflite_model.add_metadata(TFLITE_METADATA_TAG, parameters_flatbuffer)
