/*
 * Copyright (c) 1997-2007  The Stanford SRP Authentication Project
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Redistributions in source or binary form must retain an intact copy
 * of this copyright notice.
 */

#ifndef T_CLIENT_H
#define T_CLIENT_H

#include "t_sha.h"
#include "srp_cstr.h"

#define ALEN 32
#define MIN_MOD_BYTES 64	/* 512 bits */

struct t_client {
  struct t_num n;
  struct t_num g;
  struct t_num s;

  struct t_num a;
  struct t_num A;

  struct t_num p;
  struct t_num v;

  cstr * ex_data;

  SHA1_CTX hash, ckhash;

  char username[MAXUSERLEN];
  unsigned char session_key[SESSION_KEY_LEN];
  unsigned char session_response[RESPONSE_LEN];

  unsigned char nbuf[MAXPARAMLEN], gbuf[MAXPARAMLEN], sbuf[MAXSALTLEN];
  unsigned char pbuf[MAXPARAMLEN], vbuf[MAXPARAMLEN];
  unsigned char abuf[ALEN], Abuf[MAXPARAMLEN];
};

/*
 * SRP client-side negotiation
 *
 * This code negotiates the client side of an SRP exchange.
 * "t_clientopen" accepts a username, and N, g, and s parameters,
 *   which are usually sent by the server in the first round.
 *   The client should then call...
 * "t_clientgenexp" will generate a random 256-bit exponent and
 *   raise g to that power, returning the result.  This result
 *   should be sent to the server as w(p).
 * "t_clientpasswd" accepts the user's password, which should be
 *   entered locally and updates the client's state.
 * "t_clientaddexdata" adds data to be authenticated along with
 *   the rest of the session parameters.  This data is included in
 *   the response hash after the session key.
 * "t_clientgetkey" accepts the exponential y(p), which should
 *   be sent by the server in the next round and computes the
 *   256-bit session key.  This data should be saved before the
 *   session is closed.
 * "t_clientresponse" computes the session key proof as SHA(y(p), K).
 * "t_clientclose" closes the session and frees its memory.
 *
 * Note that authentication is not performed per se; it is up
 * to either/both sides of the protocol to now verify securely
 * that their session keys agree in order to establish authenticity.
 * One possible way is through "oracle hashing"; one side sends
 * r, the other replies with H(r,K), where H() is a hash function.
 *
 * t_clientresponse and t_clientverify now implement a version of
 * the session-key verification described above.
 */
_TYPE( struct t_client * )
  t_clientopen P((const char *, struct t_num *, struct t_num *,
		  struct t_num *));
_TYPE( struct t_num * ) t_clientgenexp P((struct t_client *));
_TYPE( void ) t_clientpasswd P((struct t_client *, const char *));
_TYPE( void ) t_clientaddexdata P((struct t_client *, unsigned char *, int));
_TYPE( unsigned char * )
  t_clientgetkey P((struct t_client *, struct t_num *));
_TYPE( int ) t_clientverify P((struct t_client *, unsigned char *));
_TYPE( unsigned char * ) t_clientresponse P((struct t_client *));
_TYPE( void ) t_clientclose P((struct t_client *));

#endif
