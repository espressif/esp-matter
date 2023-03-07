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
#include "rws_error.h"
#include "rws_string.h"
#include "rws_memory.h"

_rws_error g_rws_err;

// private
_rws_error * rws_error_create(void)
{
	_rws_error * e = (_rws_error *)rws_malloc_zero(sizeof(_rws_error));
	return e;
}

_rws_error * rws_error_new_code_descr(const int code, const char * description)
{
	_rws_error * e = (_rws_error *)rws_malloc_zero(sizeof(_rws_error));
	if (e == NULL) {
		e = &g_rws_err;
	}
	e->code = code;
	e->description = rws_string_copy(description);
	return e;
}

void rws_error_delete(_rws_error * error)
{
	if (error)
	{
		rws_string_delete(error->description);
		if (error != &g_rws_err) {
			rws_free(error);
		}		
	}
}

void rws_error_delete_clean(_rws_error ** error)
{
	if (error)
	{
		rws_error_delete(*error);
		*error = NULL;
	}
}

// public
int rws_error_get_code(rws_error error)
{
	_rws_error * e = (_rws_error *)error;
	return e ? e->code : 0;
}

int rws_error_get_http_error(rws_error error)
{
	_rws_error * e = (_rws_error *)error;
	return e ? e->http_error : 0;
}

const char * rws_error_get_description(rws_error error)
{
	_rws_error * e = (_rws_error *)error;
	return e ? e->description : NULL;
}

