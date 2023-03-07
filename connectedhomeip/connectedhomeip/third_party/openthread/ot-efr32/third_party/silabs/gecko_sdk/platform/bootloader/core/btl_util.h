#ifndef BTL_UTIL_H
#define BTL_UTIL_H

#define BTL_STR_HELPER(x) #x
#define QUOTE(x) BTL_STR_HELPER(x)

#if defined(__CSTAT__)
#define MISRAC_DISABLE _Pragma( \
    "cstat_disable=                                                                     \
    \"MISRAC2012-Dir-4.3\",\"MISRAC2012-Dir-4.4\",\"MISRAC2012-Dir-4.5\",               \
    \"MISRAC2012-Dir-4.6_a\",\"MISRAC2012-Dir-4.6_b\",\"MISRAC2012-Dir-4.7_a\",         \
    \"MISRAC2012-Dir-4.7_b\",\"MISRAC2012-Dir-4.7_c\",\"MISRAC2012-Dir-4.8\",           \
    \"MISRAC2012-Dir-4.9\",\"MISRAC2012-Dir-4.10\",\"MISRAC2012-Dir-4.11_a\",           \
    \"MISRAC2012-Dir-4.11_b\",\"MISRAC2012-Dir-4.11_c\",\"MISRAC2012-Dir-4.11_d\",      \
    \"MISRAC2012-Dir-4.11_e\",\"MISRAC2012-Dir-4.11_f\",\"MISRAC2012-Dir-4.11_g\",      \
    \"MISRAC2012-Dir-4.11_h\",\"MISRAC2012-Dir-4.11_i\",\"MISRAC2012-Dir-4.12\",        \
    \"MISRAC2012-Dir-4.13_b\",\"MISRAC2012-Dir-4.13_c\",\"MISRAC2012-Dir-4.13_d\",      \
    \"MISRAC2012-Dir-4.13_e\",\"MISRAC2012-Dir-4.13_f\",\"MISRAC2012-Dir-4.13_g\",      \
    \"MISRAC2012-Dir-4.13_h\",\"MISRAC2012-Rule-1.3_a\",\"MISRAC2012-Rule-1.3_b\",      \
    \"MISRAC2012-Rule-1.3_c\",\"MISRAC2012-Rule-1.3_d\",\"MISRAC2012-Rule-1.3_e\",      \
    \"MISRAC2012-Rule-1.3_f\",\"MISRAC2012-Rule-1.3_g\",\"MISRAC2012-Rule-1.3_h\",      \
    \"MISRAC2012-Rule-1.3_i\",\"MISRAC2012-Rule-1.3_j\",\"MISRAC2012-Rule-1.3_k\",      \
    \"MISRAC2012-Rule-1.3_m\",\"MISRAC2012-Rule-1.3_n\",\"MISRAC2012-Rule-1.3_o\",      \
    \"MISRAC2012-Rule-1.3_p\",\"MISRAC2012-Rule-1.3_q\",\"MISRAC2012-Rule-1.3_r\",      \
    \"MISRAC2012-Rule-1.3_s\",\"MISRAC2012-Rule-1.3_t\",\"MISRAC2012-Rule-1.3_u\",      \
    \"MISRAC2012-Rule-1.3_v\",\"MISRAC2012-Rule-1.3_w\",\"MISRAC2012-Rule-2.1_a\",      \
    \"MISRAC2012-Rule-2.1_b\",\"MISRAC2012-Rule-2.2_a\",\"MISRAC2012-Rule-2.2_b\",      \
    \"MISRAC2012-Rule-2.2_c\",\"MISRAC2012-Rule-2.3\",\"MISRAC2012-Rule-2.4\",          \
    \"MISRAC2012-Rule-2.5\",\"MISRAC2012-Rule-2.6\",\"MISRAC2012-Rule-2.7\",            \
    \"MISRAC2012-Rule-3.1\",\"MISRAC2012-Rule-3.2\",\"MISRAC2012-Rule-5.1\",            \
    \"MISRAC2012-Rule-5.2_c89\",\"MISRAC2012-Rule-5.2_c99\",                            \
    \"MISRAC2012-Rule-5.3_c89\",\"MISRAC2012-Rule-5.3_c99\",                            \
    \"MISRAC2012-Rule-5.4_c89\",\"MISRAC2012-Rule-5.4_c99\",                            \
    \"MISRAC2012-Rule-5.5_c89\",\"MISRAC2012-Rule-5.5_c99\",                            \
    \"MISRAC2012-Rule-5.6\",\"MISRAC2012-Rule-5.7\",\"MISRAC2012-Rule-5.8\",            \
    \"MISRAC2012-Rule-5.9\",\"MISRAC2012-Rule-6.1\",\"MISRAC2012-Rule-6.2\",            \
    \"MISRAC2012-Rule-7.1\",\"MISRAC2012-Rule-7.2\",\"MISRAC2012-Rule-7.3\",            \
    \"MISRAC2012-Rule-7.4_a\",\"MISRAC2012-Rule-7.4_b\",\"MISRAC2012-Rule-8.1\",        \
    \"MISRAC2012-Rule-8.2_a\",\"MISRAC2012-Rule-8.2_b\",\"MISRAC2012-Rule-8.3_b\",      \
    \"MISRAC2012-Rule-8.4\",\"MISRAC2012-Rule-8.5_a\",\"MISRAC2012-Rule-8.5_b\",        \
    \"MISRAC2012-Rule-8.6\",\"MISRAC2012-Rule-8.7\",\"MISRAC2012-Rule-8.9_a\",          \
    \"MISRAC2012-Rule-8.9_b\",\"MISRAC2012-Rule-8.10\",\"MISRAC2012-Rule-8.11\",        \
    \"MISRAC2012-Rule-8.12\",\"MISRAC2012-Rule-8.13\",\"MISRAC2012-Rule-8.14\",         \
    \"MISRAC2012-Rule-9.1_a\",\"MISRAC2012-Rule-9.1_b\",\"MISRAC2012-Rule-9.1_c\",      \
    \"MISRAC2012-Rule-9.1_d\",\"MISRAC2012-Rule-9.1_e\",\"MISRAC2012-Rule-9.1_f\",      \
    \"MISRAC2012-Rule-9.2\",\"MISRAC2012-Rule-9.3\",\"MISRAC2012-Rule-9.4\",            \
    \"MISRAC2012-Rule-9.5_a\",\"MISRAC2012-Rule-9.5_b\",\"MISRAC2012-Rule-10.1_R2\",    \
    \"MISRAC2012-Rule-10.1_R3\",\"MISRAC2012-Rule-10.1_R4\",                            \
    \"MISRAC2012-Rule-10.1_R5\",\"MISRAC2012-Rule-10.1_R6\",                            \
    \"MISRAC2012-Rule-10.1_R7\",\"MISRAC2012-Rule-10.1_R8\",                            \
    \"MISRAC2012-Rule-10.2\",\"MISRAC2012-Rule-10.3\",\"MISRAC2012-Rule-10.4_a\",       \
    \"MISRAC2012-Rule-10.4_b\",\"MISRAC2012-Rule-10.5\",\"MISRAC2012-Rule-10.6\",       \
    \"MISRAC2012-Rule-10.7\",\"MISRAC2012-Rule-10.8\",\"MISRAC2012-Rule-11.1\",         \
    \"MISRAC2012-Rule-11.2\",\"MISRAC2012-Rule-11.3\",\"MISRAC2012-Rule-11.4\",         \
    \"MISRAC2012-Rule-11.5\",\"MISRAC2012-Rule-11.6\",\"MISRAC2012-Rule-11.7\",         \
    \"MISRAC2012-Rule-11.8\",\"MISRAC2012-Rule-11.9\",\"MISRAC2012-Rule-12.1\",         \
    \"MISRAC2012-Rule-12.2\",\"MISRAC2012-Rule-12.3\",\"MISRAC2012-Rule-13.1\",         \
    \"MISRAC2012-Rule-13.2_a\",\"MISRAC2012-Rule-13.2_b\",\"MISRAC2012-Rule-13.2_c\",   \
    \"MISRAC2012-Rule-13.3\",\"MISRAC2012-Rule-13.4_a\",\"MISRAC2012-Rule-13.4_b\",     \
    \"MISRAC2012-Rule-13.5\",\"MISRAC2012-Rule-13.6\",\"MISRAC2012-Rule-14.1_a\",       \
    \"MISRAC2012-Rule-14.1_b\",\"MISRAC2012-Rule-14.2\",\"MISRAC2012-Rule-14.3_a\",     \
    \"MISRAC2012-Rule-14.3_b\",\"MISRAC2012-Rule-14.4_a\",\"MISRAC2012-Rule-14.4_b\",   \
    \"MISRAC2012-Rule-14.4_c\",\"MISRAC2012-Rule-14.4_d\",\"MISRAC2012-Rule-15.1\",     \
    \"MISRAC2012-Rule-15.2\",\"MISRAC2012-Rule-15.3\",\"MISRAC2012-Rule-15.4\",         \
    \"MISRAC2012-Rule-15.5\",\"MISRAC2012-Rule-15.6_a\",\"MISRAC2012-Rule-15.6_b\",     \
    \"MISRAC2012-Rule-15.6_c\",\"MISRAC2012-Rule-15.6_d\",\"MISRAC2012-Rule-15.6_e\",   \
    \"MISRAC2012-Rule-15.7\",\"MISRAC2012-Rule-16.1\",\"MISRAC2012-Rule-16.2\",         \
    \"MISRAC2012-Rule-16.3\",\"MISRAC2012-Rule-16.4\",\"MISRAC2012-Rule-16.5\",         \
    \"MISRAC2012-Rule-16.6\",\"MISRAC2012-Rule-16.7\",\"MISRAC2012-Rule-17.1\",         \
    \"MISRAC2012-Rule-17.2_a\",\"MISRAC2012-Rule-17.2_b\",\"MISRAC2012-Rule-17.3\",     \
    \"MISRAC2012-Rule-17.4\",\"MISRAC2012-Rule-17.5\",\"MISRAC2012-Rule-17.6\",         \
    \"MISRAC2012-Rule-17.7\",\"MISRAC2012-Rule-17.8\",\"MISRAC2012-Rule-18.1_a\",       \
    \"MISRAC2012-Rule-18.1_b\",\"MISRAC2012-Rule-18.1_c\",\"MISRAC2012-Rule-18.1_d\",   \
    \"MISRAC2012-Rule-18.2\",\"MISRAC2012-Rule-18.3\",\"MISRAC2012-Rule-18.4\",         \
    \"MISRAC2012-Rule-18.5\",\"MISRAC2012-Rule-18.6_a\",\"MISRAC2012-Rule-18.6_b\",     \
    \"MISRAC2012-Rule-18.6_c\",\"MISRAC2012-Rule-18.6_d\",\"MISRAC2012-Rule-18.7\",     \
    \"MISRAC2012-Rule-18.8\",\"MISRAC2012-Rule-19.1\",\"MISRAC2012-Rule-19.2\",         \
    \"MISRAC2012-Rule-20.1\",\"MISRAC2012-Rule-20.2\",\"MISRAC2012-Rule-20.4_c89\",     \
    \"MISRAC2012-Rule-20.4_c99\",\"MISRAC2012-Rule-20.5\",\"MISRAC2012-Rule-20.7\",     \
    \"MISRAC2012-Rule-20.10\",\"MISRAC2012-Rule-21.1\",\"MISRAC2012-Rule-21.2\",        \
    \"MISRAC2012-Rule-21.3\",\"MISRAC2012-Rule-21.4\",\"MISRAC2012-Rule-21.5\",         \
    \"MISRAC2012-Rule-21.6\",\"MISRAC2012-Rule-21.7\",\"MISRAC2012-Rule-21.8\",         \
    \"MISRAC2012-Rule-21.9\",\"MISRAC2012-Rule-21.10\",\"MISRAC2012-Rule-21.11\",       \
    \"MISRAC2012-Rule-21.12_a\",\"MISRAC2012-Rule-21.12_b\",\"MISRAC2012-Rule-22.1_a\", \
    \"MISRAC2012-Rule-22.1_b\",\"MISRAC2012-Rule-22.2_a\",\"MISRAC2012-Rule-22.2_b\",   \
    \"MISRAC2012-Rule-22.2_c\",\"MISRAC2012-Rule-22.3\",\"MISRAC2012-Rule-22.4\",       \
    \"MISRAC2012-Rule-22.5_a\",\"MISRAC2012-Rule-22.5_b\",\"MISRAC2012-Rule-22.6\"")

