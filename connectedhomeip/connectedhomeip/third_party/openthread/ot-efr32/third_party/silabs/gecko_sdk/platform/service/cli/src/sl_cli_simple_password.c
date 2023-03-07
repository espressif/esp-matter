/***************************************************************************//**
 * @file
 * @brief CLI simple password protection.
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_cli_simple_password_config.h"
#include "sl_cli_simple_password.h"
#include "sl_status.h"
#include "sl_iostream.h"

#include "sl_cli.h"
#include "sl_cli_config.h"
#include "sl_cli_types.h"
#include "sl_enum.h"
#include "sl_sleeptimer.h"

#include "em_core.h"
#include "ecode.h"

#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "psa/crypto.h"
#include "psa_crypto_storage.h"

#include "nvm3.h"

#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#define CLI_SECURITY_PSA_CRYPTO_KEY_ID                    (0x00004202)
#define CLI_SECURITY_NVM3_KEY_ID_ENCRYPTED_PASSWORD_HASH  (SL_CLI_NVM3_KEY_BEGIN)
#define CLI_SECURITY_NVM3_KEY_ID_IV                       (SL_CLI_NVM3_KEY_BEGIN + 1)
#define CLI_SECURITY_NVM3_KEY_ID_WARN_FLAG                (SL_CLI_NVM3_KEY_BEGIN + 2)
#define CLI_SECURITY_NVM3_KEY_ID_LOCK_TIMER               (SL_CLI_NVM3_KEY_BEGIN + 3)
#define CLI_SECURITY_NVM3_KEY_ID_RETRY_COUNTER            (SL_CLI_NVM3_KEY_BEGIN + 4)

SL_ENUM(state_t){
  INIT = 0,
  ASK_NEW_PASSWORD,
  WAITING_NEW_PASSWORD,
  ASK_NEW_PASSWORD_CONFIRMATION,
  WAITING_NEW_PASSWORD_CONFIRMATION,
  ASK_PASSWORD,
  WAITING_PASSWORD,
  LOCKOUT,
  LOGGED_IN,
  CONFIGURE,
  CONFIGURING,
  CONFIGURED,
};

static psa_algorithm_t psa_algo = PSA_ALG_CBC_NO_PADDING;
static state_t module_state = INIT;
static sl_cli_handle_t instance_initialize_password = NULL;
static bool security_warning = false;
static bool lock_timer_running = false;
static uint8_t retry_count = 0;

#define CIPHER_BLOCK_SIZE     PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)
#define BLOCK_SIZE   32
static uint8_t encrypted_password_hash[BLOCK_SIZE];
static uint8_t iv[CIPHER_BLOCK_SIZE];

typedef struct  {
  char buffer[SL_CLI_SIMPLE_PASSWORD_LENGTH_MAX + 1];
  char tmp_password[SL_CLI_SIMPLE_PASSWORD_LENGTH_MAX + 1];
  psa_key_id_t key_id;
  state_t state;
  uint32_t buffer_ix;
  uint8_t wrong_password_cnt;
  bool locked;
  sl_sleeptimer_timer_handle_t timer;
} cli_instance_data_session_t;

/***************************************************************************//**
 * @brief  Generate and store an encryption key
 ******************************************************************************/
static sl_status_t generate_encryption_key(psa_key_id_t * key_id)
{
  // Set key attributes.
  psa_status_t status;
  psa_key_attributes_t key_attr;

  key_attr = psa_key_attributes_init();
  psa_set_key_id(&key_attr, CLI_SECURITY_PSA_CRYPTO_KEY_ID);
  psa_set_key_algorithm(&key_attr, psa_algo);
  psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_type(&key_attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attr, 128);
  psa_set_key_lifetime(&key_attr, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT, PSA_KEY_LOCATION_LOCAL_STORAGE));
  status = psa_generate_key(&key_attr, key_id);
  EFM_ASSERT(status == PSA_SUCCESS);

  psa_reset_key_attributes(&key_attr);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief  Encrypt/Decypt Cipher Operation
 ******************************************************************************/
