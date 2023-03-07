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

#include <stdio.h>
#include <aos/kernel.h>

#include <blog.h>

BLOG_DECLARE(blog_testc2);

void test_buf(void)
{
    int i;
    static uint8_t buf[256];

    for(i = 0; i < sizeof(buf); i++) {
        buf[i] = (uint8_t)i;
    }

    blog_debug_hexdump("hexdumpbuf", buf, sizeof(buf));
    blog_info_hexdump("hexdumpbuf", buf, sizeof(buf));
    blog_warn_hexdump("hexdumpbuf", buf, sizeof(buf));
    blog_error_hexdump("hexdumpbuf", buf, sizeof(buf));
    blog_assert_hexdump("hexdumpbuf", buf, sizeof(buf));
}
void blog_testc2_entry(void *arg)
{
    aos_msleep(5000);

    blog_debug("blog_testc2 debug\r\n");
    blog_info("blog_testc2 info\r\n");
    blog_warn("blog_testc2 warn\r\n");
    blog_error("blog_testc2 error\r\n");

    blog_debug_user(blog_testc2,"blog_testc2 debug user\r\n");
    blog_info_user(blog_testc2,"blog_testc2 info user\r\n");
    blog_warn_user(blog_testc2,"blog_testc2 warn user\r\n");
    blog_error_user(blog_testc2,"blog_testc2 error user\r\n");
    blog_assert_user(blog_testc2,"blog_testc2 assert user\r\n");

    while (1) {
        aos_msleep(5000);

        test_buf();

        blog_debug("blog_testc2 debug\r\n");
        blog_info("blog_testc2 info\r\n");
        blog_warn("blog_testc2 warn\r\n");
        blog_error("blog_testc2 error\r\n");

        blog_debug_user(blog_testc2,"blog_testc2 debug user\r\n");
        blog_info_user(blog_testc2,"blog_testc2 info user\r\n");
        blog_warn_user(blog_testc2,"blog_testc2 warn user\r\n");
        blog_error_user(blog_testc2,"blog_testc2 error user\r\n");
        blog_assert_user(blog_testc2,"blog_testc2 assert user\r\n");
    }
}

int blog_testc2_init(void)
{
    aos_task_new("blog_testc2", blog_testc2_entry, NULL, 2048);

    return 0;
}
