/* A "micro-shell" to test editline library.
 * If given any arguments, commands aren't executed.
 *
 * Copyright (c) 1992, 1993  Simmule Turner and Rich Salz. All rights reserved.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or of the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 * 4. This notice may not be removed or altered.
 */
#include <config.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "editline.h"

#ifndef HAVE_PERROR
extern int errno;
void perror(char *s)
{
    fprintf(stderr, "%s: error %d\n", s, errno);
}
#endif /* !HAVE_PERROR */

int main(int argc, char *argv[] __attribute__ ((unused)))
{
    int doit;
    char *prompt, *p;

    read_history(".testit_history");

    doit = argc == 1;
    if ((prompt = getenv("TESTPROMPT")) == NULL)
	prompt = "testit> ";

    while ((p = readline(prompt)) != NULL) {
	printf("\t\t\t|%s|\n", p);
	if (doit) {
	    if (strncmp(p, "cd ", 3) == 0) {
		if (chdir(&p[3]) < 0)
		    perror(&p[3]);
	    } else if (system(p) != 0) {
		perror(p);
	    }
	}
	free(p);
    }

    write_history(".testit_history");
    rl_uninitialize();

    return 0;
}

/**
 * Local Variables:
 *  c-file-style: "k&r"
 *  c-basic-offset: 4
 * End:
 */
