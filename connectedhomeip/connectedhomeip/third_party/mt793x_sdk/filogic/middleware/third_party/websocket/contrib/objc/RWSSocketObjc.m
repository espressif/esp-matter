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


#import "RWSSocketObjc.h"
#include "../../librws.h"

@interface RWSSocketObjc()
{
@private
	rws_socket _socket;
	__strong NSURL * _url;
}

@end

@implementation RWSSocketObjc

- (void) onDisconnected
{
	rws_error error = rws_socket_get_error(_socket);
	_socket = NULL;

	NSError * err = nil;
	if (error) err = [NSError errorWithDomain:@"RWSSocket"
										 code:rws_error_get_code(error)
									 userInfo:@{ NSLocalizedDescriptionKey : [NSString stringWithUTF8String:rws_error_get_description(error)] }];

	id<RWSSocketObjcDelegate> d = self.delegate;
	if (d && [d respondsToSelector:@selector(onRWSSocketDisconnected:withError:)])
	{
		dispatch_async(dispatch_get_main_queue(), ^{ [d onRWSSocketDisconnected:self withError:err]; });
	}
}

static void onRWSSocketObjcConnected(rws_socket socket)
{
	RWSSocketObjc * s = (__bridge RWSSocketObjc *)rws_socket_get_user_object(socket);
	id<RWSSocketObjcDelegate> d = s ? [s delegate] : nil;
	if (d && [d respondsToSelector:@selector(onRWSSocketConnected:)])
	{
		dispatch_async(dispatch_get_main_queue(), ^{ [d onRWSSocketConnected:s]; });
	}
}

static void onRWSSocketObjcDisconnected(rws_socket socket)
{
	RWSSocketObjc * s = (__bridge RWSSocketObjc *)rws_socket_get_user_object(socket);
	if (s) [s onDisconnected];
}

static void onRWSSocketObjcRecvdText(rws_socket socket, const char * text, const unsigned int length)
{
	RWSSocketObjc * s = (__bridge RWSSocketObjc *)rws_socket_get_user_object(socket);
	id<RWSSocketObjcDelegate> d = s ? [s delegate] : nil;
	if (text && length && d && [d respondsToSelector:@selector(onRWSSocket:receivedText:)])
	{
		NSString * t = [NSString stringWithUTF8String:text];
		dispatch_async(dispatch_get_main_queue(), ^{ [d onRWSSocket:s receivedText:t]; });
	}
}

static void onRWSSocketObjcRecvdBin(rws_socket socket, const void * data, const unsigned int length)
{
	RWSSocketObjc * s = (__bridge RWSSocketObjc *)rws_socket_get_user_object(socket);
	id<RWSSocketObjcDelegate> d = s ? [s delegate] : nil;
	if (data && length && d && [d respondsToSelector:@selector(onRWSSocket:receivedData:)])
	{
		NSData * dt = [NSData dataWithBytes:data length:length];
		dispatch_async(dispatch_get_main_queue(), ^{ [d onRWSSocket:s receivedData:dt]; });
	}
}

- (BOOL) sendText:(NSString *) text
{
	if (text && [text length])
	{
		if (rws_socket_send_text(_socket, [text UTF8String])) return YES;
	}
	return NO;
}

- (void) cleanup
{
	if (_socket)
	{
		rws_socket_set_user_object(_socket, NULL);
		rws_socket_disconnect_and_release(_socket);
		_socket = NULL;
	}
}

- (BOOL) connect
{
	[self cleanup];
	if (!_url) return NO;

	_socket = rws_socket_create();
	rws_socket_set_scheme(_socket, [[_url scheme] UTF8String]);
	rws_socket_set_host(_socket, [[_url host] UTF8String]);
	rws_socket_set_port(_socket, [[_url port] intValue]);

	NSString * path = [_url path];
	if (!path || [path length] == 0) path = @"/";
	rws_socket_set_path(_socket, [path UTF8String]);

	rws_socket_set_user_object(_socket, (__bridge void *)self);

	rws_socket_set_on_disconnected(_socket, &onRWSSocketObjcDisconnected);
	rws_socket_set_on_connected(_socket, &onRWSSocketObjcConnected);
	rws_socket_set_on_received_text(_socket, &onRWSSocketObjcRecvdText);
	rws_socket_set_on_received_bin(_socket, &onRWSSocketObjcRecvdBin);

	return rws_socket_connect(_socket) ? YES : NO;
}

- (BOOL) isConnected
{
	return rws_socket_is_connected(_socket) ? YES : NO;
}

- (nullable id) initWithURL:(nonnull NSURL *) url
{
	self = [super init];
	if (self && url)
	{
		_url = url;
		return self;
	}
	return nil;
}

- (id) init
{
	return nil;
}

- (void) dealloc
{
	[self cleanup];
}

@end