static psa_status_t cipher_operation(psa_cipher_operation_t * operation,
                                     const uint8_t * input,
                                     size_t input_size,
                                     size_t part_size,
                                     uint8_t * output,
                                     size_t output_size,
                                     size_t * output_len)
{
  psa_status_t psa_status;
  size_t bytes_to_write = 0, bytes_written = 0, len = 0;

  *output_len = 0;
  while ( bytes_written != input_size ) {
    bytes_to_write = (input_size - bytes_written > part_size
                      ? part_size
                      : input_size - bytes_written);

    psa_status = psa_cipher_update(operation, input + bytes_written,
                                   bytes_to_write, output + *output_len,
                                   output_size - *output_len, &len);
    EFM_ASSERT(psa_status == PSA_SUCCESS);

    bytes_written += bytes_to_write;
    *output_len += len;
  }

  psa_status = psa_cipher_finish(operation, output + *output_len,
                                 output_size - *output_len, &len);
  EFM_ASSERT(psa_status == PSA_SUCCESS);
  *output_len += len;

  return(psa_status);
}

/***************************************************************************//**
 * @brief Encrypt Data
 ******************************************************************************/
static psa_status_t cipher_encrypt(uint8_t * iv,
                                   size_t iv_size,
                                   const uint8_t * input,
                                   size_t input_size,
                                   uint8_t * output,
                                   size_t output_size,
                                   size_t * output_len)
{
  psa_status_t psa_status;
  psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
  size_t iv_len = 0;

  if (!psa_is_key_present_in_storage(CLI_SECURITY_PSA_CRYPTO_KEY_ID)) {
    // Key has been destroyed. Generate a new one
    psa_key_id_t key_id;
    sl_status_t status = generate_encryption_key(&key_id);
    EFM_ASSERT(status == SL_STATUS_OK);
    EFM_ASSERT(key_id == CLI_SECURITY_PSA_CRYPTO_KEY_ID);
  }

  mbedtls_platform_zeroize(&operation, sizeof(operation) );
  psa_status = psa_cipher_encrypt_setup(&operation, CLI_SECURITY_PSA_CRYPTO_KEY_ID, psa_algo);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_status = psa_cipher_generate_iv(&operation, iv, iv_size, &iv_len);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_status = cipher_operation(&operation, input, input_size, BLOCK_SIZE,
                                output, output_size, output_len);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_cipher_abort(&operation);
  return(psa_status);
}

/***************************************************************************//**
 * @brief Decrypt Data
 ******************************************************************************/
static psa_status_t cipher_decrypt(const uint8_t * iv,
                                   size_t iv_size,
                                   const uint8_t * input,
                                   size_t input_size,
                                   uint8_t * output,
                                   size_t output_size,
                                   size_t * output_len)
{
  psa_status_t psa_status;
  psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;

  mbedtls_platform_zeroize(&operation, sizeof(operation) );
  psa_status = psa_cipher_decrypt_setup(&operation, CLI_SECURITY_PSA_CRYPTO_KEY_ID, psa_algo);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_status = psa_cipher_set_iv(&operation, iv, iv_size);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_status = cipher_operation(&operation, input, input_size, BLOCK_SIZE,
                                output, output_size, output_len);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_cipher_abort(&operation);
  return(psa_status);
}

/***************************************************************************//**
 * @brief Validate if a provided password match the password configured
 ******************************************************************************/
static bool is_valid_password(char *password)
{
  uint8_t decrypted_hash[BLOCK_SIZE];
  size_t length;
  psa_status_t psa_status;
  psa_hash_operation_t operation;

  cipher_decrypt(iv, sizeof(iv),
                 encrypted_password_hash, sizeof(encrypted_password_hash),
                 decrypted_hash, sizeof(decrypted_hash), &length);

  length = strlen(password);
  operation = psa_hash_operation_init();

  psa_status = psa_hash_setup(&operation, PSA_ALG_SHA_256);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_status = psa_hash_update(&operation, (uint8_t *)password, length);
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  psa_status = psa_hash_verify(&operation, decrypted_hash, sizeof(decrypted_hash));

  psa_hash_abort(&operation);
  mbedtls_platform_zeroize(decrypted_hash, sizeof(decrypted_hash));

  if (psa_status == PSA_SUCCESS) {
    return true;
  }

  return false;
}

/***************************************************************************//**
 * @brief Output string with Line feed (LF) on the cli instance stream
 ******************************************************************************/
static void write_string_lf(sl_cli_handle_t handle,
                            char *str)
{
  sl_iostream_write(handle->iostream_handle, str, strlen(str));
  sl_iostream_putchar(handle->iostream_handle, '\n');
}

/***************************************************************************//**
 * @brief  Read data from Terminal
 ******************************************************************************/
