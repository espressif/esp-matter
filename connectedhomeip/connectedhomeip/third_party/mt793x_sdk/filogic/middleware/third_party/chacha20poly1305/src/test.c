#ifdef __UNIT_TEST__
#include "evp.h"
#include <stdlib.h>
int testchacha()
{
int i;
const EVP_AEAD      *aead  = NULL;
size_t            encryptedData_len        = 0;
size_t            out_len2        = 0;
unsigned char   encryptedData[30] = {0}; 
unsigned char   out2[20]={0};

aead = EVP_aead_chacha20_poly1305();
EVP_AEAD_CTX         ctx;
if (!EVP_AEAD_CTX_init(&ctx, aead,"\x42\x90\xbc\xb1\x54\x17\x35\x31\xf3\x14\xaf\x57\xf3\xbe\x3b\x50\x06\xda\x37\x1e\xce\x27\x2a\xfa\x1b\x5d\xbd\xd1\x10\x0a\x10\x07", 32, 16))  //0: default tag length - 16
        {
            printf("Failed to init AEAD\n");
            return 0;
        }

        // Encrypt sub-TLV and generate auth tag. encryptedData_len = length of encryptedData + length of authTag
        if (!EVP_AEAD_CTX_seal(&ctx, encryptedData, &encryptedData_len, sizeof(encryptedData), "\xcd\x7c\xf6\x7b\xe3\x9c\x79\x4a", 8, "\x86\xd0\x99\x74\x84\x0b\xde\xd2\xa5\xca", 10,"\x87\xe2\x29\xd4\x50\x08\x45\xa0\x79\xc0", 10))
        {
            printf("Failed to run AEAD\n");
                  return 0;
        }

        printf("[acs]: encrypt sub-TLV and generate auth tag\n");
        printf("encrypted data = \n");

        for ( i = 0; i < encryptedData_len; i ++) {
        printf(" %02x", encryptedData[i]);
        }

        printf("\nencryptedData_len=%d\n",encryptedData_len);

        if (!EVP_AEAD_CTX_open(&ctx, out2, &out_len2, 10, "\xcd\x7c\xf6\x7b\xe3\x9c\x79\x4a",
	    8, encryptedData, encryptedData_len, "\x87\xe2\x29\xd4\x50\x08\x45\xa0\x79\xc0", 10)) {
		printf("Failed to decrypt  \n");
		return 0;
	}
        for (  i = 0; i < out_len2; i ++) {
          printf(" %02x", out2[i]);
         }
        printf("\ndecryptData_len=%d\n",out_len2);

        EVP_AEAD_CTX_cleanup(&ctx);
        return 0;
}
#endif /* __UNIT_TEST__ */

