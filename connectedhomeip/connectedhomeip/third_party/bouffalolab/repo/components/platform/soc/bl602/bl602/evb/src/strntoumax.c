/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * strntoumax.c
 *
 * The strntoumax() function and associated
 */

#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <inttypes.h>

static __inline int digitval(int ch)
{
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	} else if (ch >= 'A' && ch <= 'Z') {
		return ch - 'A' + 10;
	} else if (ch >= 'a' && ch <= 'z') {
		return ch - 'a' + 10;
	} else {
		return -1;
	}
}

uintmax_t strntoumax(const char *nptr, char **endptr, int base, size_t n)
{
	int minus = 0;
	uintmax_t v = 0;
	int d;

	while (n && isspace((unsigned char)*nptr)) {
		nptr++;
		n--;
	}

	/* Single optional + or - */
	if (n) {
		char c = *nptr;
		if (c == '-' || c == '+') {
			minus = (c == '-');
			nptr++;
			n--;
		}
	}

	if (base == 0) {
		if (n >= 2 && nptr[0] == '0' &&
		    (nptr[1] == 'x' || nptr[1] == 'X')) {
			n -= 2;
			nptr += 2;
			base = 16;
		} else if (n >= 1 && nptr[0] == '0') {
			n--;
			nptr++;
			base = 8;
		} else {
			base = 10;
		}
	} else if (base == 16) {
		if (n >= 2 && nptr[0] == '0' &&
		    (nptr[1] == 'x' || nptr[1] == 'X')) {
			n -= 2;
			nptr += 2;
		}
	}

	while (n && (d = digitval(*nptr)) >= 0 && d < base) {
		v = v * base + d;
		n--;
		nptr++;
	}

	if (endptr)
		*endptr = (char *)nptr;

	return minus ? -v : v;
}
