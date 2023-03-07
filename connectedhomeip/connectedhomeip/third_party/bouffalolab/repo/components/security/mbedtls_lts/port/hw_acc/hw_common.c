#include "hw_common.h"

#include <mbedtls/bignum.h>
#include <bignum_ext.h>

int mpi_words_to_reg_size(size_t words)
{
    size_t bytes = words * 4;
    if (bytes <= 8) return SEC_ENG_PKA_REG_SIZE_8;
    else if (bytes <= 16) return SEC_ENG_PKA_REG_SIZE_16;
    else if (bytes <= 32) return SEC_ENG_PKA_REG_SIZE_32;
    else if (bytes <= 64) return SEC_ENG_PKA_REG_SIZE_64;
    else if (bytes <= 96) return SEC_ENG_PKA_REG_SIZE_96;
    else if (bytes <= 128) return SEC_ENG_PKA_REG_SIZE_128;
    else if (bytes <= 192) return SEC_ENG_PKA_REG_SIZE_192;
    else if (bytes <= 256) return SEC_ENG_PKA_REG_SIZE_256;
    else if (bytes <= 384) return SEC_ENG_PKA_REG_SIZE_384;
    else if (bytes <= 512) return SEC_ENG_PKA_REG_SIZE_512;
    else return 0; // too large
}

size_t mpi_reg_size_to_words(int reg_size)
{
    if (reg_size == SEC_ENG_PKA_REG_SIZE_8) return 8 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_16) return 16 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_32) return 32 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_64) return 64 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_96) return 96 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_128) return 128 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_192) return 192 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_256) return 256 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_384) return 384 / 4;
    else if (reg_size == SEC_ENG_PKA_REG_SIZE_512) return 512 / 4;
    else return 0;
}

#if MBEDTLS_HW_ACC_DBG
void dump_pka_reg(const char *tag, int s, uint8_t reg)
{
    mbedtls_mpi num;
    mbedtls_mpi_init(&num);
    static uint32_t tmp_buf[512/4];
    printf("reg %s:\r\n", tag);
    size_t words = mpi_reg_size_to_words(s);
    memset(tmp_buf, 0, sizeof(tmp_buf));
    Sec_Eng_PKA_Read_Data(s, reg, (void *)tmp_buf, words);
    if (mbedtls_mpi_read_binary(&num, (uint8_t *)tmp_buf, words * 4)) {
        printf("<err>\r\n");
        return;
    }
    dump_mpi(NULL, &num);
    mbedtls_mpi_free(&num);
}
#endif
