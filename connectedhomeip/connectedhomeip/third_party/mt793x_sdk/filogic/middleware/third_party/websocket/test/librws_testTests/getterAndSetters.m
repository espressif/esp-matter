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


#import <XCTest/XCTest.h>
#import "librws.h"

@interface getterAndSetters : XCTestCase
{
@private
	rws_socket * _socket;
}
@end

@implementation getterAndSetters

- (void) setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
	_socket = rws_socket_create();
}

- (void) tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
	rws_socket_disconnect_and_release(_socket);
	_socket = NULL;
}

- (void) testSetURL
{
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.

	rws_socket_set_url(_socket, "ws", "host", 80, "/");

	XCTAssert(strcmp(rws_socket_get_scheme(_socket), "ws") == 0, @"set get scheme");
	XCTAssert(strcmp(rws_socket_get_host(_socket), "host") == 0, @"set get host");
	XCTAssert(strcmp(rws_socket_get_path(_socket), "/") == 0, @"set get path");
	XCTAssert(rws_socket_get_port(_socket) == 80, @"set get port");

	rws_socket_set_url(_socket, NULL, NULL, -2, NULL);
	XCTAssert(rws_socket_get_scheme(_socket) == NULL, @"set get scheme");
	XCTAssert(rws_socket_get_host(_socket) == NULL, @"set get host");
	XCTAssert(rws_socket_get_path(_socket) == NULL, @"set get path");
	XCTAssert(rws_socket_get_port(_socket) == -2, @"set get port");

	XCTAssert(rws_socket_get_scheme(NULL) == NULL, @"set get scheme");
	XCTAssert(rws_socket_get_host(NULL) == NULL, @"set get host");
	XCTAssert(rws_socket_get_path(NULL) == NULL, @"set get path");
	XCTAssert(rws_socket_get_port(NULL) < 0, @"set get port");

	rws_socket_set_scheme(_socket, "ws1");
	rws_socket_set_scheme(NULL, "ws2");
	rws_socket_set_scheme(NULL, NULL);
	XCTAssert(strcmp(rws_socket_get_scheme(_socket), "ws1") == 0, @"set get scheme");
	rws_socket_set_scheme(_socket, NULL);
	XCTAssert(rws_socket_get_scheme(_socket) == NULL, @"set get scheme");

	rws_socket_set_host(_socket, "host1");
	rws_socket_set_host(NULL, "host1");
	rws_socket_set_host(NULL, NULL);
	XCTAssert(strcmp(rws_socket_get_host(_socket), "host1") == 0, @"set get host");
	rws_socket_set_host(_socket, NULL);
	XCTAssert(rws_socket_get_host(_socket) == NULL, @"set get host");

	rws_socket_set_path(_socket, "/path1");
	rws_socket_set_path(NULL, "/path1");
	rws_socket_set_path(NULL, NULL);
	XCTAssert(strcmp(rws_socket_get_path(_socket), "/path1") == 0, @"set get path");
	rws_socket_set_path(_socket, NULL);
	XCTAssert(rws_socket_get_path(_socket) == NULL, @"set get path");


	XCTAssert(rws_socket_get_error(_socket) == NULL, @"set get error");
	XCTAssert(rws_socket_get_error(NULL) == NULL, @"set get error");


	XCTAssertFalse(rws_socket_is_connected(_socket), "check is connected without connect");
	XCTAssertFalse(rws_socket_is_connected(NULL), "check is connected without connect");


	rws_socket_set_user_object(_socket, (__bridge void *)self);
	rws_socket_set_user_object(NULL, NULL);
	id thisTestClass = (__bridge id)(rws_socket_get_user_object(_socket));
	XCTAssertEqualObjects(self, thisTestClass, @"user object incorrect");
	XCTAssert(rws_socket_get_user_object(NULL) == NULL, @"user object incorrect");

	rws_socket_set_user_object(_socket, NULL);
	XCTAssert(rws_socket_get_user_object(_socket) == NULL, @"user object incorrect");
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
