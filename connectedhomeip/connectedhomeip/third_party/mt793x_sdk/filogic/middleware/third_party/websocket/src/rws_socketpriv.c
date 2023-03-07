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


#include "../librws.h"
#include "rws_socket.h"
#include "rws_memory.h"
#include "rws_string.h"
#include <assert.h>


#if defined(RWS_MTK)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "task_def.h"
#endif

static const char * k_rws_socket_min_http_ver = "1.1";
static const char * k_rws_socket_sec_websocket_accept = "Sec-Websocket-Accept";

#define RWS_CONNECT_RETRY_DELAY 200
#define RWS_CONNECT_ATTEMPS 5

#ifndef  RWS_OS_WINDOWS
#define	WSAEWOULDBLOCK  EAGAIN
#define	WSAEINPROGRESS     EINPROGRESS
#endif

unsigned int rws_socket_get_next_message_id(_rws_socket * s)
{
	const unsigned int mess_id = ++s->next_message_id;
	if (mess_id > 9999999) s->next_message_id = 0;
	return mess_id;
}

void rws_socket_send_ping(_rws_socket * s)
{
	char buff[16];
	size_t len = 0;
	_rws_frame * frame = NULL;

	rws_mutex_lock(s->send_mutex);
	frame = rws_frame_create();
	if (frame == NULL) {
		WS_DBG("rws_frame_create fail");
		rws_mutex_unlock(s->send_mutex);
		return;
	}

	len = rws_sprintf(buff, 16, "%u", rws_socket_get_next_message_id(s));

	WS_DBG("%s, buff: %s", __FUNCTION__, buff);

	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_ping;
	rws_frame_fill_with_send_data(frame, buff, len);
	if (frame->data == NULL) {
		WS_DBG("rws_frame_fill_with_send_data fail");
		rws_frame_delete(frame);
	}
	else {
		rws_socket_append_send_frames(s, frame);
	}	
	rws_mutex_unlock(s->send_mutex);
}

void rws_socket_inform_recvd_frames(_rws_socket * s)
{
	rws_bool is_all_finished = rws_true;
	_rws_frame * frame = NULL;
	_rws_node * cur = s->recvd_frames;

	while (cur)
	{
		frame = (_rws_frame *)cur->value.object;
		if (frame)
		{
			if (frame->is_finished)
			{
				switch (frame->opcode)
				{
					case rws_opcode_text_frame:
                        {
                            WS_DBG("%s: run on_recvd_text callback", __FUNCTION__);
						    if (s->on_recvd_text) s->on_recvd_text(s, (const char *)frame->data, (unsigned int)frame->data_size);
                        }
						break;
					case rws_opcode_binary_frame:
                        {
                            WS_DBG("%s: run on_recvd_bin callback", __FUNCTION__);
						    if (s->on_recvd_bin) s->on_recvd_bin(s, frame->data, (unsigned int)frame->data_size);
                        }
						break;
					default: break;
				}
				rws_frame_delete(frame);
				cur->value.object = NULL;
			}
			else
			{
				is_all_finished = rws_false;
			}
		}
		cur = cur->next;
	}
	if (is_all_finished) rws_list_delete_clean(&s->recvd_frames);
}

void rws_socket_read_handshake_responce_value(const char * str, char ** value)
{
	const char * s = NULL;
	size_t len = 0;

	while (*str == ':' || *str == ' ') { str++; }
	s = str;
	while (*s != '\r' && *s != '\n') { s++; len++; }
	if (len > 0) *value = rws_string_copy_len(str, len);
}

rws_bool rws_socket_process_handshake_responce(_rws_socket * s)
{
	const char * str = (const char *)s->received;
	const char * sub = NULL;
#if !defined(RWS_MTK)
	float http_ver = -1;
#endif
	int http_code = -1;

    WS_DBG("%s: s->received=%s", __FUNCTION__, s->received ? s->received : "");

	rws_error_delete_clean(&s->error);

	sub = strstr(str, "HTTP/");
	if (!sub) return rws_false;
    sub += 5;

#if defined(RWS_MTK)
    sub = strchr(sub, ' ');
	if (!sub) return rws_false;
    sub++;

    /* sscanf() in IAR does not work as the same as the one in GCC and Keil. */
    if (rws_sscanf(sub, "%d", &http_code) != 1)
	{
		http_code = -1;
	}
#else
	if (rws_sscanf(sub, "%f %i", &http_ver, &http_code) != 2)
	{
		http_ver = -1; http_code = -1;
	}
#endif

	sub = strstr(str, "Sec-WebSocket-Accept"); // "Sec-WebSocket-Accept"
	if (sub)
	{
		sub += strlen(k_rws_socket_sec_websocket_accept);
		rws_socket_read_handshake_responce_value(sub, &s->sec_ws_accept);
	}

	if (http_code != 101 || !s->sec_ws_accept)
	{
		s->error = rws_error_new_code_descr(rws_error_code_parse_handshake,
											(http_code != 101) ? "HTPP code not found or non 101" : "Accept key not found");
        WS_DBG("%s: http_code:%d, code:%d, error:%s", __FUNCTION__, http_code, s->error->code, s->error->description);
		return rws_false;
	}
	return rws_true;
}

