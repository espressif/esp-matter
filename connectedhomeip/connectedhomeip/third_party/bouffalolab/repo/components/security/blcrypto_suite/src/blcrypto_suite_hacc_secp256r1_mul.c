#include <blcrypto_suite/blcrypto_suite_top_config.h>
#include <blcrypto_suite/blcrypto_suite_hacc.h>

#if USE_HWCRYPTO
#ifdef CFG_CHIP_BL602
#include <bl602_sec_eng.h>
#endif

#ifdef CFG_CHIP_BL808
#include <bl606p_sec_eng.h>
#endif

#define ECP_SECP256R1_REG_TYPE                      SEC_ENG_PKA_REG_SIZE_32
#define ECP_SECP256R1_N_REG_INDEX                   0
#define ECP_SECP256R1_NPRIME_N_REG_INDEX            1
#define ECP_SECP256R1_INVR_N_REG_INDEX              2
#define ECP_SECP256R1_NPRIME_P_REG_INDEX            3
#define ECP_SECP256R1_INVR_P_REG_INDEX              4
#define ECP_SECP256R1_SIZE                          32
/* Used in verify */
#define ECP_SECP256R1_S_REG_INDEX                   5
#define ECP_SECP256R1_BAR_S_REG_INDEX               6
#define ECP_SECP256R1_HASH_REG_INDEX                6  //use ECP_SECP256R1_BAR_S_REG_INDEX since it's temp
#define ECP_SECP256R1_U1_REG_INDEX                  7
#define ECP_SECP256R1_LT_REG_TYPE                   SEC_ENG_PKA_REG_SIZE_64
#define ECP_SECP256R1_LT_REG_INDEX                  7
#define ECP_SECP256R1_SLT_REG_TYPE                  SEC_ENG_PKA_REG_SIZE_128
#define ECP_SECP256R1_SLT_REG_INDEX                 3

