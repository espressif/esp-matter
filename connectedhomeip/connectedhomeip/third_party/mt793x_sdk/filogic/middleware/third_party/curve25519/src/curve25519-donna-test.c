#ifdef __UNIT_TEST__
#include <stdio.h>
#include <string.h>
#include "curve25519-donna.h"

static void curve25519_djb_doit(unsigned char *ek,unsigned char *e,unsigned char *k, int print)
{
  int i;

  if (print) {
    for (i = 0;i < 32;++i) printf("%02x",(unsigned int) e[i]); printf(" ");
    for (i = 0;i < 32;++i) printf("%02x",(unsigned int) k[i]); printf(" ");
  }
  curve25519_donna(ek,e,k);
  if (print) {
    for (i = 0;i < 32;++i) printf("%02x",(unsigned int) ek[i]); printf("\n");
  }
}
int curve25519_djb_test(int print)
{
  unsigned char e1k[32];
  unsigned char e2k[32];
  unsigned char e1e2k[32];
  unsigned char e2e1k[32];
  unsigned char e1[32] = {3};
  unsigned char e2[32] = {5};
  unsigned char k[32] = {9};
  int loop;
  int i;

  os_memset(e1k,0,sizeof(e1k));
  os_memset(e2k,0,sizeof(e2k));
  os_memset(e1e2k,0,sizeof(e1e2k));
  os_memset(e2e1k,0,sizeof(e2e1k));
  for (loop = 0;loop < 5;++loop) {
    curve25519_djb_doit(e1k,e1,k, print);
    curve25519_djb_doit(e2e1k,e2,e1k, print);
    curve25519_djb_doit(e2k,e2,k, print);
    curve25519_djb_doit(e1e2k,e1,e2k, print);
    for (i = 0;i < 32;++i) if (e1e2k[i] != e2e1k[i]) {
      if (print) printf("fail\n");
      return 1;
    }
    for (i = 0;i < 32;++i) e1[i] ^= e2k[i];
    for (i = 0;i < 32;++i) e2[i] ^= e1k[i];
    for (i = 0;i < 32;++i) k[i] ^= e1e2k[i];
  }

  return 0;
}
#endif /* __UNIT_TEST__ */
