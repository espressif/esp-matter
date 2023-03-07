/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

void cgi_urldecode(char *url);
bool cgi_get_varval(char *var_str, char *var_name, char *var_val, uint32_t length);

void format_post_data(char *string);

void http_srv_task(void *arg);

#endif
