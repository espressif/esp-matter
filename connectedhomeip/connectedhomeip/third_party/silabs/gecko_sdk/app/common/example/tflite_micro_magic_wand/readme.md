# TensorFlow Lite Micro - Magic Wand 
This application demonstrates a model trained to recognize various hand gestures
with an accelerometer. The detected gestures are printed to the serial port. 

With the board facing up, and the USB cable pointed towards you, you should be
able to perform one of the three gestures: Wing (*W*), Ring (*O*), or Slope
(*L*) and have them correctly detected and output to the serial port. The
detection works best when the board is mounted to the end of a wand, or similar.
The LEDs should also light up to indicate a detected gesture. 

This application is a port of TensorFlow's Magic Wand application. For details
about the application or information on how to train a model of your own, see
TensorFlow Lite Micro's [GitHub page](https://github.com/tensorflow/tflite-micro/blob/3e190e5389be49c94475e509452bdae245bd4fa6/tensorflow/lite/micro/examples/magic_wand).
