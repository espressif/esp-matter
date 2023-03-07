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


#import <Foundation/Foundation.h>

@class RWSSocketObjc;

/**
 @brief Websocket delegate protocol.
 All methods called from main thread.
 */
@protocol RWSSocketObjcDelegate <NSObject>

@required
/**
 @brief Websocket connected.
 @detailed Connection extablished and handshake done. 
 Web socket ready to send and receive.
 @param socket Socket object.
 */
- (void) onRWSSocketConnected:(nonnull RWSSocketObjc *) socket;


/**
 @brief Socket client disconnected.
 @detailed Internal socket already freed and dealocated. 
 Connect once again.
 @param socket Socket object.
 @param error Disconnect error.
 */
- (void) onRWSSocketDisconnected:(nonnull RWSSocketObjc *) socket
					   withError:(nullable NSError *) error;

@optional
/**
 @brief Socket received non empty text.
 @param socket Socket object.
 @param text Non empty text.
 */
- (void) onRWSSocket:(nonnull RWSSocketObjc *) socket receivedText:(nonnull NSString *) text;



/**
 @brief Socket received non empty binary data.
 @param socket Socket object.
 @param text Non binary data.
 */
- (void) onRWSSocket:(nonnull RWSSocketObjc *) socket receivedData:(nonnull NSData *) data;

@end


/**
 @brief Objective-C web socket wrapper.
 @detailed Internal implementation use C library part.
 */
@interface RWSSocketObjc : NSObject


/**
 @brief Weak read/write reference to delegate object.
 */
@property (nonatomic, weak) id<RWSSocketObjcDelegate> delegate;


/**
 @brief Check websocket exists and connected.
 */
@property (nonatomic, readonly) BOOL isConnected;


/**
 @brief Send text to connected web socket.
 @detailed Add text frame to send queue.
 @param text Text for sending.
 @return YES - socket exist, connected and text not empty, othervice NO.
 */
- (BOOL) sendText:(nonnull NSString *) text;


/**
 @brief Connect to server.
 @detailed Remove previos socket object and make new connection.
 @return YES - started connection sequence, othervice NO.
 */
- (BOOL) connect;


/**
 @brief Initialize web socket with destination url.
 @param url Connection URL. Should not be nil.
 @return Websocket object or nil on error.
 */
- (nullable id) initWithURL:(nonnull NSURL *) url;


@end
