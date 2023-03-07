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

#if !defined(RWS_OS_WINDOWS)
//#include <signal.h>
#endif

#if defined(RWS_MTK) && defined(WEBSOCKET_DEBUG)
log_create_module(websocket, PRINT_LEVEL_INFO);
#endif

// public
rws_bool rws_socket_connect(rws_socket socket)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)socket;
	const char * params_error_msg = NULL;
	if (!s) return rws_false;

	rws_error_delete_clean(&s->error);

	if (s->port <= 0) params_error_msg = "No URL port provided";
	if (!s->scheme) params_error_msg = "No URL scheme provided";
	if (!s->host) params_error_msg = "No URL host provided";
	if (!s->path) params_error_msg = "No URL path provided";
	if (!s->on_disconnected) params_error_msg = "No on_disconnected callback provided";
    s->received_len = 0;
	if (params_error_msg)
	{
		s->error = rws_error_new_code_descr(rws_error_code_missed_parameter, params_error_msg);
		return rws_false;
	}
	return rws_socket_create_start_work_thread(s);
}

void rws_socket_disconnect_and_release(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	if (!s) return;

	rws_mutex_lock(s->work_mutex);

    WS_DBG("%s: is_connected:%d, command:%d", __FUNCTION__, s->is_connected, s->command);

	rws_socket_delete_all_frames_in_list(s->send_frames);
	rws_list_delete_clean(&s->send_frames);

	if (s->is_connected) // connected in loop
	{
		s->command = COMMAND_DISCONNECT;
		rws_mutex_unlock(s->work_mutex);
	}
	else if (s->work_thread) // disconnected in loop
	{
		s->command = COMMAND_END;
		rws_mutex_unlock(s->work_mutex);
	}
	else if (s->command != COMMAND_END)
	{
		// not in loop
		rws_mutex_unlock(s->work_mutex);
		rws_socket_delete(s);
	}
}

rws_bool rws_socket_send_data(rws_socket socket, const void * data, const size_t data_size)
{
    //WS_DBG("%s: test:%s", __FUNCTION__, text ? text : "");
	//WS_DBG("%s,data_size:%d", __FUNCTION__,data_size);

	_rws_socket * s = (_rws_socket *)socket;
	rws_bool r = rws_false;
	if (!s) return r;
	rws_mutex_lock(s->send_mutex);
	r = rws_socket_send_data_priv(s, data,data_size);
	rws_mutex_unlock(s->send_mutex);
	return r;
}

rws_bool rws_socket_send_text(rws_socket socket, const char * text)
{
    WS_DBG("%s: test:%s", __FUNCTION__, text ? text : "");

	_rws_socket * s = (_rws_socket *)socket;
	rws_bool r = rws_false;
	if (!s) return r;
	rws_mutex_lock(s->send_mutex);
	r = rws_socket_send_text_priv(s, text);
	rws_mutex_unlock(s->send_mutex);
	return r;
}

#if !defined(RWS_OS_WINDOWS)
void rws_socket_handle_sigpipe(int signal_number)
{
	WS_DBG("%s, signal_number:%d", __FUNCTION__, signal_number);
	return;
}
#endif

#define STRING_I(s) #s
#define TO_STRING(s) STRING_I(s)

void rws_socket_check_info(const char * info)
{
	assert(info);
}

rws_socket rws_socket_create(void)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)rws_malloc_zero(sizeof(_rws_socket));
	if (!s) return NULL;

#if !defined(RWS_OS_WINDOWS) && !defined(RWS_MTK)
    signal(SIGPIPE, rws_socket_handle_sigpipe);
#endif

	s->port = -1;
	s->socket = RWS_INVALID_SOCKET;
	s->command = COMMAND_NONE;

	s->work_mutex = rws_mutex_create_recursive();
	s->send_mutex = rws_mutex_create_recursive();

	static const char * info = "librws ver: " TO_STRING(RWS_VERSION_MAJOR) "." TO_STRING(RWS_VERSION_MINOR) "." TO_STRING(RWS_VERSION_PATCH) "\n";
	rws_socket_check_info(info);

	return s;
}

