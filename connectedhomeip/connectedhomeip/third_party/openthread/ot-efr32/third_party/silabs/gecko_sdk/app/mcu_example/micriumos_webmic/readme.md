# Micrium OS Webmic

  This example shows how to sample data from the microphone and also how to
  stream that data on a Web server using uC/HTTPs.

  The audio is sampled from on-kit microphones by using LDMA.
  The sampled audio waveform is displayed on LCD. 
  Sampling and encoding is performed in the main OS task.
  Configuration can be found in common_declarations.h and config.h.
  For more information about encoding, see IETFs and xiphs rfc6716, rfc7845 and rfc3533.

  The encoded data is then streamed through a simple no-file-system HTTP server.
  Transmission of data packets is handled by an HTTP server task by means
  of unicast and chunked transfer encoding.

  To listen to the audio, connect the board via Ethernet cable, wait for the IP to be
  shown on the display and type that IP into any browser capable of
  streaming opus. Because of real-time constraints, there's an upper limit of 5 clients.

  Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
  SystemView Trace is enabled by default when the segger_systemview component
  is included in the project. SystemView can be usedto inspect the runtime 
  behaviour of this example. It will give an overview
  of the tasks and interrupts in the application. SystemView can be downloaded 
  from https://www.segger.com/systemview.html.