/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include <stdlib.h>
#include <assert.h>
#include <string.h>


#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif

#if defined(XCODE)
#include "librws.h"
#else
#include <librws.h>
#endif

#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif

int main(int argc, char* argv[])
{
	const char * scheme = "ws";
	const char * host = "echo.websocket.org";
	const char * path = "/";
	
	rws_socket socket = rws_socket_create();
	assert(socket);


	rws_socket_set_scheme(socket, scheme);								printf("%i\n", (int)__LINE__);
	assert(strcmp(rws_socket_get_scheme(socket), scheme) == 0);			printf("%i\n", (int)__LINE__);

	rws_socket_set_host(socket, host);									printf("%i\n", (int)__LINE__);
	assert(strcmp(rws_socket_get_host(socket), host) == 0);				printf("%i\n", (int)__LINE__);

	rws_socket_set_path(socket, path);									printf("%i\n", (int)__LINE__);
	assert(strcmp(rws_socket_get_path(socket), path) == 0);				printf("%i\n", (int)__LINE__);

	rws_socket_set_port(socket, 80);									printf("%i\n", (int)__LINE__);
	assert(rws_socket_get_port(socket) == 80);							printf("%i\n", (int)__LINE__);

	rws_socket_set_port(socket, 443);									printf("%i\n", (int)__LINE__);
	assert(rws_socket_get_port(socket) == 443);							printf("%i\n", (int)__LINE__);



	rws_socket_set_scheme(socket, NULL);								printf("%i\n", (int)__LINE__);
	assert(rws_socket_get_scheme(socket) == NULL);						printf("%i\n", (int)__LINE__);

	rws_socket_set_host(socket, NULL);									printf("%i\n", (int)__LINE__);
	assert(rws_socket_get_host(socket) == NULL);						printf("%i\n", (int)__LINE__);

	rws_socket_set_path(socket, NULL);									printf("%i\n", (int)__LINE__);
	assert(rws_socket_get_path(socket) == NULL);						printf("%i\n", (int)__LINE__);


	rws_socket_disconnect_and_release(socket);

	return 0;
}