static sl_status_t read_until_line_feed(sl_cli_handle_t handle,
                                        cli_instance_data_session_t *session)
{
  state_t s_state;
  sl_cli_handle_t cli_init;
  sl_status_t status = SL_STATUS_OK;
  bool read = true;
  char c;

  CORE_ATOMIC_SECTION(s_state = session->state; \
                      cli_init = instance_initialize_password; );

  do {
#if defined(SL_CATALOG_KERNEL_PRESENT)
    status = sl_iostream_getchar(handle->iostream_handle, &c);
#else
    if (handle->input_char == EOF) {
      status = sl_iostream_getchar(handle->iostream_handle, &c);
    } else {
      // Char read before going to sleep by the sl_cli_is_ok_to_sleep()
      c = handle->input_char;
      handle->input_char = EOF;
    }
#endif

    if (status == SL_STATUS_EMPTY) {
      return SL_STATUS_IN_PROGRESS;
    } else if (status == SL_STATUS_OK) {
      CORE_DECLARE_IRQ_STATE;

      CORE_ENTER_ATOMIC();
      if (module_state == CONFIGURE) {
        // Start receiving password on this instance;
        // Block other instance for configuring until completed by the current instance
        module_state = CONFIGURING;
        instance_initialize_password = handle;
      }
      CORE_EXIT_ATOMIC();

      if ((c == '\n') || (c == '\r')) {  // line feed
        if (session->buffer_ix > 0) {
          read = false;
          session->buffer[session->buffer_ix] = '\0';
          sl_iostream_putchar(handle->iostream_handle, c);
          return SL_STATUS_OK;
        }
      } else if (c == '\b') { // backspace
        if (session->buffer_ix > 0) { // underflow check
          session->buffer_ix--;
        }
        // Erase char on terminal
        sl_iostream_putchar(handle->iostream_handle, '\b');
        sl_iostream_putchar(handle->iostream_handle, ' ');
        sl_iostream_putchar(handle->iostream_handle, '\b');
      } else if (session->buffer_ix == SL_CLI_SIMPLE_PASSWORD_LENGTH_MAX) {
        // Buffer overflow; restart setup
        write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_ENTRY_TOO_LONG);
        read = false;
        switch (s_state) {
          case WAITING_NEW_PASSWORD:
          case WAITING_NEW_PASSWORD_CONFIRMATION:
            CORE_ATOMIC_SECTION(session->state = INIT; );
            if (cli_init == handle) {
              CORE_ATOMIC_SECTION(instance_initialize_password = NULL; );
            }
            break;
          case WAITING_PASSWORD:
            CORE_ATOMIC_SECTION(session->state = ASK_PASSWORD; );
            break;
          default:
            EFM_ASSERT(false);
        }
        return SL_STATUS_WOULD_OVERFLOW;
      } else if (isgraph(c)) {  // Only allow printable char and no space
        // Store char
        session->buffer[session->buffer_ix] = c;
        session->buffer_ix++;
        if (SL_CLI_SIMPLE_PASSWORD_ECHO_PASSWORD) {
          sl_iostream_putchar(handle->iostream_handle, c);
        } else {
          sl_iostream_putchar(handle->iostream_handle, '*');
        }
      }
    } else {
      EFM_ASSERT(false);
    }
  } while (read);

  return SL_STATUS_FAIL;
}

/***************************************************************************//**
 * @brief  timer callback
 ******************************************************************************/
static void on_lock_timeout(sl_sleeptimer_timer_handle_t *handle,
                            void *arg)
{
  Ecode_t ecode;
  (void)handle;
  cli_instance_data_session_t *session = (cli_instance_data_session_t *)arg;
  CORE_ATOMIC_SECTION(session->locked = false; \
                      session->wrong_password_cnt = 0; );

  lock_timer_running = false;
  retry_count = 0;
  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_LOCK_TIMER, &lock_timer_running, sizeof(lock_timer_running));
  nvm3_writeCounter(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_RETRY_COUNTER, retry_count);
  // Don't need to notify, by default we will wake up.

  EFM_ASSERT(ecode == ECODE_NVM3_OK);
}

/***************************************************************************//**
 * @brief  Configure timer
 ******************************************************************************/
