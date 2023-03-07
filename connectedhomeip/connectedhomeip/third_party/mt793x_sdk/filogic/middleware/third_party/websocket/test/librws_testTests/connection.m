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

@interface connection : XCTestCase
{
@private
	rws_socket * _socket;
}

@property (nonatomic) BOOL isWorking;
@property (nonatomic) BOOL isError;
@property (nonatomic) BOOL isFinished;
@property (nonatomic, strong) NSString * errorMessage;

@end

@implementation connection

static const char * _testSendText = "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";

static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length)
{
	connection * con = (__bridge connection *)rws_socket_get_user_object(socket);

	if (strncmp(text, _testSendText, length) != 0)
	{
		con.isError = YES;
		con.errorMessage = @"send and echo test/len not equal";
	}
	con.isWorking = NO;
	con.isFinished = YES;
}

static void on_socket_connected(rws_socket socket)
{
	NSLog(@"Socket connected");

	rws_socket_send_text(socket, _testSendText);
}

static void on_socket_disconnected(rws_socket socket)
{
	rws_error error = rws_socket_get_error(socket);
	if (error)
	{
		NSLog(@"Socket disconnect with code, error: %i, %s",
			  rws_error_get_code(error),
			  rws_error_get_description(error));
	}
	connection * con = (__bridge connection *)rws_socket_get_user_object(socket);
	con.errorMessage = @"can't connect to host";
	con.isError = YES;
	con.isWorking = NO;
	con.isFinished = YES;
}

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.

	_socket = rws_socket_create();
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];

	rws_socket_disconnect_and_release(_socket);
	_socket = NULL;
}

- (void) testConnection
{
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.

	rws_socket_set_url(_socket, "ws", "echo.websocket.org", 80, "/");
	rws_socket_set_user_object(_socket, (__bridge void *)self);

	rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
	rws_socket_set_on_connected(_socket, &on_socket_connected);
	rws_socket_set_on_received_text(_socket, &on_socket_received_text);

	self.isWorking = YES;
	self.isError = NO;
	self.isFinished = NO;
	rws_socket_connect(_socket);

	while (self.isWorking)
	{
		rws_thread_sleep(100);
	}
	XCTAssertFalse(self.isError, @"error during connection");
	XCTAssertTrue(self.isFinished, @"connection test not finished");
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
