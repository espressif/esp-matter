#!/usr/bin/env python3

import sys
import os
import argparse
from string import Template
import re

# Patch site-packages to find numpy
import jinja2
if sys.platform.startswith("win"):
  site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), "../../../ext-site-packages"))
else:
  site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), "../../../../ext-site-packages"))
if os.path.exists(site_packages_path):
  if site_packages_path not in sys.path:
    sys.path.insert(0, site_packages_path)

"""
Generation of parameter files requires the tflite_model, tflite_model_parameters
and tensorflow_lite_support packages. Because these packages are not installed
in the uc-generation environment where this python script will be run, these
packages are supplied as source. tflite_model and tflite_model_parameters were
fetched from internal repos, while the tensorflow_lite_support was fetched from
https://github.com/tensorflow/tflite-support.
"""
import tflite.Model
from tflite_model import TfliteModel
from tflite_model_parameters import TfliteModelParameters

model_data_h = """// Auto-generated serialization of TFLite flatbuffers in config directory
#ifndef SL_TFLITE_MICRO_MODEL_H
#define SL_TFLITE_MICRO_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern const uint8_t sl_tflite_model_array[];
extern const uint32_t sl_tflite_model_len;

#ifdef __cplusplus
}
#endif

#endif // SL_TFLITE_MICRO_MODEL_H
"""

template_model_data_c = """// Auto-generated serialization of TFLite flatbuffers in config directory

#include "em_device.h"
#include "sl_tflite_micro_model.h"

// Model data generated from "${model_name}.tflite"
const uint8_t sl_tflite_model_array[] __ALIGNED(4) = {
  ${data}
};

const uint32_t sl_tflite_model_len = ${data_len}UL;
"""

template_opcode_resolver_h = """// Auto-generated macro to instanciate and initialize opcode resolver based on TFLite flatbuffers in config directory
#ifndef SL_TFLITE_MICRO_OPCODE_RESOLVER_H
#define SL_TFLITE_MICRO_OPCODE_RESOLVER_H

#define SL_TFLITE_MICRO_OPCODE_RESOLVER(opcode_resolver, error_reporter) \\
static tflite::MicroMutableOpResolver<${data_len}> opcode_resolver(error_reporter); \\
${data}

#endif // SL_TFLITE_MICRO_OPCODE_RESOLVER_H
"""

template_model_parameter_single = """#define SL_TFLITE_MODEL_${config_key} ${config_val}
"""

template_model_parameters_h = """// Auto-generated parameters from TFLite flatbuffers in config directory
#ifndef SL_TFLITE_MICRO_MODEL_PARAMETERS_H
#define SL_TFLITE_MICRO_MODEL_PARAMETERS_H

// Definitions generated from "${model_name}.tflite"'
${data}
#endif // SL_TFLITE_MICRO_MODEL_PARAMETERS_H
"""
"""
The following dictionary has been created using the BuiltinOperator enum defining operatior values, see schema_generated.h, and
function names defined in the MicroMutableOpResolver object, see micro_mutable_op_resolver.h.
"""
opcode_dict = {
    101: 'AddAbs',
    0:   'AddAdd',
    106: 'AddAddN',
    56:  'AddArgMax',
    79:  'AddArgMin',
    1:   'AddAveragePool2D',
    104: 'AddCeil',
    2:   'AddConcatenation',
    3:   'AddConv2D',
    108: 'AddCos',
    4:   'AddDepthwiseConv2D',
    6:   'AddDequantize',
    71:  'AddEqual',
    8:   'AddFloor',
    9:   'AddFullyConnected',
    61:  'AddGreater',
    62:  'AddGreaterEqual',
    117: 'AddHardSwish',
    11:  'AddL2Normalization',
    58:  'AddLess',
    63:  'AddLessEqual',
    73:  'AddLog',
    86:  'AddLogicalAnd',
    87:  'AddLogicalNot',
    84:  'AddLogicalOr',
    14:  'AddLogistic',
    55:  'AddMaximum',
    17:  'AddMaxPool2D',
    40:  'AddMean',
    57:  'AddMinimum',
    18:  'AddMul',
    59:  'AddNeg',
    73:  'AddNotEqual',
    83:  'AddPack',
    34:  'AddPad',
    60:  'AddPadV2',
    54:  'AddPrelu',
    114: 'AddQuantize',
    82:  'AddReduceMax',
    19:  'AddRelu',
    21:  'AddRelu6',
    22:  'AddReshape',
    97:  'AddResizeNearestNeighbor',
    116: 'AddRound',
    76:  'AddRsqrt',
    77:  'AddShape',
    66:  'AddSin',
    25:  'AddSoftmax',
    47:  'AddSplit',
    102: 'AddSplitV',
    75:  'AddSqrt',
    92:  'AddSquare',
    45:  'AddStridedSlice',
    41:  'AddSub',
    27:  'AddSvdf',
    28:  'AddTanh',
    67:  'AddTransposeConv',
    88:  'AddUnpack'
}