static void configure_session_lock_timer(sl_cli_handle_t handle,
                                         cli_instance_data_session_t * session)
{
  Ecode_t ecode;
  uint32_t timeout = SL_CLI_SIMPLE_PASSWORD_LOCK_OUT_TIMEOUT_SEC * 1000;

  CORE_ATOMIC_SECTION(session->locked = true; );
  security_warning = true;
  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_WARN_FLAG, &security_warning, sizeof(security_warning));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_LOCK_OUT_MESSAGE);

  EFM_ASSERT(sl_sleeptimer_start_timer_ms(&session->timer, timeout, on_lock_timeout, session, 0, 0) == SL_STATUS_OK);

  lock_timer_running = true;

  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_LOCK_TIMER, &lock_timer_running, sizeof(lock_timer_running));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);
}

/***************************************************************************//**
 * @brief  Destroy key; Force new setup
 ******************************************************************************/
sl_status_t sl_cli_simple_password_destroy_key(void)
{
  psa_crypto_init();
  psa_destroy_key(CLI_SECURITY_PSA_CRYPTO_KEY_ID);
  CORE_ATOMIC_SECTION(module_state = CONFIGURE; );
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief  Reset Security Flag Command
 ******************************************************************************/
sl_status_t sl_cli_reset_security_warning_flag(void)
{
  Ecode_t ecode;
  bool warning = false;
  CORE_ATOMIC_SECTION(security_warning = false; );
  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_WARN_FLAG, &warning, sizeof(security_warning));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief  Set new password
 ******************************************************************************/
sl_status_t sl_cli_set_simple_password(char *new_password)
{
  uint8_t hash[BLOCK_SIZE];
  size_t hash_len;
  Ecode_t ecode;
  psa_hash_operation_t operation;
  psa_status_t status;

  if (strlen(new_password) < SL_CLI_SIMPLE_PASSWORD_LENGTH_MIN) {
    return SL_STATUS_INVALID_SIGNATURE;
  }

  operation = psa_hash_operation_init();

  status = psa_hash_setup(&operation, PSA_ALG_SHA_256);
  EFM_ASSERT(status == PSA_SUCCESS);

  status = psa_hash_update(&operation, (uint8_t *)new_password, strlen(new_password));
  EFM_ASSERT(status == PSA_SUCCESS);

  status = psa_hash_finish(&operation, hash, sizeof(hash), &hash_len);
  EFM_ASSERT(status == PSA_SUCCESS);

  psa_hash_abort(&operation);

  EFM_ASSERT(status == PSA_SUCCESS);
  EFM_ASSERT(hash_len == sizeof(encrypted_password_hash));

  cipher_encrypt(iv, sizeof(iv),
                 hash, sizeof(hash),
                 encrypted_password_hash, sizeof(encrypted_password_hash), &hash_len);
  mbedtls_platform_zeroize(hash, sizeof(hash));

  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_ENCRYPTED_PASSWORD_HASH, encrypted_password_hash, sizeof(encrypted_password_hash));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_IV, iv, sizeof(iv));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  CORE_ATOMIC_SECTION(module_state = CONFIGURED; );

  return (SL_STATUS_OK);
}

/***************************************************************************//**
 * @brief  Initialize module
 *
 * @note  Must be called prior sl_cli_instance_init()
 ******************************************************************************/