static const uint8_t secp256r1P[32] ALIGN4 ={
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
static const uint8_t secp256r1N[32] ALIGN4 ={
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xbc,0xe6,0xfa,0xad,0xa7,0x17,0x9e,0x84,0xf3,0xb9,0xca,0xc2,0xfc,0x63,0x25,0x51
};
static const uint8_t secp256r1PrimeN_N[32] ALIGN4 ={
        0x60,0xd0,0x66,0x33,0xa9,0xd6,0x28,0x1c,0x50,0xfe,0x77,0xec,0xc5,0x88,0xc6,0xf6,
        0x48,0xc9,0x44,0x08,0x7d,0x74,0xd2,0xe4,0xcc,0xd1,0xc8,0xaa,0xee,0x00,0xbc,0x4f
};
static const uint8_t secp256r1InvR_N[32] ALIGN4 ={
        0x60,0xd0,0x66,0x33,0x49,0x05,0xc1,0xe9,0x07,0xf8,0xb6,0x04,0x1e,0x60,0x77,0x25,
        0xba,0xde,0xf3,0xe2,0x43,0x56,0x6f,0xaf,0xce,0x1b,0xc8,0xf7,0x9c,0x19,0x7c,0x79
};
static const uint8_t secp256r1PrimeN_P[32] ALIGN4 ={
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};
static const uint8_t secp256r1InvR_P[32] ALIGN4 ={
        0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xfd,0x00,0x00,0x00,0x02,
        0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00
};
static const uint8_t secp256r1_1[32] ALIGN4 ={
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};
static const uint8_t secp256r1_BAR2[32] ALIGN4 ={
        0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02
};
static const uint8_t secp256r1_BAR3[32] ALIGN4 ={
        0x00,0x00,0x00,0x02,0xff,0xff,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xfd,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03
};
static const uint8_t secp256r1_BAR4[32] ALIGN4 ={
        0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04
};
static const uint8_t secp256r1_BAR8[32] ALIGN4 ={
        0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08
};
static const uint8_t secp256r1_1P1[32] ALIGN4 ={
        0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02
};
static const uint8_t secp256r1_1M1[32] ALIGN4 ={
        0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const uint8_t secp256r1_Zerox[32] ALIGN4 ={
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const uint8_t secp256r1_Zeroy[32] ALIGN4 ={
        0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};

static int bflb_ecc_basic_parameter_init(uint8_t id)
{
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_N_REG_INDEX,(uint32_t *)secp256r1N,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_NPRIME_N_REG_INDEX,(uint32_t *)secp256r1PrimeN_N,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_INVR_N_REG_INDEX,(uint32_t *)secp256r1InvR_N,ECP_SECP256R1_SIZE/4,0);

    return 0;
}

static int bflb_ecc_point_mul_init(uint8_t id)
{
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,0,(uint32_t *)secp256r1P,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,1,(uint32_t *)secp256r1PrimeN_P,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,8,(uint32_t *)secp256r1_1,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,9,(uint32_t *)secp256r1_BAR2,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,10,(uint32_t *)secp256r1_BAR3,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,11,(uint32_t *)secp256r1_BAR4,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,12,(uint32_t *)secp256r1_BAR8,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,19,(uint32_t *)secp256r1_1P1,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,20,(uint32_t *)secp256r1_1M1,ECP_SECP256R1_SIZE/4,0);

    return 0;
}

static void bflb_ecdsa_point_add_inf_check(uint8_t *pka_p1_eq_inf,uint8_t *pka_p2_eq_inf)
{
    uint8_t res[4];

    /* index 2:BAR_Zero_x
     * index 3:BAR_Zero_y
     * index 4:BAR_Zero_z
     * index 5:BAR_G_x
     * index 6:BAR_G_y
     * index 7:BAR_G_z
     * index 8:1
     * index 9:2
     * index 10:3
     * index 11:4
     * index 12:8
     * index 19:1P1
     * index 20:1m1*/

    //cout = 1 if X1 = 0
    Sec_Eng_PKA_LCMP(res,3,2,3,8);//s0 < s1 => cout = 1
    //cout = 1 if Y1 < Bar_1p1
    Sec_Eng_PKA_LCMP(res+1,3,3,3,19);
    //cout=1 if Y1 > Bar_1m1
    Sec_Eng_PKA_LCMP(res+2,3,20,3,3);
    //cout =1 if Z1 = 0
    Sec_Eng_PKA_LCMP(res+3,3,4,3,8);
    *pka_p1_eq_inf = res[0]&res[1]&res[2]&res[3];

    //cout = 1 if X2 = 0
    Sec_Eng_PKA_LCMP(res,3,5,3,8);
    // cout = 1 if Y2 < Bar_1p1
    Sec_Eng_PKA_LCMP(res+1,3,6,3,19);
    //cout = 1 if Y2 > Bar_1m1
    Sec_Eng_PKA_LCMP(res+2,3,20,3,6);
    //cout = 1 if Z2 = 0
    Sec_Eng_PKA_LCMP(res+3,3,7,3,8);
    *pka_p2_eq_inf = res[0]&res[1]&res[2]&res[3];
}

static void bflb_ecdsa_copy_x2_to_x1(uint8_t id)
{
    //X2->X1
    Sec_Eng_PKA_Move_Data(3,2,3,5,0);
    //Y2->Y1
    Sec_Eng_PKA_Move_Data(3,3,3,6,0);
    //Z2->Z1
    Sec_Eng_PKA_Move_Data(3,4,3,7,1);//Caution!!! wait movdat ready to execute next command
}

static void bflb_ecdsa_point_add(uint8_t id)
{
    /* index 2:BAR_Zero_x
     * index 3:BAR_Zero_y
     * index 4:BAR_Zero_z
     * index 5:BAR_G_x
     * index 6:BAR_G_y
     * index 7:BAR_G_z
     * index 8:1
     * index 9:2
     * index 10:3
     * index 11:4
     * index 12:8
     * index 19:1P1
     * index 20:1m1*/

    //U1 = Y2*Z1
    //PKA_MMUL(0,3,13,3, 6,3, 4,3,0);//d_reg_type,d_reg_idx,s0_reg_type,s0_reg_idx,s1_reg_type,s1_reg_idx,s2_reg_type,s2_reg_idx
    Sec_Eng_PKA_MMUL(3,13,3,6,3,4,3,0,0);

    //U2 = Y1*Z2
    //PKA_MMUL(0,3,14,3, 3,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3,14,3,3,3,7,3,0,0);

    //V1 = X2*Z1
    //PKA_MMUL(0,3,15,3, 5,3, 4,3,0);
    Sec_Eng_PKA_MMUL(3,15,3,5,3,4,3,0,0);

    //V2 = X1*Z2
    //PKA_MMUL(0,3,16,3, 2,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3,16,3,2,3,7,3,0,0);

    //U = U1-U2
    //PKA_MSUB(0,3,13,3,13,3,14,3,0);
    Sec_Eng_PKA_MSUB(3,13,3,13,3,14,3,0,0);

    //V = V1-V2
    //PKA_MSUB(0,3,15,3,15,3,16,3,0);
    Sec_Eng_PKA_MSUB(3,15,3,15,3,16,3,0,0);

    //W = Z1*Z2
    //PKA_MMUL(0,3, 2,3, 4,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3,2,3,4,3,7,3,0,0);

    //V^2
    //PKA_MMUL(0,3, 3,3,15,3,15,3,0);
    Sec_Eng_PKA_MMUL(3,3,3,15,3,15,3,0,0);

    //V^3
    //PKA_MMUL(0,3, 4,3, 3,3,15,3,0);
    Sec_Eng_PKA_MMUL(3,4,3,3,3,15,3,0,0);

    //U^2
    //PKA_MMUL(0,3,17,3,13,3,13,3,0);
    Sec_Eng_PKA_MMUL(3,17,3,13,3,13,3,0,0);

    //U^2*W
    //PKA_MMUL(0,3,17,3,17,3, 2,3,0);
    Sec_Eng_PKA_MMUL(3,17,3,17,3,2,3,0,0);

    //U^2*W-V^3
    //PKA_MSUB(0,3,17,3,17,3, 4,3,0);
    Sec_Eng_PKA_MSUB(3,17,3,17,3,4,3,0,0);

    //2*V^2
    //PKA_MMUL(0,3,18,3, 9,3, 3,3,0);
    Sec_Eng_PKA_MMUL(3,18,3,9,3,3,3,0,0);

    //2*V^2*V2
    //PKA_MMUL(0,3,18,3,18,3,16,3,0);
    Sec_Eng_PKA_MMUL(3,18,3,18,3,16,3,0,0);

    //A = U^2*W-V^3-2*V^2*V2
    //PKA_MSUB(0,3,18,3,17,3,18,3,0);
    Sec_Eng_PKA_MSUB(3,18,3,17,3,18,3,0,0);

    //V^2*V2
    //PKA_MMUL(0,3, 3,3, 3,3,16,3,0);
    Sec_Eng_PKA_MMUL(3,3,3,3,3,16,3,0,0);

    //V^3*U2
    //PKA_MMUL(0,3,14,3, 4,3,14,3,0);
    Sec_Eng_PKA_MMUL(3,14,3,4,3,14,3,0,0);

    //Z3 = V^3*W
    //PKA_MMUL(0,3, 4,3, 4,3, 2,3,0);
    Sec_Eng_PKA_MMUL(3,4,3,4,3,2,3,0,0);

    //X3 = V*A
    //PKA_MMUL(0,3, 2,3,15,3,18,3,0);
    Sec_Eng_PKA_MMUL(3,2,3,15,3,18,3,0,0);

    //V^2*V2-A
    //PKA_MSUB(0,3, 3,3, 3,3,18,3,0);
    Sec_Eng_PKA_MSUB(3,3,3,3,3,18,3,0,0);

    //U*(V^2*V2-A)
    //PKA_MMUL(0,3, 3,3,13,3, 3,3,0);
    Sec_Eng_PKA_MMUL(3,3,3,13,3,3,3,0,0);

    //Y3 = U*(V^2*V2-A)-V^3*U2
    //PKA_MSUB(1,3, 3,3, 3,3,14,3,0);
    Sec_Eng_PKA_MSUB(3,3,3,3,3,14,3,0,1);

}

static void bflb_ecdsa_point_double(uint8_t id)
{
    /* index 2:BAR_Zero_x
     * index 3:BAR_Zero_y
     * index 4:BAR_Zero_z
     * index 5:BAR_G_x
     * index 6:BAR_G_y
     * index 7:BAR_G_z
     * index 8:1
     * index 9:2
     * index 10:3
     * index 11:4
     * index 12:8
     * index 19:1P1
     * index 20:1m1*/

    //X1^2
    //PKA_MMUL(0,3,13,3, 5,3, 5,3,0);//d_reg_type,d_reg_idx,s0_reg_type,s0_reg_idx,s1_reg_type,s1_reg_idx,s2_reg_type,s2_reg_idx
    Sec_Eng_PKA_MMUL(3,13,3,5,3,5,3,0,0);

    //Z1^2
    //PKA_MMUL(0,3,14,3, 7,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3,14,3,7,3,7,3,0,0);

    //X1^2-Z1^2
    //PKA_MSUB(0,3,13,3,13,3,14,3,0);
    Sec_Eng_PKA_MSUB(3,13,3,13,3,14,3,0,0);

    //W = 3*(X1^2-Z1^2)
    //PKA_MMUL(0,3,13,3,10,3,13,3,0);
    Sec_Eng_PKA_MMUL(3,13,3,10,3,13,3,0,0);

    //S = Y1*Z1
    //PKA_MMUL(0,3,14,3, 6,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3,14,3,6,3,7,3,0,0);

    //X1*Y1
    //PKA_MMUL(0,3,15,3, 5,3, 6,3,0);
    Sec_Eng_PKA_MMUL(3,15,3,5,3,6,3,0,0);

    //W^2
    //PKA_MMUL(0,3, 7,3,13,3,13,3,0);
    Sec_Eng_PKA_MMUL(3,7,3,13,3,13,3,0,0);

    //B = X1*Y1*S
    //PKA_MMUL(0,3,15,3,15,3,14,3,0);
    Sec_Eng_PKA_MMUL(3,15,3,15,3,14,3,0,0);

    //8*B
    //PKA_MMUL(0,3, 5,3,12,3,15,3,0);
    Sec_Eng_PKA_MMUL(3,5,3,12,3,15,3,0,0);

    //H = W^2-8*B
    //PKA_MSUB(0,3, 7,3, 7,3, 5,3,0);
    Sec_Eng_PKA_MSUB(3,7,3,7,3,5,3,0,0);

    //2*H
    //PKA_MMUL(0,3, 5,3, 9,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3, 5,3,9,3,7,3,0,0);

    //X2 = 2*H*S
    //PKA_MMUL(0,3, 5,3, 5,3,14,3,0);
    Sec_Eng_PKA_MMUL(3, 5,3,5,3,14,3,0,0);

    //4*B
    //PKA_MMUL(0,3,15,3,11,3,15,3,0);
    Sec_Eng_PKA_MMUL(3,15,3,11,3,15,3,0,0);

    //S^2
    //PKA_MMUL(0,3,16,3,14,3,14,3,0);
    Sec_Eng_PKA_MMUL(3,16,3,14,3,14,3,0,0);

    //4*B-H
    //PKA_MSUB(0,3,15,3,15,3, 7,3,0);
    Sec_Eng_PKA_MSUB(3,15,3,15,3,7,3,0,0);

    //Y1^2
    //PKA_MMUL(0,3, 6,3, 6,3, 6,3,0);
    Sec_Eng_PKA_MMUL(3,6,3,6,3,6,3,0,0);

    //W*(4*B-H)
    //PKA_MMUL(0,3,15,3,15,3,13,3,0);
    Sec_Eng_PKA_MMUL(3,15,3,15,3,13,3,0,0);

    //8*Y1^2
    //PKA_MMUL(0,3, 6,3,12,3, 6,3,0);
    Sec_Eng_PKA_MMUL(3,6,3,12,3,6,3,0,0);

    //8*Y1^2*S^2
    //PKA_MMUL(0,3, 6,3, 6,3,16,3,0);
    Sec_Eng_PKA_MMUL(3,6,3,6,3,16,3,0,0);

    //Y2 = W*(4*B-H)-8*Y1^2*S^2
    //PKA_MSUB(0,3, 6,3,15,3, 6,3,0);
    Sec_Eng_PKA_MSUB(3,6,3,15,3,6,3,0,0);

    //S^3
    //PKA_MMUL(0,3, 7,3,14,3,16,3,0);
    Sec_Eng_PKA_MMUL(3,7,3,14,3,16,3,0,0);

    //Z2 = 8*S^3
    //PKA_MMUL(1,3, 7,3,12,3, 7,3,0);
    Sec_Eng_PKA_MMUL(3,7,3,12,3,7,3,0,1);

}

static int bflb_ecdsa_verify_point_mul(uint8_t id,const uint32_t *m )
{
    uint32_t i,j,k;
    uint32_t tmp;
    uint32_t isOne=0;
    uint8_t *p=(uint8_t *)m;
    uint8_t pka_p1_eq_inf,pka_p2_eq_inf;

    /* Remove zeros bytes*/
    k=0;
    while(p[k]==0&&k<31){
        k++;
    }
    i=31;
    for(;i>=k;i--){
        tmp=p[i];
        j=0;
        for(j=0;j<8;j++){
            isOne=tmp&(1<<j);
            if(isOne){
                bflb_ecdsa_point_add_inf_check(&pka_p1_eq_inf,&pka_p2_eq_inf);
                if(pka_p1_eq_inf==1 && pka_p2_eq_inf == 0){
                    //sum = X2
                    bflb_ecdsa_copy_x2_to_x1(id);
                }else if(pka_p1_eq_inf==0 && pka_p2_eq_inf == 1){
                    //sum = X1
                    MSG("sum = X1\r\n");
                }else if(pka_p1_eq_inf==0 && pka_p2_eq_inf == 0){
                    //sum = X1 + X2
                    bflb_ecdsa_point_add(id);
                }else{
                    MSG("Error! infinite point + infinite point\r\n");
                    return -1;
                }
            }
            bflb_ecdsa_point_double(id);
        }
        if(i==0){
            break;
        }
    }


    return 0;
}

// R = m * P
int blcrypto_suite_hacc_secp256r1_mul_bin(uint8_t id, const uint32_t *m, const uint32_t *Px, const uint32_t *Py, uint32_t *Rx, uint32_t *Ry)
{
    bflb_ecc_basic_parameter_init(id);

    //Clear D[7]
    //PKA_CREG(1,4, 7,0);
    Sec_Eng_PKA_CREG(ECP_SECP256R1_LT_REG_TYPE,7,ECP_SECP256R1_SIZE/4,1);

    bflb_ecc_point_mul_init(id);

    //X1
    //PKA_CTREG(3, 2,8,bar_Zero_x);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,2,(uint32_t *)secp256r1_Zerox,ECP_SECP256R1_SIZE/4,0);
    //Y1
    //PKA_CTREG(3, 3,8,bar_Zero_y);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,3,(uint32_t *)secp256r1_Zeroy,ECP_SECP256R1_SIZE/4,0);
    //Z1
    //PKA_CTREG(3, 4,8,bar_Zero_z);
    //PKA_MOVDAT(1,3, 4,3, 2);
    Sec_Eng_PKA_Move_Data(3,4,3,2,1);

    //X2
    //PKA_CTREG(3, 5,8,bar_G_x);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,5,Px,ECP_SECP256R1_SIZE/4,0);
    //Y2
    //PKA_CTREG(3, 6,8,bar_G_y);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,6,Py,ECP_SECP256R1_SIZE/4,0);
    //Z2
    //PKA_CTREG(3, 7,8,bar_G_z);
    //PKA_MOVDAT(1,3, 7,3, 3);
    Sec_Eng_PKA_Move_Data(3,7,3,3,1);
    /* Clear temp register since it's used in point-mul*/
    Sec_Eng_PKA_CREG(ECP_SECP256R1_LT_REG_TYPE,7,ECP_SECP256R1_SIZE/4,1);

    bflb_ecdsa_verify_point_mul(id, m);
    //get bar_u1_x
    Sec_Eng_PKA_Read_Data(ECP_SECP256R1_REG_TYPE,2,(uint32_t *)Rx,ECP_SECP256R1_SIZE/4);
    Sec_Eng_PKA_Read_Data(ECP_SECP256R1_REG_TYPE,3,(uint32_t *)Ry,ECP_SECP256R1_SIZE/4);

    //get R.x
    //R.z ^ -1
    Sec_Eng_PKA_MINV(ECP_SECP256R1_REG_TYPE,5,ECP_SECP256R1_REG_TYPE,4,ECP_SECP256R1_REG_TYPE,0,1);
    //inv_r
    //PKA_CTREG(3, 6,8,inv_r);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,6,(uint32_t *)secp256r1InvR_P,ECP_SECP256R1_SIZE/4,0);
    //R.z ^ -1
    Sec_Eng_PKA_CREG(ECP_SECP256R1_REG_TYPE,2*ECP_SECP256R1_LT_REG_INDEX-1,ECP_SECP256R1_SIZE/4,1);
    Sec_Eng_PKA_CREG(ECP_SECP256R1_REG_TYPE,2*ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_SIZE/4,1);
    //PKA_MONT2GF(3, 5,3, 5,3, 6);
    Sec_Eng_PKA_Mont2GF(ECP_SECP256R1_REG_TYPE,5, ECP_SECP256R1_REG_TYPE,5, ECP_SECP256R1_REG_TYPE, 6,
                            ECP_SECP256R1_LT_REG_TYPE,ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_REG_TYPE,0);

    //R.x (Montgomery to GF)
    //PKA_MONT2GF(3, 6,3, 2,3, 6);
    Sec_Eng_PKA_Mont2GF(ECP_SECP256R1_REG_TYPE,6, ECP_SECP256R1_REG_TYPE,2, ECP_SECP256R1_REG_TYPE, 6,
                            ECP_SECP256R1_LT_REG_TYPE,ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_REG_TYPE,0);

    //R.x (GF to Affine domain)
    //PKA_MONT2GF(3, 2,3, 5,3, 6);
    Sec_Eng_PKA_Mont2GF(ECP_SECP256R1_REG_TYPE,2, ECP_SECP256R1_REG_TYPE,5, ECP_SECP256R1_REG_TYPE, 6,
                            ECP_SECP256R1_LT_REG_TYPE,ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_REG_TYPE,0);
    Sec_Eng_PKA_Read_Data(ECP_SECP256R1_REG_TYPE,2,(uint32_t *)Rx,ECP_SECP256R1_SIZE/4);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_N_REG_INDEX,(uint32_t *)secp256r1N,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_MREM(ECP_SECP256R1_REG_TYPE,2,ECP_SECP256R1_REG_TYPE,2,
                        ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_N_REG_INDEX,1);
    Sec_Eng_PKA_Read_Data(ECP_SECP256R1_REG_TYPE,2,(uint32_t *)Rx,ECP_SECP256R1_SIZE/4);

    /*after %n,re write p*/
    Sec_Eng_PKA_Write_Data(SEC_ENG_PKA_REG_SIZE_32,0,(uint32_t *)secp256r1P,ECP_SECP256R1_SIZE/4,0);
    //get R.y
    //R.z ^ -1
    Sec_Eng_PKA_MINV(ECP_SECP256R1_REG_TYPE,5,ECP_SECP256R1_REG_TYPE,4,ECP_SECP256R1_REG_TYPE,0,1);
    //inv_r
    //PKA_CTREG(3, 6,8,inv_r);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,6,(uint32_t *)secp256r1InvR_P,ECP_SECP256R1_SIZE/4,0);
    //R.z ^ -1
    Sec_Eng_PKA_CREG(ECP_SECP256R1_REG_TYPE,2*ECP_SECP256R1_LT_REG_INDEX-1,ECP_SECP256R1_SIZE/4,1);
    Sec_Eng_PKA_CREG(ECP_SECP256R1_REG_TYPE,2*ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_SIZE/4,1);
    //PKA_MONT2GF(3, 5,3, 5,3, 6);
    Sec_Eng_PKA_Mont2GF(ECP_SECP256R1_REG_TYPE,5, ECP_SECP256R1_REG_TYPE,5, ECP_SECP256R1_REG_TYPE, 6,
                            ECP_SECP256R1_LT_REG_TYPE,ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_REG_TYPE,0);
    //R.x (Montgomery to GF)
    //PKA_MONT2GF(3, 6,3, 2,3, 6);
    Sec_Eng_PKA_Mont2GF(ECP_SECP256R1_REG_TYPE,6, ECP_SECP256R1_REG_TYPE,3, ECP_SECP256R1_REG_TYPE, 6,
                            ECP_SECP256R1_LT_REG_TYPE,ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_REG_TYPE,0);

    //R.x (GF to Affine domain)
    //PKA_MONT2GF(3, 2,3, 5,3, 6);
    Sec_Eng_PKA_Mont2GF(ECP_SECP256R1_REG_TYPE,3, ECP_SECP256R1_REG_TYPE,5, ECP_SECP256R1_REG_TYPE, 6,
                            ECP_SECP256R1_LT_REG_TYPE,ECP_SECP256R1_LT_REG_INDEX,ECP_SECP256R1_REG_TYPE,0);
    Sec_Eng_PKA_Read_Data(ECP_SECP256R1_REG_TYPE,3,(uint32_t *)Ry,ECP_SECP256R1_SIZE/4);
    Sec_Eng_PKA_Write_Data(ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_N_REG_INDEX,(uint32_t *)secp256r1N,ECP_SECP256R1_SIZE/4,0);
    Sec_Eng_PKA_MREM(ECP_SECP256R1_REG_TYPE,3,ECP_SECP256R1_REG_TYPE,3,
                        ECP_SECP256R1_REG_TYPE,ECP_SECP256R1_N_REG_INDEX,1);
    Sec_Eng_PKA_Read_Data(ECP_SECP256R1_REG_TYPE,3,(uint32_t *)Ry,ECP_SECP256R1_SIZE/4);
    return 0;
}

#define S32 SEC_ENG_PKA_REG_SIZE_32
#define S64 SEC_ENG_PKA_REG_SIZE_64

int blcrypto_suite_hacc_secp256r1_exp_mod(uint32_t *result, uint32_t *A, uint32_t *E)
{
    SEC_ENG_PKA_REG_SIZE_Type nregType=S32;
    SEC_ENG_PKA_REG_SIZE_Type lregType=S64;
    const uint32_t dataSize=32/4;

    /* 0:c
     * 1:NPrime_c
     * 2:invR_c
     * 4:a(mont domain)
     * 5:b
     * 6:a^b%c(mont domain)
     * 7:a^b%c(gf domain)
     * 10&11:2^size for GF2Mont*/
    Sec_Eng_PKA_Write_Data(nregType,0,(uint32_t *)secp256r1P,dataSize,1);
    Sec_Eng_PKA_Write_Data(nregType,1,(uint32_t *)secp256r1PrimeN_P,dataSize,1);
    Sec_Eng_PKA_Write_Data(nregType,2,(uint32_t *)secp256r1InvR_P,dataSize,1);

    /* change a into mont domain*/
    Sec_Eng_PKA_Write_Data(nregType,4,(uint32_t *)A,dataSize,1);
    Sec_Eng_PKA_CREG(nregType,10,dataSize,1);
    Sec_Eng_PKA_CREG(nregType,11,dataSize,1);
    Sec_Eng_PKA_GF2Mont(nregType,4,nregType,4,256,lregType,5,nregType,0);

    Sec_Eng_PKA_Write_Data(nregType,5,(uint32_t *)E,dataSize,1);
    /* a^b%c*/
    Sec_Eng_PKA_CREG(nregType, 6, dataSize, 1);
    Sec_Eng_PKA_MEXP(nregType,6,nregType,4,nregType,5,nregType,0,1);

    /* change result into gf domain*/
    Sec_Eng_PKA_CREG(nregType,10,dataSize,1);
    Sec_Eng_PKA_CREG(nregType,11,dataSize,1);
    /*index 2 is invertR*/
    Sec_Eng_PKA_Mont2GF(nregType,7, nregType,6, nregType, 2,lregType,5,nregType,0);

    Sec_Eng_PKA_Read_Data(nregType,7,(uint32_t *)result,dataSize);
    return 0;
}
#endif
