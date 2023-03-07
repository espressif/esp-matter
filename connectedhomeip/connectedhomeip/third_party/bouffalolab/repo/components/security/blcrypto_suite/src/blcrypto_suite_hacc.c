#include <blcrypto_suite/blcrypto_suite_top_config.h>
#include <blcrypto_suite/blcrypto_suite_hacc.h>

#if USE_HWCRYPTO
#ifdef CFG_CHIP_BL602
#include <bl602_sec_eng.h>
#endif

#ifdef CFG_CHIP_BL808
#include <bl606p_sec_eng.h>
#endif

int blcrypto_suite_hacc_reset()
{
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    return 0;
}

int blcrypto_suite_hacc_gf2mont_256_bin(uint32_t *result, uint32_t *src, uint32_t *p)
{
    /*
     * S32.0 dst
     * S32.1 src
     * S32.2 p
     * S32.4,5(S64.2) tmp
     */
    Sec_Eng_PKA_CREG(SEC_ENG_PKA_REG_SIZE_64, 2, 64 / 4, 1);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32, 1, src, 32 / 4, 1);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32, 2, p, 32 / 4, 1);
    Sec_Eng_PKA_GF2Mont(SEC_ENG_PKA_REG_SIZE_32, 0, SEC_ENG_PKA_REG_SIZE_32, 1, 256, SEC_ENG_PKA_REG_SIZE_64, 2, SEC_ENG_PKA_REG_SIZE_32, 2);
    Sec_Eng_PKA_Read_Data(SEC_ENG_PKA_REG_SIZE_32, 0, result, 32 / 4);

    return 0;
}
#endif