sl_status_t sl_cli_simple_password_init(void)
{
  psa_key_id_t key_id;
  Ecode_t ecode;
  sl_status_t status;
  psa_status_t psa_status = psa_crypto_init();

  EFM_ASSERT(psa_status == PSA_SUCCESS);

  if (psa_is_key_present_in_storage(CLI_SECURITY_PSA_CRYPTO_KEY_ID)) {
    uint32_t obj_type;
    size_t obj_len;
    ecode = nvm3_getObjectInfo(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_ENCRYPTED_PASSWORD_HASH, &obj_type, &obj_len);
    EFM_ASSERT(obj_len <= SL_CLI_SIMPLE_PASSWORD_LENGTH_MAX);
    if (ecode == ECODE_NVM3_OK) {
      uint32_t count;
      ecode = nvm3_readData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_ENCRYPTED_PASSWORD_HASH, encrypted_password_hash, obj_len);
      EFM_ASSERT(ecode == ECODE_NVM3_OK);

      ecode = nvm3_readData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_IV, iv, sizeof(iv));
      EFM_ASSERT(ecode == ECODE_NVM3_OK);

      ecode = nvm3_readData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_WARN_FLAG, &security_warning, sizeof(security_warning));
      EFM_ASSERT(ecode == ECODE_NVM3_OK);

      ecode = nvm3_readData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_LOCK_TIMER, &lock_timer_running, sizeof(lock_timer_running));
      EFM_ASSERT(ecode == ECODE_NVM3_OK);

      ecode = nvm3_readCounter(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_RETRY_COUNTER, &count);
      EFM_ASSERT(ecode == ECODE_NVM3_OK);

      retry_count = (uint8_t)count;

      CORE_ATOMIC_SECTION(module_state = CONFIGURED; );

      return SL_STATUS_OK;
    } else {
      sl_cli_simple_password_destroy_key();
    }
  }

  status = generate_encryption_key(&key_id);
  EFM_ASSERT(status == SL_STATUS_OK);
  EFM_ASSERT(key_id == CLI_SECURITY_PSA_CRYPTO_KEY_ID);
  CORE_ATOMIC_SECTION(module_state = CONFIGURE; );

  // Set warning flag to false at first startup
  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_WARN_FLAG, &security_warning, sizeof(security_warning));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  // Set warning flag to false at first startup
  ecode = nvm3_writeData(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_LOCK_TIMER, &lock_timer_running, sizeof(lock_timer_running));
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  nvm3_writeCounter(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_RETRY_COUNTER, retry_count);
  EFM_ASSERT(ecode == ECODE_NVM3_OK);

  CORE_ATOMIC_SECTION(module_state = CONFIGURE; );

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief  Initialize CLI instance's session
 *
 *@note  This function is called for every CLI instance started.
 ******************************************************************************/
sl_status_t sli_cli_session_init(sl_cli_handle_t handle)
{
  cli_instance_data_session_t *session;
  state_t state;

  session = malloc(sizeof(cli_instance_data_session_t));
  EFM_ASSERT(session != NULL);

  sl_iostream_putchar(handle->iostream_handle, '\n');
  write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_WELCOME_MESSAGE);

  mbedtls_platform_zeroize(session, sizeof(*session));
  CORE_ATOMIC_SECTION(state = module_state; );
  switch (state) {
    case CONFIGURE:
    case CONFIGURING:   //Possible case with an RTOS
      CORE_ATOMIC_SECTION(session->state = INIT; );
      break;

    case CONFIGURED:
      CORE_ATOMIC_SECTION(session->state = ASK_PASSWORD; );
      if (lock_timer_running) {
        configure_session_lock_timer(handle, session);
      }
      break;

    default:
      EFM_ASSERT(false);
  }
  session->wrong_password_cnt = retry_count;
  handle->session_data = session;
  handle->req_prompt = false;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief  Session handler
 *
 * @note  This function return if the accssing the cli command is allowed or not.
 *
 * @note  This function can write or read from the CLI stream for asking session
 *        information in order to log the user.
 ******************************************************************************/
