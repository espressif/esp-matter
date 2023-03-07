#include "sl_iostream.h"

#ifndef UNITY_IOSTREAM_STDOUT
#define UNITY_IOSTREAM_STDOUT   SL_IOSTREAM_STDOUT
#else
extern sl_iostream_t  * UNITY_IOSTREAM_STDOUT;
#endif

void unity_iostream_putchar(char c)
{
  sl_iostream_putchar(UNITY_IOSTREAM_STDOUT, c);
}