void rws_socket_delete(_rws_socket * s)
{
    WS_DBG("%s, soc_id:%d", __FUNCTION__, s->socket);

	rws_socket_close(s);

	rws_string_delete_clean(&s->sec_ws_accept);

	rws_free_clean(&s->received);
	s->received_size = 0;
	s->received_len = 0;

	rws_socket_delete_all_frames_in_list(s->send_frames);
	rws_list_delete_clean(&s->send_frames);
	rws_socket_delete_all_frames_in_list(s->recvd_frames);
	rws_list_delete_clean(&s->recvd_frames);

	rws_string_delete_clean(&s->scheme);
	rws_string_delete_clean(&s->host);
	rws_string_delete_clean(&s->path);

	rws_string_delete_clean(&s->sec_ws_accept);
#if defined(RWS_MTK)
	rws_string_delete_clean(&s->user_header);
#endif

	rws_error_delete_clean(&s->error);

	rws_free_clean(&s->received);
	rws_socket_delete_all_frames_in_list(s->send_frames);
	rws_list_delete_clean(&s->send_frames);
	rws_socket_delete_all_frames_in_list(s->recvd_frames);
	rws_list_delete_clean(&s->recvd_frames);

	rws_mutex_delete(s->work_mutex);
	rws_mutex_delete(s->send_mutex);

	rws_free(s);
}

void rws_socket_set_url(rws_socket socket,
						const char * scheme,
						const char * host,
						const int port,
						const char * path)
{
    WS_DBG("%s: scheme:%s, host:%s, port:%d, path:%s", __FUNCTION__, scheme ? scheme : "",
                                                    host ? host : "", port, path ? path : "");

	_rws_socket * s = (_rws_socket *)socket;
	if (!s) return;
	rws_string_delete(s->scheme);
	s->scheme = rws_string_copy(scheme);
	rws_string_delete(s->host);
	s->host = rws_string_copy(host);
	rws_string_delete(s->path);
	s->path = rws_string_copy(path);
	s->port = port;
}

void rws_socket_set_scheme(rws_socket socket, const char * scheme)
{
    WS_DBG("%s: scheme:%s", __FUNCTION__, scheme ? scheme : "");

	_rws_socket * s = (_rws_socket *)socket;
	if (!s) return;
	rws_string_delete(s->scheme);
	s->scheme = rws_string_copy(scheme);
}

const char * rws_socket_get_scheme(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	return s ? s->scheme : NULL;
}

void rws_socket_set_host(rws_socket socket, const char * host)
{
    WS_DBG("%s: host:%s", __FUNCTION__, host ? host : "");

	_rws_socket * s = (_rws_socket *)socket;
	if (!s) return;
	rws_string_delete(s->host);
	s->host = rws_string_copy(host);
}

const char * rws_socket_get_host(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	return s ? s->host : NULL;
}

void rws_socket_set_path(rws_socket socket, const char * path)
{
    WS_DBG("%s: path: %s", __FUNCTION__, path ? path : "");

	_rws_socket * s = (_rws_socket *)socket;
	if (!s) return;
	rws_string_delete(s->path);
	s->path = rws_string_copy(path);
}

const char * rws_socket_get_path(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	return s ? s->path : NULL;
}

void rws_socket_set_port(rws_socket socket, const int port)
{
    WS_DBG("%s: port: %d", __FUNCTION__, port);

	_rws_socket * s = (_rws_socket *)socket;
	if (s) s->port = port;
}

int rws_socket_get_port(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	return s ? s->port : -1;
}