sl_status_t sli_cli_session_handler(sl_cli_handle_t handle)
{
  sl_cli_handle_t cli_init;
  state_t m_state;
  state_t s_state;
  bool locked;
  bool loop;
  cli_instance_data_session_t * session = (cli_instance_data_session_t *)handle->session_data;

#if !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->block_sleep = false;
#endif
  CORE_ATOMIC_SECTION(m_state = module_state;                  \
                      s_state = session->state;                \
                      cli_init = instance_initialize_password; \
                      locked = session->locked; );
  if (locked) {
    return SL_STATUS_PERMISSION;
  }

  if (s_state == LOGGED_IN) {
    return SL_STATUS_OK;
  } else if (m_state == (CONFIGURE | CONFIGURING)) {
    // If the key is destroyed at runtime. All users logged in are still allowed accessing the cli until it logout
    // First instance to log will configure the new password.
    if (m_state == CONFIGURING && cli_init != handle) {
      // Another instance started to configure the password. Do not allow more than 1
      // instance to configure an initial password
      return SL_STATUS_PERMISSION;
    } else if (m_state == CONFIGURED && s_state == WAITING_NEW_PASSWORD) {
      // Other instance has completed to configure the initial password. We cannot ask for
      // the password in order to get logged in.
      write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_CONFIGURED_BY_OTHER_INSTANCE);
      CORE_ATOMIC_SECTION(session->state = ASK_PASSWORD; );
      s_state = ASK_PASSWORD;
    }
  }

  do {
    bool print_prompt = false;
    bool read = false;

    loop = false;

    // Output message depending on the session states
    switch (s_state) {
      case INIT:
      case ASK_NEW_PASSWORD:
        // First access: Let ask an initial password
        sl_iostream_putchar(handle->iostream_handle, '\n');
        write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_ASK_PASSWORD_FIRST_START_MESSAGE);
        print_prompt = true;
        read = true;
        s_state = WAITING_NEW_PASSWORD;
        break;

      case ASK_NEW_PASSWORD_CONFIRMATION:
        // Store password provided and ask for a confirmation before setting it
        write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIRMATION_MESSAGE);
        print_prompt = true;
        read = true;
        s_state = WAITING_NEW_PASSWORD_CONFIRMATION;
        break;

      case ASK_PASSWORD:
        // Not first access; Password already exist. Ask for it
        sl_iostream_putchar(handle->iostream_handle, '\n');
        if (security_warning) {
          write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_SECURITY_WARNING_FLAG);
        }
        write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_ASK_PASSWORD);
        print_prompt = true;
        read = true;
        s_state = WAITING_PASSWORD;
        break;

      case WAITING_NEW_PASSWORD_CONFIRMATION:
      case WAITING_NEW_PASSWORD:
      case WAITING_PASSWORD:
        // Still waiting to receive bytes from the stream.
        read = true;
        break;

      default:
        EFM_ASSERT(false);
        break;
    }

    if (print_prompt) {
      // When asking for information print a prompt
      sl_iostream_putchar(handle->iostream_handle, '\n');
      session->buffer_ix = 0;
    }

    CORE_ATOMIC_SECTION(session->state = s_state; );

    if (read) {
      // if user must provide information in order to configure or login
      sl_status_t status = read_until_line_feed(handle, session);
      if (status != SL_STATUS_OK) {
        return status;
      }
    }

    CORE_ATOMIC_SECTION(s_state = session->state; );

    switch (s_state) {
      case WAITING_NEW_PASSWORD:
        // Store first password received for validation with confirmation
        if (strlen(session->buffer) < SL_CLI_SIMPLE_PASSWORD_LENGTH_MIN) {
          write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_PASSWORD_TOO_SHORT);
          CORE_ATOMIC_SECTION(session->state = INIT; \
                              module_state = CONFIGURE; );
          return SL_STATUS_PERMISSION;
        }
        strcpy(session->tmp_password, session->buffer);
        s_state = ASK_NEW_PASSWORD_CONFIRMATION;
        CORE_ATOMIC_SECTION(session->state = ASK_NEW_PASSWORD_CONFIRMATION; );
        loop = true;
        break;

      case WAITING_NEW_PASSWORD_CONFIRMATION:
        // Validate the same password has been provide 2 times.
        if (strcmp(session->tmp_password, session->buffer) == 0) {
          // Password match, apply it
          sl_status_t status = sl_cli_set_simple_password(session->tmp_password);
          // Clear read buffer (Do not expose password)
          mbedtls_platform_zeroize(session->buffer, sizeof(session->buffer));
          switch (status) {
            case SL_STATUS_OK:
              write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIGURED);
              break;
            case SL_STATUS_INVALID_SIGNATURE:
              write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_PASSWORD_TOO_SHORT);

              CORE_ATOMIC_SECTION(session->state = INIT; \
                                  module_state = CONFIGURE; );
              loop = false;
              return SL_STATUS_PERMISSION;
            default:
              EFM_ASSERT(false);
          }

          CORE_ATOMIC_SECTION(session->state = ASK_PASSWORD; \
                              module_state = CONFIGURED; );
#if !defined(SL_CATALOG_KERNEL_PRESENT)
          handle->block_sleep = true;   // Force cli to run
