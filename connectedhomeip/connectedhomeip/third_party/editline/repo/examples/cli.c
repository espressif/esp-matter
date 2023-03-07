/* Custom CLI command completion.
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

#include "editline.h"
#include <string.h>

#define HISTORY "/tmp/.cli-history"

static char *list[] = {
    "foo ", "bar ", "bsd ", "cli ", "ls ", "cd ", "malloc ", "tee ", NULL
};

/* Attempt to complete the pathname, returning an allocated copy.
 * Fill in *unique if we completed it, or set it to 0 if ambiguous. */
static char *my_rl_complete(char *token, int *match)
{
    int i;
    int index = -1;
    int matchlen = 0;
    int count = 0;

    for (i = 0; list[i]; i++) {
	int partlen = strlen(token); /* Part of token */

	if (!strncmp(list[i], token, partlen)) {
	    index = i;
	    matchlen = partlen;
	    count ++;
	}
    }

    if (count == 1) {
	*match = 1;
	return strdup(list[index] + matchlen);
    }

    return NULL;
}

/* Return all possible completions. */
static int my_rl_list_possib(char *token, char ***av)
{
    int i, num, total = 0;
    char **copy;

    for (num = 0; list[num]; num++)
	;

    if (!num)
	return 0;

    copy = malloc(num * sizeof(char *));
    for (i = 0; i < num; i++) {
	if (!strncmp(list[i], token, strlen (token))) {
	    copy[total] = strdup(list[i]);
	    total++;
	}
    }
    *av = copy;

    return total;
}

el_status_t list_possible(void)
{
    char        **av;
    char        *word;
    int         ac;

    word = el_find_word();
    ac = rl_list_possib(word, &av);
    if (word)
        free(word);
    if (ac) {
        el_print_columns(ac, av);
        while (--ac >= 0)
            free(av[ac]);
        free(av);

        return CSmove;
    }

    return el_ring_bell();
}

el_status_t do_suspend(void)
{
    puts("Abort!");
    return CSstay;
}

static void breakit(int signo)
{
    (void)signo;
    puts("Got SIGINT");
}

/* Use el_no_echo when reading passwords and similar */
static int unlock(const char *passwd)
{
    char *prompt = "Enter password: ";
    char *line;
    int rc = 1;

    el_no_echo = 1;

    while ((line = readline(prompt))) {
	rc = strncmp(line, passwd, strlen(passwd));
	free(line);

	if (rc) {
	    printf("\nWrong password, please try again, it's secret.\n");
	    continue;
	}

	printf("\nAchievement unlocked!\n");
	break;
    }

    el_no_echo = 0;

    return rc;
}

int main(void)
{
    char *line;
    char *prompt = "cli> ";

    signal(SIGINT, breakit);

    /* Setup callbacks */
    rl_set_complete_func(&my_rl_complete);
    rl_set_list_possib_func(&my_rl_list_possib);

    el_bind_key('?', list_possible);
    el_bind_key(CTL('Z'), do_suspend);
    read_history(HISTORY);

    while ((line = readline(prompt))) {
	if (!strncmp(line, "unlock", 6) && unlock("secret")) {
	    free(line);
	    fprintf(stderr, "\nSecurity breach, user logged out!\n");
	    break;
	}

	if (*line != '\0')
	    printf("\t\t\t|%s|\n", line);
 	free(line);
    }

    write_history(HISTORY);

    return 0;
}

/**
 * Local Variables:
 *  c-file-style: "k&r"
 *  c-basic-offset: 4
 * End:
 */
