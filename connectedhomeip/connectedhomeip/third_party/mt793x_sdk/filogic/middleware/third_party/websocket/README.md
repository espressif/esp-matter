# librws -  Tiny, cross platform websocket client C library. 

[![Platform](https://img.shields.io/cocoapods/p/librws.svg?style=flat)](http://cocoapods.org/pods/librws)
[![Version](https://img.shields.io/cocoapods/v/librws.svg?style=flat)](http://cocoapods.org/pods/librws)
[![License](https://img.shields.io/cocoapods/l/librws.svg?style=flat)](http://cocoapods.org/pods/librws)
[![Build Status](https://travis-ci.org/OlehKulykov/librws.svg?branch=master)](https://travis-ci.org/OlehKulykov/librws) 
[![Build status](https://ci.appveyor.com/api/projects/status/9f8032rmlbrtaa9o?svg=true)](https://ci.appveyor.com/project/OlehKulykov/librws)


### Features
* No additional dependecies
* Single header library interface ```librws.h``` with public methods
* Thread safe
* Send/receive logic in background thread


### Installation with CocoaPods
#### Podfile
```ruby
pod 'librws'
```


### Example
##### Create and store websocket object handle
```c
  // Define variable or field for socket handle
  rws_socket _socket = NULL;
  ............
  // Create socket object
  _socket = rws_socket_create();
```
##### Set websocket connection url
```c
// Combined url: "ws://echo.websocket.org:80/"
rws_socket_set_scheme(_socket, "ws");
rws_socket_set_host(_socket, "echo.websocket.org");
rws_socket_set_port(_socket, 80);
rws_socket_set_path(_socket, "/");
```
##### Set websocket responce callbacks
Warning: ```rws_socket_set_on_disconnected``` is required
```c
// Main callbacks functions
// callback trigered on socket disconnected with/without error
static void on_socket_disconnected(rws_socket socket) 
{
  // process error
  rws_error error = rws_socket_get_error(socket);
  if (error) printf("\nSocket disconnect with code, error: %i, %s", rws_error_get_code(error), rws_error_get_description(error));
  // forget about this socket object, due to next disconnection sequence
  _socket = NULL;
}
// callback trigered on socket connected and handshake done
static void on_socket_connected(rws_socket socket)
{
  printf("\nSocket connected");
}
// callback trigered on socket received text
static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length)
{
  printf("\nSocket text: %s", text);
}
..................
// Set socket callbacks
rws_socket_set_on_disconnected(_socket, &on_socket_disconnected); // required
rws_socket_set_on_connected(_socket, &on_socket_connected);
rws_socket_set_on_received_text(_socket, &on_socket_received_text);
```
##### Connect
```c
rws_socket_connect(_socket);
```
##### Send message to websocket
```c
const char * example_text =
  "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";
rws_socket_send_text(_socket, example_text);
```
##### Disconnect or delete websocket object
Since socket can be connected and we need to send disconnect mesage, not just lazy close, need to wait and than delete object.
Thats why just call ```rws_socket_disconnect_and_release```, its thread safe, and forget about this socket object.
```c
rws_socket_disconnect_and_release(_socket);
_socket = NULL;
```


### License
---------

The MIT License (MIT)

Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