#endif
          return SL_STATUS_PERMISSION;  // Ask password to get logged in
        } else {
          // passwords do not match
          write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_CONFIRMATION_DO_NOT_MATCH);
          s_state = INIT;
          CORE_ATOMIC_SECTION(module_state = CONFIGURE; );
          loop = false;
        }
        break;

      case WAITING_PASSWORD:
        // Validate password provided to get logged in.
        if (is_valid_password(session->buffer)) {
          CORE_ATOMIC_SECTION(session->state = LOGGED_IN;      \
                              session->wrong_password_cnt = 0; \
                              retry_count = 0; );
          nvm3_writeCounter(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_RETRY_COUNTER, retry_count);
          write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_LOGGED_IN);
          handle->req_prompt = true;
          return SL_STATUS_OK;
        } else {
          // Wrong password
          write_string_lf(handle, SL_CLI_SIMPLE_PASSWORD_INVALID_PASSWORD);
          CORE_ATOMIC_SECTION(session->state = ASK_PASSWORD; );
          loop = true;

          CORE_ATOMIC_SECTION(session->wrong_password_cnt++; );
          if (retry_count < SL_CLI_SIMPLE_PASSWORD_MAX_RETRY) {
            retry_count++;
            nvm3_writeCounter(nvm3_defaultHandle, CLI_SECURITY_NVM3_KEY_ID_RETRY_COUNTER, retry_count);
          }

          if (session->wrong_password_cnt >= SL_CLI_SIMPLE_PASSWORD_MAX_RETRY) {
            configure_session_lock_timer(handle, session);
            return SL_STATUS_PERMISSION;
          } else {
            sl_iostream_putchar(handle->iostream_handle, '\n');
          }
        }
        // Clear read buffer(Do not expose password)
        mbedtls_platform_zeroize(session->buffer, sizeof(session->buffer));
        break;

      default:
        EFM_ASSERT(false);
        break;
    }

    CORE_ATOMIC_SECTION(s_state = session->state; );
  } while (loop);

  return SL_STATUS_FAIL;
}

/***************************************************************************//**
 * @brief  Logout
 ******************************************************************************/
sl_status_t sl_cli_simple_password_logout(sl_cli_handle_t handle)
{
  state_t s_state;
  state_t m_state;
  cli_instance_data_session_t * session = (cli_instance_data_session_t *)handle->session_data;
  CORE_ATOMIC_SECTION(s_state = session->state; \
                      m_state = module_state; );
  if (s_state == LOGGED_IN) {
    if (m_state == CONFIGURED) {
      CORE_ATOMIC_SECTION(session->state = ASK_PASSWORD; );
    } else {
      CORE_ATOMIC_SECTION(session->state = INIT; );
    }
  }

  handle->req_prompt = false;
#if !defined(SL_CATALOG_KERNEL_PRESENT)
  handle->block_sleep = true;   // Force cli to run before going to sleep
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief  Log out command
 ******************************************************************************/
void cli_logout_cmd(sl_cli_command_arg_t * arguments)
{
  (void)arguments;
  sl_cli_simple_password_logout((sl_cli_handle_t)arguments->handle);
}

/***************************************************************************//**
 * @brief  Set new password command
 ******************************************************************************/
void cli_set_password_cmd(sl_cli_command_arg_t * arguments)
{
  char *password = sl_cli_get_argument_string(arguments, 0);
  char *new_password_1 = sl_cli_get_argument_string(arguments, 1);
  char *new_password_2 = sl_cli_get_argument_string(arguments, 2);
  sl_status_t status;

  if (strcmp(new_password_1, new_password_2) != 0) {
    write_string_lf((sl_cli_handle_t)arguments->handle, SL_CLI_SIMPLE_PASSWORD_CONFIRMATION_DO_NOT_MATCH);
    return;
  }

  if (psa_is_key_present_in_storage(CLI_SECURITY_PSA_CRYPTO_KEY_ID)) {
    if (!is_valid_password(password)) {
      write_string_lf((sl_cli_handle_t)arguments->handle, SL_CLI_SIMPLE_PASSWORD_INVALID_PASSWORD);
      return;
    }
  }

  status = sl_cli_set_simple_password(new_password_1);
  switch (status) {
    case SL_STATUS_OK:
      write_string_lf(arguments->handle, SL_CLI_SIMPLE_PASSWORD_NEW_PASSWORD_CONFIGURED);
      break;
    case SL_STATUS_INVALID_SIGNATURE:
      write_string_lf(arguments->handle, SL_CLI_SIMPLE_PASSWORD_PASSWORD_TOO_SHORT);
      break;
    default:
      EFM_ASSERT(false);
  }
}

/***************************************************************************//**
 * @brief  Reset Security Flag Command
 ******************************************************************************/
void cli_reset_security_warning_flag_cmd(sl_cli_command_arg_t * arguments)
{
  (void)arguments;

  EFM_ASSERT(sl_cli_reset_security_warning_flag() == SL_STATUS_OK);
}

/***************************************************************************//**
 * @brief  Destroy key command
 ******************************************************************************/
void cli_destroy_key_cmd(sl_cli_command_arg_t * arguments)
{
  (void)arguments;
  EFM_ASSERT(sl_cli_simple_password_destroy_key() == SL_STATUS_OK);
}