// need close socket on error
rws_bool rws_socket_send(_rws_socket * s, const void * data, const size_t data_size)
{
	int sended = -1, error_number = -1;
	size_t sended_size = 0;
	const uint8_t *send_data = (const uint8_t *)data;

    //WS_DBG("%s: data[size:%d]:%s", __FUNCTION__, data_size, data ? data : "");

	rws_error_delete_clean(&s->error);

	while (sended_size < data_size) {
        //errno = -1;
#ifdef MTK_WEBSOCKET_SSL_ENABLE
        if(s->scheme && strcmp(s->scheme, "wss") == 0)
            sended = mbedtls_ssl_write(&(s->ssl->ssl_ctx), (const unsigned char *)send_data, (int)(data_size - sended_size));
        else
#endif /* MTK_WEBSOCKET_SSL_ENABLE */
            sended = (int)send(s->socket, send_data, (int)(data_size - sended_size), 0);

#if defined(RWS_OS_WINDOWS)
        error_number = WSAGetLastError();
#else
        error_number = errno;
#endif

        if (sended > 0) {
            sended_size += sended;
            send_data = send_data + sended;
        } else {
            rws_socket_check_write_error(s, error_number);
            if (s->error)
            {
                rws_socket_close(s);
                return rws_false;
            }
        }

	}

	return rws_true;
}

rws_bool rws_socket_recv(_rws_socket * s)
{
	int is_reading = 1, error_number = -1, len = -1;
	char * received = NULL;
	size_t total_len = 0;
	size_t new_size = 0;
#if !defined(RWS_MTK)
	char buff[8192];
#else
	char *buff = (char *)rws_malloc(8192);

    if (!buff)
    {
        WS_DBG("%s: %s", __FUNCTION__, "Not enough memory");
        return rws_false;
    }
#endif

	rws_error_delete_clean(&s->error);
	while (is_reading)
	{
    #ifdef MTK_WEBSOCKET_SSL_ENABLE
        if(s->scheme && strcmp(s->scheme, "wss") == 0)
            len = mbedtls_ssl_read(&(s->ssl->ssl_ctx), (unsigned char *)buff, 8192);
        else
    #endif /* MTK_WEBSOCKET_SSL_ENABLE */
		    len = (int)recv(s->socket, buff, 8192, 0);
    #if defined(RWS_OS_WINDOWS)
        error_number = WSAGetLastError();
    #else
        error_number = errno;
    #endif

		if (len > 0)
		{
			total_len += len;
			if (s->received_size-s->received_len < len)
			{
				new_size = s->received_size + len;
				rws_socket_resize_received(s, s->received_size+len);
				if (new_size != s->received_size) {
					WS_DBG("rws_socket_resize_received fail");
					continue;
				}
			}
			received = (char *)s->received;
			if (s->received_len) received += s->received_len;
			memcpy(received, buff, len);
			s->received_len += len;
		}
		else
		{
			is_reading = 0;
		}
	}
#if defined(RWS_MTK)
    rws_free(buff);
#endif
	//if (error_number < 0) return rws_true;
	if (error_number != WSAEWOULDBLOCK && error_number != WSAEINPROGRESS)
	{
		s->error = rws_error_new_code_descr(rws_error_code_read_write_socket, "Failed read/write socket");
        WS_DBG("%s: errno:%d, code:%d, error:%s", __FUNCTION__, error_number, s->error->code, s->error->description);
		rws_socket_close(s);
		return rws_false;
	}
	return rws_true;
}

_rws_frame * rws_socket_last_unfin_recvd_frame_by_opcode(_rws_socket * s, const rws_opcode opcode)
{
	_rws_frame * last = NULL;
	_rws_frame * frame = NULL;
	_rws_node * cur = s->recvd_frames;
	while (cur)
	{
		frame = (_rws_frame *)cur->value.object;
		if (frame)
		{
            //  [FIN=0,opcode !=0 ],[FIN=0,opcode ==0 ],....[FIN=1,opcode ==0 ]
			if (!frame->is_finished /*&& frame->opcode == opcode*/) last = frame;
		}
		cur = cur->next;
	}
	return last;
}

void rws_socket_process_bin_or_text_frame(_rws_socket * s, _rws_frame * frame)
{
	_rws_frame * last_unfin = rws_socket_last_unfin_recvd_frame_by_opcode(s, frame->opcode);
	if (last_unfin)
	{
		rws_frame_combine_datas(last_unfin, frame);
		last_unfin->is_finished = frame->is_finished;
		rws_frame_delete(frame);
	}
	else
	{
		if (frame->data && frame->data_size) rws_socket_append_recvd_frames(s, frame);
		else rws_frame_delete(frame);
	}
}

