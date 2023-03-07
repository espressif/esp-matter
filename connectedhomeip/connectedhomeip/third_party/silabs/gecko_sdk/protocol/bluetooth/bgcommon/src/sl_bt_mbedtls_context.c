#include "sl_bt_mbedtls_context.h"

size_t sl_bt_get_mbedtls_aes_ctx_size()
{
  return sizeof(mbedtls_aes_context);
}

size_t sl_bt_get_mbedtls_cipher_ctx_size()
{
  return sizeof(mbedtls_cipher_context_t);
}

size_t sl_bt_get_mbedtls_crt_drbg_ctx_size()
{
  return sizeof(mbedtls_ctr_drbg_context);
}

size_t sl_bt_get_mbedtls_entropy_ctx_size()
{
  return sizeof(mbedtls_entropy_context);
}

size_t sl_bt_get_mbedtls_ccm_ctx_size()
{
  return sizeof(mbedtls_ccm_context);
}

size_t sl_bt_get_mbedtls_ecp_group_size()
{
  return sizeof(mbedtls_ecp_group);
}

size_t sl_bt_get_mbedtls_ecp_point_size()
{
  return sizeof(mbedtls_ecp_point);
}

size_t sl_bt_get_mbedtls_mpi_size()
{
  return sizeof(mbedtls_mpi);
}
