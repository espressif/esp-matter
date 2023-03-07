/* Copyright 1998-2010 IAR Systems AB.  */

/*******************
 * This is the default implementation of the "abort" function of the
 * standard library.  It can be replaced with a system-specific
 * implementation.
 ********************/

#include <stdlib.h>
#include <yfuns.h>

_STD_BEGIN

#pragma module_name = "?abort"

void abort(void)
{
  __exit(EXIT_FAILURE);
}

_STD_END