def sanitize_filename(name):
  # Strip invalid characters
  name = re.sub(r'[^a-zA-Z0-9_]', '', name)
  # C variables can't start with a number
  name = name.lstrip('0123456789')
  return name


def find_first_tflite_file(input_dir):
  '''
  Return the alphabetically sorted first tflite filename and content
  '''
  for f in sorted(os.listdir(input_dir)):
    if os.path.splitext(f)[-1] == '.tflite':
      with open(os.path.join(input_dir, f), 'rb') as fd:
        data = fd.read()
      filename = sanitize_filename(os.path.splitext(f)[0])
      return filename, data

def generate_c_array(buf):
  arr = ''
  for i, ch in enumerate(buf):
    if (i % 12) == 0:
      arr += '\n  '
    arr += '0x{:02x}, '.format(ch)

  return arr.lstrip().rstrip(', ')

def opcode_parse_opcode(opcode):
  if opcode.CustomCode() != None:
    opcode_val = opcode.CustomCode()
  else:
    opcode_val = opcode.BuiltinCode()
  if opcode_val in opcode_dict.keys():
    opcode_func = opcode_dict[opcode_val]
    opcode_entry = {opcode_val: opcode_func}
  else:
    print(f"tflite.py WARNING: An unknown operator with code value={opcode_val} has been discovered. It will not be automatic initialized.")
    opcode_entry = {-1: "UndefinedOp"}
  return opcode_entry

def opcode_parse_model(model):
  opcodes = {}
  for index in range(model.OperatorCodesLength()):
    opcode = model.OperatorCodes(index)
    opcodes.update(opcode_parse_opcode(opcode))
  return opcodes

def generate_c_type(value):
  if type(value) is list:
        c_list =[]
        for item in value:
          c_list.append(generate_c_type(item))
        value = "{" + ', '.join(c_list) + "}"
  elif type(value) == str:
    value = f'"{value}"'
  elif type(value) == bool:
    value = str(value).lower()
  elif type(value) == float:
    value = str(value) + "f"
  return value

def generate_files(input_dir, output_dir):
  model_name, buf = find_first_tflite_file(input_dir)

  # Generate model data
  props = {
    'model_name': model_name,
    'data': generate_c_array(buf),
    'data_len': len(buf),
  }
  tc = Template(template_model_data_c)
  model_data_c = tc.substitute(**props)

  # Generate OP code resolver
  opcodes = {}
  model = tflite.Model.Model.GetRootAsModel(buf)
  opcodes.update(opcode_parse_model(model))
  opcode_len = len(opcodes)
  opcode_str = ''
  for opcode_key in opcodes.keys():
    if opcode_key != 32:       # CUSTOM opcode
      opcode_str += f"opcode_resolver.{opcodes[opcode_key]}(); \\\n"
  tm = Template(template_opcode_resolver_h)
  opcode_data = tm.substitute({'data_len':str(opcode_len), 'data':opcode_str})

  # Extract model parameters
  parameter_defines = ''
  try:
    loaded_model_params = TfliteModelParameters.load_from_tflite_flatbuffer(buf)
  except:
    loaded_model_params = {}
  if loaded_model_params:
    param_define_t = Template(template_model_parameter_single)
    for key, value in sorted(loaded_model_params.items()):
      # Ensure valid C code:
      c_value = generate_c_type(value)
      props = {
        'config_key': key.replace('.', '_').upper(),
        'config_val': c_value,
      }
      parameter_defines += param_define_t.substitute(**props)
    parameter_defines += '\n'

  with open(os.path.join(output_dir, 'sl_tflite_micro_model.h'), 'w') as fd:
    fd.write(model_data_h)
  with open(os.path.join(output_dir, 'sl_tflite_micro_model.c'), 'w') as fd:
    fd.write(model_data_c)
  with open(os.path.join(output_dir, 'sl_tflite_micro_opcode_resolver.h'), 'w') as fd:
    fd.write(opcode_data)
  # Only emit this file if model parameters are available
  if parameter_defines:
    tp = Template(template_model_parameters_h)
    with open(os.path.join(output_dir, 'sl_tflite_micro_model_parameters.h'), 'w') as fd:
      fd.write(tp.substitute(data=(parameter_defines), model_name=model_name))



def entry():
  parser = argparse.ArgumentParser(description='TensorFlow Lite flatbuffer to C converter.')
  parser.add_argument('-i', required=True, help='Input directory containing .tflite files')
  parser.add_argument('-o', required=True, help='Output directory to populate with serialized content.')
  args = parser.parse_args()

  generate_files(args.i, args.o)

entry()