/*
unsigned int _rws_socket_get_receive_buffer_size(rws_socket_t socket)
{
	unsigned int size = 0;
#if defined(RWS_OS_WINDOWS)
	int len = sizeof(unsigned int);
	if (getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)&size, &len) == -1) size = 0;
#else
	socklen_t len = sizeof(unsigned int);
	if (getsockopt(socket, SOL_SOCKET, SO_RCVBUF, &size, &len) == -1) size = 0;
#endif
	return size;
}

unsigned int rws_socket_get_receive_buffer_size(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	if (!s) return 0;
	if (s->socket == RWS_INVALID_SOCKET) return 0;
	return _rws_socket_get_receive_buffer_size(s->socket);
}
*/
rws_error rws_socket_get_error(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	return s ? s->error : NULL;
}

void rws_socket_set_user_object(rws_socket socket, void * user_object)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)socket;
	if (s) s->user_object = user_object;
}

void * rws_socket_get_user_object(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	return s ? s->user_object : NULL;
}

void rws_socket_set_on_connected(rws_socket socket, rws_on_socket callback)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)socket;
	if (s) s->on_connected = callback;
}

void rws_socket_set_on_disconnected(rws_socket socket, rws_on_socket callback)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)socket;
	if (s) s->on_disconnected = callback;
}

void rws_socket_set_on_received_text(rws_socket socket, rws_on_socket_recvd_text callback)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)socket;
	if (s) s->on_recvd_text = callback;
}

void rws_socket_set_on_received_bin(rws_socket socket, rws_on_socket_recvd_bin callback)
{
    WS_DBG("%s", __FUNCTION__);

	_rws_socket * s = (_rws_socket *)socket;
	if (s) s->on_recvd_bin = callback;
}

rws_bool rws_socket_is_connected(rws_socket socket)
{
	_rws_socket * s = (_rws_socket *)socket;
	rws_bool r = rws_false;
	if (!s) return r;
	rws_mutex_lock(s->send_mutex);
	r = s->is_connected;
    WS_DBG("%s: is_connected:%d", __FUNCTION__, r);
	rws_mutex_unlock(s->send_mutex);
	return r;
}

void rws_socket_set_server_cert(rws_socket socket, const char *server_cert, int server_cert_len)
{
    WS_DBG("%s: server_cert: %x, server_cert_len: %d", __FUNCTION__, server_cert, server_cert_len);

#ifdef MTK_WEBSOCKET_SSL_ENABLE
	_rws_socket * s = (_rws_socket *)socket;
	if (s && server_cert && server_cert_len)
    {
        s->server_cert = server_cert;
        s->server_cert_len = server_cert_len;
    }
    else
#endif
    {
        WS_DBG("Wrong parameter(s) or not supported.");
    }
}

void rws_socket_set_client_cert_pk(rws_socket socket,
                                         const char *client_cert,
                                         const unsigned int client_cert_len,
                                         const char *client_pk,
                                         const unsigned int client_pk_len)
{
    WS_DBG("%s: client_cert: %x, client_cert_len: %d, client_pk: %x, client_pk_len: %d", __FUNCTION__,
        client_cert, client_cert_len, client_pk, client_pk_len);

#ifdef MTK_WEBSOCKET_SSL_ENABLE
	_rws_socket * s = (_rws_socket *)socket;
	if (s && client_cert && client_cert_len && client_pk && client_pk_len)
    {
        s->client_cert = client_cert;
        s->client_cert_len = client_cert_len;
        s->client_pk = client_pk;
        s->client_pk_len = client_pk_len;
    }
    else
#endif
    {
        WS_DBG("Wrong parameter(s) or not supported.");
    }
}

#if defined(RWS_MTK)
void rws_socket_set_userheader(rws_socket socket, const char * user_header)
{
	_rws_socket * s = (_rws_socket *)socket;
    if (user_header) {
        WS_DBG("userheader %d:%s\r\n", strlen(user_header), user_header);
    }
    else {
        WS_DBG("userheader NULL");
        return;
    }
    
	if (!s) return;
	rws_string_delete(s->user_header);
	s->user_header = rws_string_copy(user_header);
}
#endif