#define MISRAC_ENABLE  _Pragma( \
    "cstat_restore=                                                                     \
    \"MISRAC2012-Dir-4.3\",\"MISRAC2012-Dir-4.4\",\"MISRAC2012-Dir-4.5\",               \
    \"MISRAC2012-Dir-4.6_a\",\"MISRAC2012-Dir-4.6_b\",\"MISRAC2012-Dir-4.7_a\",         \
    \"MISRAC2012-Dir-4.7_b\",\"MISRAC2012-Dir-4.7_c\",\"MISRAC2012-Dir-4.8\",           \
    \"MISRAC2012-Dir-4.9\",\"MISRAC2012-Dir-4.10\",\"MISRAC2012-Dir-4.11_a\",           \
    \"MISRAC2012-Dir-4.11_b\",\"MISRAC2012-Dir-4.11_c\",\"MISRAC2012-Dir-4.11_d\",      \
    \"MISRAC2012-Dir-4.11_e\",\"MISRAC2012-Dir-4.11_f\",\"MISRAC2012-Dir-4.11_g\",      \
    \"MISRAC2012-Dir-4.11_h\",\"MISRAC2012-Dir-4.11_i\",\"MISRAC2012-Dir-4.12\",        \
    \"MISRAC2012-Dir-4.13_b\",\"MISRAC2012-Dir-4.13_c\",\"MISRAC2012-Dir-4.13_d\",      \
    \"MISRAC2012-Dir-4.13_e\",\"MISRAC2012-Dir-4.13_f\",\"MISRAC2012-Dir-4.13_g\",      \
    \"MISRAC2012-Dir-4.13_h\",\"MISRAC2012-Rule-1.3_a\",\"MISRAC2012-Rule-1.3_b\",      \
    \"MISRAC2012-Rule-1.3_c\",\"MISRAC2012-Rule-1.3_d\",\"MISRAC2012-Rule-1.3_e\",      \
    \"MISRAC2012-Rule-1.3_f\",\"MISRAC2012-Rule-1.3_g\",\"MISRAC2012-Rule-1.3_h\",      \
    \"MISRAC2012-Rule-1.3_i\",\"MISRAC2012-Rule-1.3_j\",\"MISRAC2012-Rule-1.3_k\",      \
    \"MISRAC2012-Rule-1.3_m\",\"MISRAC2012-Rule-1.3_n\",\"MISRAC2012-Rule-1.3_o\",      \
    \"MISRAC2012-Rule-1.3_p\",\"MISRAC2012-Rule-1.3_q\",\"MISRAC2012-Rule-1.3_r\",      \
    \"MISRAC2012-Rule-1.3_s\",\"MISRAC2012-Rule-1.3_t\",\"MISRAC2012-Rule-1.3_u\",      \
    \"MISRAC2012-Rule-1.3_v\",\"MISRAC2012-Rule-1.3_w\",\"MISRAC2012-Rule-2.1_a\",      \
    \"MISRAC2012-Rule-2.1_b\",\"MISRAC2012-Rule-2.2_a\",\"MISRAC2012-Rule-2.2_b\",      \
    \"MISRAC2012-Rule-2.2_c\",\"MISRAC2012-Rule-2.3\",\"MISRAC2012-Rule-2.4\",          \
    \"MISRAC2012-Rule-2.5\",\"MISRAC2012-Rule-2.6\",\"MISRAC2012-Rule-2.7\",            \
    \"MISRAC2012-Rule-3.1\",\"MISRAC2012-Rule-3.2\",\"MISRAC2012-Rule-5.1\",            \
    \"MISRAC2012-Rule-5.2_c89\",\"MISRAC2012-Rule-5.2_c99\",                            \
    \"MISRAC2012-Rule-5.3_c89\",\"MISRAC2012-Rule-5.3_c99\",                            \
    \"MISRAC2012-Rule-5.4_c89\",\"MISRAC2012-Rule-5.4_c99\",                            \
    \"MISRAC2012-Rule-5.5_c89\",\"MISRAC2012-Rule-5.5_c99\",                            \
    \"MISRAC2012-Rule-5.6\",\"MISRAC2012-Rule-5.7\",\"MISRAC2012-Rule-5.8\",            \
    \"MISRAC2012-Rule-5.9\",\"MISRAC2012-Rule-6.1\",\"MISRAC2012-Rule-6.2\",            \
    \"MISRAC2012-Rule-7.1\",\"MISRAC2012-Rule-7.2\",\"MISRAC2012-Rule-7.3\",            \
    \"MISRAC2012-Rule-7.4_a\",\"MISRAC2012-Rule-7.4_b\",\"MISRAC2012-Rule-8.1\",        \
    \"MISRAC2012-Rule-8.2_a\",\"MISRAC2012-Rule-8.2_b\",\"MISRAC2012-Rule-8.3_b\",      \
    \"MISRAC2012-Rule-8.4\",\"MISRAC2012-Rule-8.5_a\",\"MISRAC2012-Rule-8.5_b\",        \
    \"MISRAC2012-Rule-8.6\",\"MISRAC2012-Rule-8.7\",\"MISRAC2012-Rule-8.9_a\",          \
    \"MISRAC2012-Rule-8.9_b\",\"MISRAC2012-Rule-8.10\",\"MISRAC2012-Rule-8.11\",        \
    \"MISRAC2012-Rule-8.12\",\"MISRAC2012-Rule-8.13\",\"MISRAC2012-Rule-8.14\",         \
    \"MISRAC2012-Rule-9.1_a\",\"MISRAC2012-Rule-9.1_b\",\"MISRAC2012-Rule-9.1_c\",      \
    \"MISRAC2012-Rule-9.1_d\",\"MISRAC2012-Rule-9.1_e\",\"MISRAC2012-Rule-9.1_f\",      \
    \"MISRAC2012-Rule-9.2\",\"MISRAC2012-Rule-9.3\",\"MISRAC2012-Rule-9.4\",            \
    \"MISRAC2012-Rule-9.5_a\",\"MISRAC2012-Rule-9.5_b\",\"MISRAC2012-Rule-10.1_R2\",    \
    \"MISRAC2012-Rule-10.1_R3\",\"MISRAC2012-Rule-10.1_R4\",                            \
    \"MISRAC2012-Rule-10.1_R5\",\"MISRAC2012-Rule-10.1_R6\",                            \
    \"MISRAC2012-Rule-10.1_R7\",\"MISRAC2012-Rule-10.1_R8\",                            \
    \"MISRAC2012-Rule-10.2\",\"MISRAC2012-Rule-10.3\",\"MISRAC2012-Rule-10.4_a\",       \
    \"MISRAC2012-Rule-10.4_b\",\"MISRAC2012-Rule-10.5\",\"MISRAC2012-Rule-10.6\",       \
    \"MISRAC2012-Rule-10.7\",\"MISRAC2012-Rule-10.8\",\"MISRAC2012-Rule-11.1\",         \
    \"MISRAC2012-Rule-11.2\",\"MISRAC2012-Rule-11.3\",\"MISRAC2012-Rule-11.4\",         \
    \"MISRAC2012-Rule-11.5\",\"MISRAC2012-Rule-11.6\",\"MISRAC2012-Rule-11.7\",         \
    \"MISRAC2012-Rule-11.8\",\"MISRAC2012-Rule-11.9\",\"MISRAC2012-Rule-12.1\",         \
    \"MISRAC2012-Rule-12.2\",\"MISRAC2012-Rule-12.3\",\"MISRAC2012-Rule-13.1\",         \
    \"MISRAC2012-Rule-13.2_a\",\"MISRAC2012-Rule-13.2_b\",\"MISRAC2012-Rule-13.2_c\",   \
    \"MISRAC2012-Rule-13.3\",\"MISRAC2012-Rule-13.4_a\",\"MISRAC2012-Rule-13.4_b\",     \
    \"MISRAC2012-Rule-13.5\",\"MISRAC2012-Rule-13.6\",\"MISRAC2012-Rule-14.1_a\",       \
    \"MISRAC2012-Rule-14.1_b\",\"MISRAC2012-Rule-14.2\",\"MISRAC2012-Rule-14.3_a\",     \
    \"MISRAC2012-Rule-14.3_b\",\"MISRAC2012-Rule-14.4_a\",\"MISRAC2012-Rule-14.4_b\",   \
    \"MISRAC2012-Rule-14.4_c\",\"MISRAC2012-Rule-14.4_d\",\"MISRAC2012-Rule-15.1\",     \
    \"MISRAC2012-Rule-15.2\",\"MISRAC2012-Rule-15.3\",\"MISRAC2012-Rule-15.4\",         \
    \"MISRAC2012-Rule-15.5\",\"MISRAC2012-Rule-15.6_a\",\"MISRAC2012-Rule-15.6_b\",     \
    \"MISRAC2012-Rule-15.6_c\",\"MISRAC2012-Rule-15.6_d\",\"MISRAC2012-Rule-15.6_e\",   \
    \"MISRAC2012-Rule-15.7\",\"MISRAC2012-Rule-16.1\",\"MISRAC2012-Rule-16.2\",         \
    \"MISRAC2012-Rule-16.3\",\"MISRAC2012-Rule-16.4\",\"MISRAC2012-Rule-16.5\",         \
    \"MISRAC2012-Rule-16.6\",\"MISRAC2012-Rule-16.7\",\"MISRAC2012-Rule-17.1\",         \
    \"MISRAC2012-Rule-17.2_a\",\"MISRAC2012-Rule-17.2_b\",\"MISRAC2012-Rule-17.3\",     \
    \"MISRAC2012-Rule-17.4\",\"MISRAC2012-Rule-17.5\",\"MISRAC2012-Rule-17.6\",         \
    \"MISRAC2012-Rule-17.7\",\"MISRAC2012-Rule-17.8\",\"MISRAC2012-Rule-18.1_a\",       \
    \"MISRAC2012-Rule-18.1_b\",\"MISRAC2012-Rule-18.1_c\",\"MISRAC2012-Rule-18.1_d\",   \
    \"MISRAC2012-Rule-18.2\",\"MISRAC2012-Rule-18.3\",\"MISRAC2012-Rule-18.4\",         \
    \"MISRAC2012-Rule-18.5\",\"MISRAC2012-Rule-18.6_a\",\"MISRAC2012-Rule-18.6_b\",     \
    \"MISRAC2012-Rule-18.6_c\",\"MISRAC2012-Rule-18.6_d\",\"MISRAC2012-Rule-18.7\",     \
    \"MISRAC2012-Rule-18.8\",\"MISRAC2012-Rule-19.1\",\"MISRAC2012-Rule-19.2\",         \
    \"MISRAC2012-Rule-20.1\",\"MISRAC2012-Rule-20.2\",\"MISRAC2012-Rule-20.4_c89\",     \
    \"MISRAC2012-Rule-20.4_c99\",\"MISRAC2012-Rule-20.5\",\"MISRAC2012-Rule-20.7\",     \
    \"MISRAC2012-Rule-20.10\",\"MISRAC2012-Rule-21.1\",\"MISRAC2012-Rule-21.2\",        \
    \"MISRAC2012-Rule-21.3\",\"MISRAC2012-Rule-21.4\",\"MISRAC2012-Rule-21.5\",         \
    \"MISRAC2012-Rule-21.6\",\"MISRAC2012-Rule-21.7\",\"MISRAC2012-Rule-21.8\",         \
    \"MISRAC2012-Rule-21.9\",\"MISRAC2012-Rule-21.10\",\"MISRAC2012-Rule-21.11\",       \
    \"MISRAC2012-Rule-21.12_a\",\"MISRAC2012-Rule-21.12_b\",\"MISRAC2012-Rule-22.1_a\", \
    \"MISRAC2012-Rule-22.1_b\",\"MISRAC2012-Rule-22.2_a\",\"MISRAC2012-Rule-22.2_b\",   \
    \"MISRAC2012-Rule-22.2_c\",\"MISRAC2012-Rule-22.3\",\"MISRAC2012-Rule-22.4\",       \
    \"MISRAC2012-Rule-22.5_a\",\"MISRAC2012-Rule-22.5_b\",\"MISRAC2012-Rule-22.6\"")

