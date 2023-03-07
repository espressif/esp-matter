# Voice Control Light
This application uses TensorFlow Lite for Microcontrollers to detect the spoken
words "on" and "off" from audio data recorded on the microphone. The detected
keywords are used to control an LED on the board.

Audio is sampled continuously from the microphone at a rate of 8kHz. The
frequency components are then extracted by calculating the FFT on short segments
of audio as they become available. The result is an array of filterbanks,
representing the frequency components of the past second of audio. The processed
audio data is passed into a neural network at an interval of 200 ms. If the
model detects that either of the keywords "on" or "off" were spoken, LED0 will
toggle accordingly and the keyword detection result is printed on VCOM.

The application is based on TensorFlow's example application, **[micro speech](https://github.com/tensorflow/tensorflow/tree/v2.3.1/tensorflow/lite/micro/examples/micro_speech)**.

## Model
The application uses one of two different available models
(```keyword_spotting_on_off.tflite``` or ```keyword_spotting_on_off_v2.tflite```)
as the default model, depending on whether the application is generated for a
development board featuring an MVP hardware accelerator or not. When an MVP
hardware accelerator is featured on the board, inference will run at a faster
speed such that a larger model can be chosen, yielding more accurate keyword
detections.

The neural network model has been trained to identify the two keywords "on" and
"off" from preprocessed audio data. When neither words are recognized, the model
will classify the input as either "unknown" or "background". The model takes an
array of filterbanks as input and outputs a vector with each value corresponding
to the probability that the input belongs to each of the
categories.

Details about the model architectures and scripts for generating the models can
be found in the [Silicon Labs machine learning applications](https://github.com/SiliconLabs/machine_learning_applications/tree/main/) repository, under
```voice/keyword_spotting/model```.