void rws_socket_process_ping_frame(_rws_socket * s, _rws_frame * frame)
{
	_rws_frame * pong_frame = NULL;

	rws_mutex_lock(s->send_mutex);
	pong_frame = rws_frame_create();
	if (pong_frame == NULL) {
		WS_DBG("rws_frame_create fail");
		rws_mutex_unlock(s->send_mutex);
		rws_frame_delete(frame);
		return;
	}
	pong_frame->opcode = rws_opcode_pong;
	pong_frame->is_masked = rws_true;
	rws_frame_fill_with_send_data(pong_frame, frame->data, frame->data_size);
	rws_frame_delete(frame);
	if (pong_frame->data == NULL) {
		WS_DBG("rws_frame_fill_with_send_data fail");
		rws_frame_delete(pong_frame);
	}
	else {
		rws_socket_append_send_frames(s, pong_frame);
	}	
	rws_mutex_unlock(s->send_mutex);
}

void rws_socket_process_conn_close_frame(_rws_socket * s, _rws_frame * frame)
{
	s->command = COMMAND_INFORM_DISCONNECTED;
	s->error = rws_error_new_code_descr(rws_error_code_connection_closed, "Connection was closed by endpoint");
    WS_DBG("%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description);
	//rws_socket_close(s);
	rws_frame_delete(frame);
}

void rws_socket_process_received_frame(_rws_socket * s, _rws_frame * frame)
{
    WS_DBG("%s: frame->opcode:%d", __FUNCTION__, frame->opcode);
	switch (frame->opcode)
	{
		case rws_opcode_ping: rws_socket_process_ping_frame(s, frame); break;
		case rws_opcode_text_frame:
		case rws_opcode_binary_frame:
        case rws_opcode_continuation:
			rws_socket_process_bin_or_text_frame(s, frame);
			break;
		case rws_opcode_connection_close: rws_socket_process_conn_close_frame(s, frame); break;
		default:
			// unprocessed => delete
			rws_frame_delete(frame);
			break;
	}
}

void rws_socket_idle_recv(_rws_socket * s)
{
	_rws_frame * frame = NULL;

	if (!rws_socket_recv(s))
	{
		// sock already closed
		if (s->error) s->command = COMMAND_INFORM_DISCONNECTED;
		return;
	}

   const size_t nframe_size = rws_check_recv_frame_size(s->received, s->received_len);
   if (nframe_size)
   {
       frame = rws_frame_create_with_recv_data(s->received, nframe_size);
       if (frame)
       {
           WS_DBG("%s", __FUNCTION__);
           rws_socket_process_received_frame(s, frame);
       }
	   else {
		   WS_DBG("rws_frame_create_with_recv_data fail");
	   }
       if (nframe_size == s->received_len)
       {
           s->received_len = 0;
       }
       else if (s->received_len > nframe_size)
       {
           const size_t nLeftLen = s->received_len - nframe_size;
           memmove((char*)s->received, (char*)s->received + nframe_size, nLeftLen);
           s->received_len = nLeftLen;
       }
   }
}

void rws_socket_idle_send(_rws_socket * s)
{
	_rws_node * cur = NULL;
	rws_bool sending = rws_true;
	_rws_frame * frame = NULL;

	rws_mutex_lock(s->send_mutex);
	cur = s->send_frames;
	if (cur)
	{
            WS_DBG("%s", __FUNCTION__);
		while (cur)
		{
			frame = (_rws_frame *)cur->value.object;
			cur->value.object = NULL;
			if (frame) {
                if (s->is_connected && sending) {
                    sending = rws_socket_send(s, frame->data, frame->data_size);
                }
    			rws_frame_delete(frame);
			}
			cur = cur->next;
		}
		rws_list_delete_clean(&s->send_frames);
		if (s->error) s->command = COMMAND_INFORM_DISCONNECTED;
	}
	rws_mutex_unlock(s->send_mutex);
}

void rws_socket_wait_handshake_responce(_rws_socket * s)
{
    //WS_DBG("%s", __FUNCTION__);

	if (!rws_socket_recv(s))
	{
		// sock already closed
		if (s->error) s->command = COMMAND_INFORM_DISCONNECTED;
		return;
	}
	if (s->received_len == 0) return;

	if (rws_socket_process_handshake_responce(s))
	{
        s->received_len = 0;
		s->is_connected = rws_true;
		s->command = COMMAND_INFORM_CONNECTED;
	}
	else
	{
		rws_socket_close(s);
		s->command = COMMAND_INFORM_DISCONNECTED;
	}
}

void rws_socket_send_disconnect(_rws_socket * s)
{
	char buff[16];

    WS_DBG("%s, soc_id:%d", __FUNCTION__, s->socket);

	size_t len = 0;
	_rws_frame * frame = rws_frame_create();

	if (frame) {
		len = rws_sprintf(buff, 16, "%u", rws_socket_get_next_message_id(s));

		frame->is_masked = rws_true;
		frame->opcode = rws_opcode_connection_close;
		rws_frame_fill_with_send_data(frame, buff, len);
		if (frame->data == NULL) {
			WS_DBG("rws_frame_fill_with_send_data fail");
		}
		else {
			rws_socket_send(s, frame->data, frame->data_size);
		}		
		rws_frame_delete(frame);
	}
	else {
		WS_DBG("rws_frame_create fail");
	}
	
	s->command = COMMAND_END;
	rws_thread_sleep(RWS_CONNECT_RETRY_DELAY); // little bit wait after send message
}

void rws_socket_send_handshake(_rws_socket * s)
{
#if defined(RWS_MTK)
	#define HS_BUF_SIZE 2048
    char *buff = NULL;
    char * ptr = NULL;
#else
	#define HS_BUF_SIZE 512
    char buff[HS_BUF_SIZE];
    char * ptr = buff;
#endif

	size_t writed = 0;

    WS_DBG("%s", __FUNCTION__);
#if defined(RWS_MTK)
    buff = (char *)rws_malloc(HS_BUF_SIZE);
    if (!buff)
    {
        WS_DBG("%s: %s", __FUNCTION__, "Not enough memory");
        return;
    }
    ptr = buff;
#endif

	writed = rws_sprintf(ptr, HS_BUF_SIZE, "GET %s HTTP/%s\r\n", s->path, k_rws_socket_min_http_ver);

//	ptr += sprintf(ptr, "Host: %s\r\n", s->host);

	if (s->port == 80) writed += rws_sprintf(ptr + writed, HS_BUF_SIZE - writed, "Host: %s\r\n", s->host);
	else writed += rws_sprintf(ptr + writed, HS_BUF_SIZE - writed, "Host: %s:%i\r\n", s->host, s->port);

	writed += rws_sprintf(ptr + writed, HS_BUF_SIZE - writed,
						  "Upgrade: websocket\r\n"
						  "Connection: Upgrade\r\n"
						  "Origin: %s://%s\r\n",
						  s->scheme, s->host);
	
#if defined(RWS_MTK)
	if (s->user_header) {
		writed += rws_sprintf(ptr + writed, HS_BUF_SIZE - writed,
								"%s\r\n",
								s->user_header);
	}
#endif

	writed += rws_sprintf(ptr + writed, HS_BUF_SIZE - writed,
						  "Sec-WebSocket-Key: %s\r\n"
						  "Sec-WebSocket-Protocol: chat, superchat\r\n"
						  "Sec-WebSocket-Version: 13\r\n"
						  "\r\n",
						  "dGhlIHNhbXBsZSBub25jZQ==");

	if (rws_socket_send(s, buff, writed))
	{
		s->command = COMMAND_WAIT_HANDSHAKE_RESPONCE;
	}
	else
	{
		if (s->error) s->error->code = rws_error_code_send_handshake;
		else s->error = rws_error_new_code_descr(rws_error_code_send_handshake, "Send handshake");
                WS_DBG("%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description);
		rws_socket_close(s);
		s->command = COMMAND_INFORM_DISCONNECTED;
	}

#if defined(RWS_MTK)
	rws_string_delete_clean(&s->user_header);
    rws_free(buff);
#endif
}

struct addrinfo * rws_socket_connect_getaddr_info(_rws_socket * s)
{
	struct addrinfo hints;
	char portstr[16];
	struct addrinfo * result = NULL;
	int ret = 0, retry_number = 0;
#if !defined(RWS_MTK)
    int last_ret = 0
#endif
#if defined(RWS_OS_WINDOWS)
	WSADATA wsa;
#endif

	rws_error_delete_clean(&s->error);

#if defined(RWS_OS_WINDOWS)
	memset(&wsa, 0, sizeof(WSADATA));
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
	{
		s->error = rws_error_new_code_descr(rws_error_code_connect_to_host, "Failed initialise winsock");
		s->command = COMMAND_INFORM_DISCONNECTED;
		return NULL;
	}
#endif

	rws_sprintf(portstr, 16, "%i", s->port);
	while (++retry_number < RWS_CONNECT_ATTEMPS)
	{
		result = NULL;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		ret = getaddrinfo(s->host, portstr, &hints, &result);
		if (ret == 0 && result) return result;
#if !defined(RWS_MTK)
		if (ret != 0) last_ret = ret;
#endif
		if (result) freeaddrinfo(result);
		rws_thread_sleep(RWS_CONNECT_RETRY_DELAY);
	}

#if defined(RWS_OS_WINDOWS)
	WSACleanup();
#endif

#if defined(RWS_MTK) // not support gai_strerror()
	s->error = rws_error_new_code_descr(rws_error_code_connect_to_host, "Failed connect to host");
#else
	s->error = rws_error_new_code_descr(rws_error_code_connect_to_host,
										(last_ret > 0) ? gai_strerror(last_ret) : "Failed connect to host");
#endif
    WS_DBG("%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description);
	s->command = COMMAND_INFORM_DISCONNECTED;
	return NULL;
}

void rws_socket_connect_to_host(_rws_socket * s)
{
	struct addrinfo * result = NULL;
	struct addrinfo * p = NULL;
	rws_socket_t sock = RWS_INVALID_SOCKET;
	int retry_number = 0;
#if defined(RWS_OS_WINDOWS)
	unsigned long iMode = 0;
#endif

    WS_DBG("%s", __FUNCTION__);

	result = rws_socket_connect_getaddr_info(s);
	if (!result) return;

	while ((++retry_number < RWS_CONNECT_ATTEMPS) && (sock == RWS_INVALID_SOCKET))
	{
		for (p = result; p != NULL; p = p->ai_next)
		{
			sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (sock != RWS_INVALID_SOCKET)
			{
				rws_socket_set_option(sock, SO_ERROR, 1); // When an error occurs on a socket, set error variable so_error and notify process
				rws_socket_set_option(sock, SO_KEEPALIVE, 1); // Periodically test if connection is alive

				if (connect(sock, p->ai_addr, p->ai_addrlen) == 0)
				{
                    s->received_len = 0;
					s->socket = sock;
                #if defined(RWS_OS_WINDOWS)
					// If iMode != 0, non-blocking mode is enabled.
					iMode = 1;
					ioctlsocket(s->socket, FIONBIO, &iMode);
                #else
					fcntl(s->socket, F_SETFL, O_NONBLOCK);
                #endif
					break;
				}
				RWS_SOCK_CLOSE(sock);
			}
		}
		if (sock == RWS_INVALID_SOCKET) rws_thread_sleep(RWS_CONNECT_RETRY_DELAY);
	}

	freeaddrinfo(result);

	if (s->socket == RWS_INVALID_SOCKET)
	{
    #if defined(RWS_OS_WINDOWS)
		WSACleanup();
    #endif
		s->error = rws_error_new_code_descr(rws_error_code_connect_to_host, "Failed connect to host");
        WS_DBG("%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description);
		s->command = COMMAND_INFORM_DISCONNECTED;
	}
	else
	{
		s->command = COMMAND_SEND_HANDSHAKE;
	}
}

static void rws_socket_work_th_func(void * user_object)
{
	_rws_socket * s = (_rws_socket *)user_object;
	size_t loop_number = 0;
    WS_DBG("%s", __FUNCTION__);
	while (s->command < COMMAND_END)
	{
		loop_number++;
		rws_mutex_lock(s->work_mutex);
		switch (s->command)
		{
			case COMMAND_CONNECT_TO_HOST:
                {
                #ifdef MTK_WEBSOCKET_SSL_ENABLE
                    if(s->scheme && strcmp(s->scheme, "wss") == 0) {
#if !defined(RWS_OS_WINDOWS)
                        rws_ssl_conn(s);
#else
                        WS_DBG("Windows is Not supported.");
                        s->socket == RWS_INVALID_SOCKET;
                        s->command = COMMAND_INFORM_DISCONNECTED;
#endif
                        break;
                    }
                    else
                #endif /* MTK_WEBSOCKET_SSL_ENABLE */
                    {
                        rws_socket_connect_to_host(s); break;
                    }
                }
			case COMMAND_SEND_HANDSHAKE: rws_socket_send_handshake(s); break;
			case COMMAND_WAIT_HANDSHAKE_RESPONCE: rws_socket_wait_handshake_responce(s); break;
			case COMMAND_DISCONNECT: rws_socket_send_disconnect(s); break;
			case COMMAND_IDLE:
				if (loop_number >= 400) { if (s->is_connected) rws_socket_send_ping(s); loop_number = 0; }
				if (s->is_connected) rws_socket_idle_send(s);
				if (s->is_connected) rws_socket_idle_recv(s);
				break;
			default: break;
		}
		rws_mutex_unlock(s->work_mutex);
		switch (s->command)
		{
			case COMMAND_INFORM_CONNECTED:
                {
                    WS_DBG("%s: run on_connected callback", __FUNCTION__);
				    s->command = COMMAND_IDLE;
				    if (s->on_connected) s->on_connected(s);
                }
				break;
			case COMMAND_INFORM_DISCONNECTED:
                {
                    s->command = COMMAND_END;
                    rws_socket_send_disconnect(s);
                    WS_DBG("%s: run on_disconnected callback", __FUNCTION__);
                    if (s->on_disconnected)
                    {
                        s->on_disconnected(s);
                    }
                }
				break;
			case COMMAND_IDLE:
				if (s->recvd_frames) rws_socket_inform_recvd_frames(s);
				break;
			default: break;
		}
        if (s->command < COMMAND_END) {
    		rws_thread_sleep(5);
        }
	}

    WS_DBG("Loop of thread ends. command:%d, soc_id:%d", s->command, s->socket);
	rws_socket_close(s);
	s->work_thread = NULL;
	rws_socket_delete(s);
}

rws_bool rws_socket_create_start_work_thread(_rws_socket * s)
{
    WS_DBG("%s", __FUNCTION__);
	rws_error_delete_clean(&s->error);
	s->command = COMMAND_NONE;
	s->work_thread = rws_thread_create(&rws_socket_work_th_func, s);
	if (s->work_thread)
	{
		s->command = COMMAND_CONNECT_TO_HOST;
		return rws_true;
	}
	return rws_false;
}

void rws_socket_resize_received(_rws_socket * s, const size_t size)
{
	void * res = NULL;
	size_t min = 0;
	if (size == s->received_size) return;

	res = rws_malloc(size);
	if (res == NULL) {
        WS_DBG("rws_socket_resize_received malloc fail %d", size);
		return;
	}
	assert(res && (size > 0));

	min = (s->received_size < size) ? s->received_size : size;
	if (min > 0 && s->received) memcpy(res, s->received, min);
	rws_free_clean(&s->received);
	s->received = res;
	s->received_size = size;
}

void rws_socket_close(_rws_socket * s)
{
    WS_DBG("%s, soc_id:%d", __FUNCTION__, s->socket);

    s->received_len = 0;
	if (s->socket != RWS_INVALID_SOCKET)
	{
	#ifdef MTK_WEBSOCKET_SSL_ENABLE
        if(s->scheme && strcmp(s->scheme, "wss") == 0)
            rws_ssl_close(s);
        else
        #endif /* MTK_WEBSOCKET_SSL_ENABLE */
		    RWS_SOCK_CLOSE(s->socket);

		s->socket = RWS_INVALID_SOCKET;
#if defined(RWS_OS_WINDOWS)
		WSACleanup();
#endif
	}
	s->is_connected = rws_false;
}

void rws_socket_append_recvd_frames(_rws_socket * s, _rws_frame * frame)
{
	_rws_node_value frame_list_var;
	frame_list_var.object = frame;
	if (s->recvd_frames)
	{
		rws_list_append(s->recvd_frames, frame_list_var);
	}
	else
	{
		s->recvd_frames = rws_list_create();
		if (s->recvd_frames == NULL) {
			rws_frame_delete(frame);
			WS_DBG("rws_list_create fail");
			return;
		}
		s->recvd_frames->value = frame_list_var;
	}
}

void rws_socket_append_send_frames(_rws_socket * s, _rws_frame * frame)
{
	_rws_node_value frame_list_var;
	frame_list_var.object = frame;
	if (s->send_frames)
	{
		rws_list_append(s->send_frames, frame_list_var);
	}
	else
	{
		s->send_frames = rws_list_create();
		if (s->send_frames == NULL) {
			rws_frame_delete(frame);
			WS_DBG("rws_list_create fail");
			return;
		}
		s->send_frames->value = frame_list_var;
	}
}

rws_bool rws_socket_send_text_priv(_rws_socket * s, const char * text)
{
	size_t len = text ? strlen(text) : 0;
	_rws_frame * frame = NULL;

    WS_DBG("%s: text: %s", __FUNCTION__, text ? text : "");

	if (len <= 0) return rws_false;

	frame = rws_frame_create();
	if (frame == NULL) {
		WS_DBG("rws_frame_create fail");
		return rws_false;
	}
	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_text_frame;
	rws_frame_fill_with_send_data(frame, text, len);
	if (frame->data == NULL) {
        WS_DBG("rws_frame_fill_with_send_data fail %s", len);
		rws_frame_delete(frame);
		return rws_false;
	}
	rws_socket_append_send_frames(s, frame);

	return rws_true;
}

rws_bool rws_socket_send_data_priv(_rws_socket * s, const void * data, const size_t data_size)
{
	_rws_frame * frame = NULL;
	if ((data == NULL)||(data_size<=0)) return rws_false;

	frame = rws_frame_create();
	if (frame == NULL) {
		WS_DBG("rws_frame_create fail");
		return rws_false;
	}
	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_binary_frame;
	rws_frame_fill_with_send_data(frame, data, data_size);
	if (frame->data == NULL) {
        WS_DBG("rws_frame_fill_with_send_data fail %d", data_size);
		rws_frame_delete(frame);
		return rws_false;
	}
	rws_socket_append_send_frames(s, frame);

	return rws_true;
}

void rws_socket_delete_all_frames_in_list(_rws_list * list_with_frames)
{
	_rws_frame * frame = NULL;
	_rws_node * cur = list_with_frames;
	while (cur)
	{
		frame = (_rws_frame *)cur->value.object;
		if (frame) rws_frame_delete(frame);
		cur->value.object = NULL;
		cur = cur->next;
	}
}

void rws_socket_set_option(rws_socket_t s, int option, int value)
{
    WS_DBG("%s: soc_id:%d, option:%d, value:%d", __FUNCTION__, s, option, value);
	setsockopt(s, SOL_SOCKET, option, (char *)&value, sizeof(int));
}

void rws_socket_check_write_error(_rws_socket * s, int error_num)
{
#if defined(RWS_OS_WINDOWS)
	int socket_code = 0, code = 0;
	unsigned int socket_code_size = sizeof(int);
#else
	int socket_code = 0, code = 0;
	socklen_t socket_code_size = sizeof(socket_code);
#endif

	if (s->socket != RWS_INVALID_SOCKET)
	{
#if defined(RWS_OS_WINDOWS)
		if (getsockopt(s->socket, SOL_SOCKET, SO_ERROR, (char *)&socket_code, (int*)&socket_code_size) != 0) socket_code = 0;
#else
		if (getsockopt(s->socket, SOL_SOCKET, SO_ERROR, &socket_code, &socket_code_size) != 0) socket_code = 0;
#endif
	}

    WS_DBG("%s: error_code:%d, error_num:%d", __FUNCTION__, socket_code, error_num);

	code = (socket_code > 0) ? socket_code : error_num;
	if (code <= 0) return;
	switch (code)
	{
		// send errors
		case EACCES: //

//		case EAGAIN: // The socket is marked nonblocking and the requested operation would block
//		case EWOULDBLOCK: // The socket is marked nonblocking and the receive operation would block

		case EBADF: // An invalid descriptor was specified
		case ECONNRESET: // Connection reset by peer
		case EDESTADDRREQ: // The socket is not connection-mode, and no peer address is set
		case EFAULT: // An invalid user space address was specified for an argument
					// The receive buffer pointer(s) point outside the process's address space.
		case EINTR: // A signal occurred before any data was transmitted
					// The receive was interrupted by delivery of a signal before any data were available
		case EINVAL: // Invalid argument passed
		case EISCONN: // The connection-mode socket was connected already but a recipient was specified
		case EMSGSIZE: // The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible
		case ENOBUFS: // The output queue for a network interface was full
		case ENOMEM: // No memory available
		case ENOTCONN: // The socket is not connected, and no target has been given
						// The socket is associated with a connection-oriented protocol and has not been connected
		case ENOTSOCK: // The argument sockfd is not a socket
					// The argument sockfd does not refer to a socket
		case EOPNOTSUPP: // Some bit in the flags argument is inappropriate for the socket type.
		case EPIPE: // The local end has been shut down on a connection oriented socket
		// recv errors
		case ECONNREFUSED: // A remote host refused to allow the network connection (typically because it is not running the requested service).
		case EHOSTUNREACH: // Host is unreachable
			s->error = rws_error_new_code_descr(rws_error_code_read_write_socket, rws_strerror(code));
            WS_DBG("%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description ? s->error->description : "");
			break;

		default:
			break;
	}
}

#ifdef MTK_WEBSOCKET_SSL_ENABLE
#if defined(MBEDTLS_DEBUG_C)
#define DEBUG_LEVEL 2
#endif
void rws_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    //WS_DBG("[SSL]%s", str ? str : NULL);
}


int rws_ssl_conn(_rws_socket *s)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
    const char *pers = "https";
    int value, ret = 0;
    uint32_t flags;
    _rws_ssl *ssl = NULL;
    char portstr[16] = {0};

    WS_DBG("[SSL]%s", __FUNCTION__);
    s->ssl = rws_malloc_zero(sizeof(_rws_ssl));
    if (!s->ssl) {
        WS_DBG("[SSL]Memory malloc error.");
        ret = -1;
        goto exit;
    }
    ssl = s->ssl;

    if (s->server_cert && s->server_cert_len)
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;

    /*
     * Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init(&ssl->net_ctx);
    mbedtls_ssl_init(&ssl->ssl_ctx);
    mbedtls_ssl_config_init(&ssl->ssl_conf);
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
    mbedtls_pk_init(&ssl->pkey);
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);

    /*
        * 0. Initialize the RNG and the session data
        */
    mbedtls_entropy_init(&ssl->entropy);
    if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg,
                               mbedtls_entropy_func,
                               &ssl->entropy,
                               (const unsigned char*)pers,
                               strlen(pers))) != 0) {
        WS_DBG("[SSL]mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    /*
    * Load the Client certificate
    */
    if (s->client_cert && s->client_cert_len && s->client_pk && s->client_pk_len)
    {
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)s->client_cert, s->client_cert_len);
        if (ret < 0) {
            WS_DBG("[SSL]Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret);
            goto exit;
        }

        ret = mbedtls_pk_parse_key(&ssl->pkey, (const unsigned char *)s->client_pk, s->client_pk_len, NULL, 0);
        if (ret != 0) {
            WS_DBG("[SSL]failed! mbedtls_pk_parse_key returned -0x%x.", -ret);
            goto exit;
        }
    }

    /*
    * Load the trusted CA
    */
    /* cert_len passed in is gotten from sizeof not strlen */
    if (s->server_cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
                                        (const unsigned char *)s->server_cert,
                                        s->server_cert_len)) < 0)) {
        WS_DBG("[SSL]mbedtls_x509_crt_parse() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    /*
     * Start the connection
     */
    rws_sprintf(portstr, 16, "%i", s->port);
    if ((ret = mbedtls_net_connect(&ssl->net_ctx, s->host, portstr, MBEDTLS_NET_PROTO_TCP)) != 0) {
        WS_DBG("[SSL]failed! mbedtls_net_connect returned %d, port:%s.", ret, portstr);
        goto exit;
    }

    WS_DBG("[SSL]mbedtls_net_connect() ok");

    s->received_len = 0;
    s->socket = ssl->net_ctx.fd;

    /*
     * Setup stuff
     */
    if ((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        WS_DBG("[SSL]mbedtls_ssl_config_defaults() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    // TODO: add customerization encryption algorithm
    memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_MD5);
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

    if (s->client_cert && (ret = mbedtls_ssl_conf_own_cert(&ssl->ssl_conf, &ssl->clicert, &ssl->pkey)) != 0) {
        WS_DBG("[SSL]failed! mbedtls_ssl_conf_own_cert returned %d.", ret );
        goto exit;
    }

    mbedtls_ssl_conf_rng(&ssl->ssl_conf, mbedtls_ctr_drbg_random, &ssl->ctr_drbg);
    mbedtls_ssl_conf_dbg(&ssl->ssl_conf, rws_debug, NULL);

    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx, &ssl->ssl_conf)) != 0) {
        WS_DBG("[SSL]mbedtls_ssl_setup() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl->ssl_ctx, s->host ) ) != 0 )
    {
        WS_DBG( "[SSL]failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
    * Handshake
    */
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            WS_DBG("[SSL]mbedtls_ssl_handshake() failed, ret:-0x%x.", -ret);
            ret = -1;
            goto exit;
        }

        WS_DBG("[SSL]mbedtls_ssl_handshake() while");
    }

    WS_DBG("[SSL]mbedtls_ssl_handshake() ok");

    mbedtls_net_set_nonblock(&ssl->net_ctx);

    /*
     * Verify the server certificate
     */
    /* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
        * handshake would not succeed if the peer's cert is bad.  Even if we used
        * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
    if ((flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0) {
        char vrfy_buf[512];
        WS_DBG("[SSL]svr_cert varification failed. authmode:%d", authmode);
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        WS_DBG("[SSL]%s", vrfy_buf);
    }
    else
        WS_DBG("[SSL]svr_cert varification ok. authmode:%d", authmode);

exit:
    if(ret != 0) {
#ifdef MBEDTLS_ERROR_C
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        WS_DBG("[SSL]Last error was: %d - %s\n\n", ret, error_buf );
#endif
        WS_DBG("[SSL]ret=%d.", ret);
    	s->error = rws_error_new_code_descr(rws_error_code_connect_to_host, "Failed connect to host");
        WS_DBG("[SSL]%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description);
    	s->command = COMMAND_INFORM_DISCONNECTED;

        if (s->ssl)
        {
            mbedtls_net_free(&s->ssl->net_ctx);
            mbedtls_x509_crt_free(&s->ssl->cacert);
            mbedtls_x509_crt_free(&s->ssl->clicert);
            mbedtls_pk_free(&s->ssl->pkey);
            mbedtls_ssl_free(&s->ssl->ssl_ctx);
            mbedtls_ssl_config_free(&s->ssl->ssl_conf);
            mbedtls_ctr_drbg_free(&s->ssl->ctr_drbg);
            mbedtls_entropy_free(&s->ssl->entropy);

            rws_free(s->ssl);
            s->ssl = NULL;
        }

        s->socket == RWS_INVALID_SOCKET;
    }
    else
    {
        s->command = COMMAND_SEND_HANDSHAKE;
    }

    return ret;
}

int rws_ssl_close(_rws_socket *s)
{
    _rws_ssl *ssl = s->ssl;

    if (!ssl)
        return -1;

    mbedtls_ssl_close_notify(&ssl->ssl_ctx);
    mbedtls_net_free(&ssl->net_ctx);
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
    mbedtls_pk_free(&ssl->pkey);
    mbedtls_ssl_free(&ssl->ssl_ctx);
    mbedtls_ssl_config_free(&ssl->ssl_conf);
    mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    mbedtls_entropy_free(&ssl->entropy);

    rws_free(ssl);
    s->ssl = NULL;

    return 0;
}
#endif