#elif defined(__ICCARM__)
#define MISRAC_DISABLE _Pragma( \
    "diag_suppress= \
    Pm001,Pm002,Pm003,Pm004,Pm005,Pm006,Pm007,Pm008,Pm009,Pm010,Pm011,\
    Pm012,Pm013,Pm014,Pm015,Pm016,Pm017,Pm018,Pm019,Pm020,Pm021,Pm022,\
    Pm023,Pm024,Pm025,Pm026,Pm027,Pm028,Pm029,Pm030,Pm031,Pm032,Pm033,\
    Pm034,Pm035,Pm036,Pm037,Pm038,Pm039,Pm040,Pm041,Pm042,Pm043,Pm044,\
    Pm045,Pm046,Pm047,Pm048,Pm049,Pm050,Pm051,Pm052,Pm053,Pm054,Pm055,\
    Pm056,Pm057,Pm058,Pm059,Pm060,Pm061,Pm062,Pm063,Pm064,Pm065,Pm066,\
    Pm067,Pm068,Pm069,Pm070,Pm071,Pm072,Pm073,Pm074,Pm075,Pm076,Pm077,\
    Pm078,Pm079,Pm080,Pm081,Pm082,Pm083,Pm084,Pm085,Pm086,Pm087,Pm088,\
    Pm089,Pm090,Pm091,Pm092,Pm093,Pm094,Pm095,Pm096,Pm097,Pm098,Pm099,\
    Pm100,Pm101,Pm102,Pm103,Pm104,Pm105,Pm106,Pm107,Pm108,Pm109,Pm110,\
    Pm111,Pm112,Pm113,Pm114,Pm115,Pm116,Pm117,Pm118,Pm119,Pm120,Pm121,\
    Pm122,Pm123,Pm124,Pm125,Pm126,Pm127,Pm128,Pm129,Pm130,Pm131,Pm132,\
    Pm133,Pm134,Pm135,Pm136,Pm137,Pm138,Pm139,Pm140,Pm141,Pm142,Pm143,\
    Pm144,Pm145,Pm146,Pm147,Pm148,Pm149,Pm150,Pm151,Pm152,Pm153,Pm154,\
    Pm155")
#define MISRAC_ENABLE  _Pragma( \
    "diag_default= \
    Pm001,Pm002,Pm003,Pm004,Pm005,Pm006,Pm007,Pm008,Pm009,Pm010,Pm011,\
    Pm012,Pm013,Pm014,Pm015,Pm016,Pm017,Pm018,Pm019,Pm020,Pm021,Pm022,\
    Pm023,Pm024,Pm025,Pm026,Pm027,Pm028,Pm029,Pm030,Pm031,Pm032,Pm033,\
    Pm034,Pm035,Pm036,Pm037,Pm038,Pm039,Pm040,Pm041,Pm042,Pm043,Pm044,\
    Pm045,Pm046,Pm047,Pm048,Pm049,Pm050,Pm051,Pm052,Pm053,Pm054,Pm055,\
    Pm056,Pm057,Pm058,Pm059,Pm060,Pm061,Pm062,Pm063,Pm064,Pm065,Pm066,\
    Pm067,Pm068,Pm069,Pm070,Pm071,Pm072,Pm073,Pm074,Pm075,Pm076,Pm077,\
    Pm078,Pm079,Pm080,Pm081,Pm082,Pm083,Pm084,Pm085,Pm086,Pm087,Pm088,\
    Pm089,Pm090,Pm091,Pm092,Pm093,Pm094,Pm095,Pm096,Pm097,Pm098,Pm099,\
    Pm100,Pm101,Pm102,Pm103,Pm104,Pm105,Pm106,Pm107,Pm108,Pm109,Pm110,\
    Pm111,Pm112,Pm113,Pm114,Pm115,Pm116,Pm117,Pm118,Pm119,Pm120,Pm121,\
    Pm122,Pm123,Pm124,Pm125,Pm126,Pm127,Pm128,Pm129,Pm130,Pm131,Pm132,\
    Pm133,Pm134,Pm135,Pm136,Pm137,Pm138,Pm139,Pm140,Pm141,Pm142,Pm143,\
    Pm144,Pm145,Pm146,Pm147,Pm148,Pm149,Pm150,Pm151,Pm152,Pm153,Pm154,\
    Pm155")
#else
#define MISRAC_DISABLE
#define MISRAC_ENABLE
#endif

#endif
