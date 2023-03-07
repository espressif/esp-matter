/***************************************************************************//**
 * @file
 * @brief Common - Authentication
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/auth.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                           (COMMON, AUTH)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_COMMON

#define  AUTH_HANDLE_IS_NULL(user_handle)    (((user_handle).UserObjPtr == Auth_UserHandleNull.UserObjPtr) ? DEF_YES : DEF_NO)

#define  AUTH_INIT_CFG_DFLT                     { \
    .RootUserCfg =                                \
    {                                             \
      .RootUserName = "admin",                    \
      .RootUserPwd = "admin"                      \
    },                                            \
    .ResourceCfg =                                \
    {                                             \
      .NbUserMax = 4u,                            \
      .NameLenMax = 10u,                          \
      .PwdLenMax = 10u                            \
    },                                            \
    .MemSegPtr = DEF_NULL                         \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct auth_user_credentials {                          // --------------- AUTH USER CREDENTIALS --------------
  AUTH_USER User;                                               // User structure.
  CPU_CHAR  *Pwd;                                               // Password for this user.
} AUTH_USER_CREDENTIALS;

typedef struct auth_data {                                      // ----------- INTERNAL AUTH DATA STRUCTURE -----------
  CPU_SIZE_T            UserNbr;                                // Current nbr of users.
  KAL_LOCK_HANDLE       LockHandle;                             // Handle to lock.
  AUTH_USER_CREDENTIALS *UsersCredentials;                      // Ptr to tbl of user credentials.
} AUTH_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

AUTH_USER_HANDLE Auth_RootUserHandle = { DEF_NULL };

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static AUTH_DATA *Auth_Ptr = DEF_NULL;

static AUTH_USER_HANDLE Auth_UserHandleNull = { DEF_NULL };

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const AUTH_INIT_CFG  Auth_InitCfgDflt = AUTH_INIT_CFG_DFLT;
static AUTH_INIT_CFG Auth_InitCfg = AUTH_INIT_CFG_DFLT;
#else
extern const AUTH_INIT_CFG Auth_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static AUTH_USER *Auth_GetUserHandler(const CPU_CHAR *p_name,
                                      RTOS_ERR       *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Auth_ConfigureRootUser()
 *
 * @brief    Configure the properties of the root user, in the Auth sub-module.
 *
 * @param    p_cfg   Pointer to the structure containing the new root user parameters.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Auth_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Auth_ConfigureRootUser(AUTH_CFG_ROOT_USER *p_cfg)
{
  RTOS_ASSERT_DBG((Auth_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  RTOS_ASSERT_DBG((p_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  Auth_InitCfg.RootUserCfg = *p_cfg;
}
#endif

/****************************************************************************************************//**
 *                                           Auth_ConfigureResource()
 *
 * @brief    Configure the properties of the resources used by the Auth sub-module.
 *
 * @param    p_cfg   Pointer to the structure containing the new resource usage parameters.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Auth_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Auth_ConfigureResource(AUTH_CFG_RESOURCE *p_cfg)
{
  RTOS_ASSERT_DBG((Auth_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  RTOS_ASSERT_DBG((p_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  Auth_InitCfg.ResourceCfg = *p_cfg;
}
#endif

/****************************************************************************************************//**
 *                                           Auth_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by Auth
 *           instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Auth_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Auth_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((Auth_Ptr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  Auth_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                               Auth_Init()
 *
 * @brief    Initializes authentication module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @note     (1) The functions Auth_Configure...() can be used to configure more specific properties
 *               of the Auth sub-module, when RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN is set to
 *               DEF_DISABLED. If set to DEF_ENABLED, the structure Auth_InitCfg needs to be declared
 *               and filled by the application to configure these specific properties for the module.
 *******************************************************************************************************/
void Auth_Init(RTOS_ERR *p_err)
{
  CPU_SIZE_T root_name_len;
  CPU_SIZE_T root_pwd_len;
  CPU_INT08U i;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((Auth_InitCfg.ResourceCfg.NbUserMax != 1u), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((Auth_InitCfg.ResourceCfg.NbUserMax != DEF_INT_08U_MAX_VAL), *p_err, RTOS_ERR_INVALID_ARG,; );

  root_name_len = Str_Len_N(Auth_InitCfg.RootUserCfg.RootUserName, Auth_InitCfg.ResourceCfg.NameLenMax + 1u);
  root_pwd_len = Str_Len_N(Auth_InitCfg.RootUserCfg.RootUserPwd, Auth_InitCfg.ResourceCfg.PwdLenMax  + 1u);

  RTOS_ASSERT_DBG_ERR_SET((root_name_len <= Auth_InitCfg.ResourceCfg.NameLenMax), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((root_pwd_len <= Auth_InitCfg.ResourceCfg.PwdLenMax), *p_err, RTOS_ERR_INVALID_ARG,; );

  Auth_Ptr = (AUTH_DATA *)Mem_SegAlloc("Auth - Data",           // Allocate data needed by Auth.
                                       Auth_InitCfg.MemSegPtr,
                                       sizeof(AUTH_DATA),
                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Auth_Ptr->UserNbr = 1;
  //                                                               Allocate data needed by Auth user cred tbl.
  Auth_Ptr->UsersCredentials = (AUTH_USER_CREDENTIALS *)Mem_SegAlloc("Auth - User Cred Tbl",
                                                                     Auth_InitCfg.MemSegPtr,
                                                                     sizeof(AUTH_USER_CREDENTIALS) * Auth_InitCfg.ResourceCfg.NbUserMax,
                                                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  for (i = 0u; i < Auth_InitCfg.ResourceCfg.NbUserMax; ++i) {
    Auth_Ptr->UsersCredentials[i].User.Name = (CPU_CHAR *)Mem_SegAlloc("Auth - User Cred Tbl Names",
                                                                       Auth_InitCfg.MemSegPtr,
                                                                       sizeof(CPU_CHAR) * Auth_InitCfg.ResourceCfg.NameLenMax,
                                                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    Auth_Ptr->UsersCredentials[i].Pwd = (CPU_CHAR *)Mem_SegAlloc("Auth - User Cred Tbl Pwds",
                                                                 Auth_InitCfg.MemSegPtr,
                                                                 sizeof(CPU_CHAR) * Auth_InitCfg.ResourceCfg.PwdLenMax,
                                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  Str_Copy_N(Auth_Ptr->UsersCredentials[0u].User.Name, Auth_InitCfg.RootUserCfg.RootUserName, root_name_len);
  Str_Copy_N(Auth_Ptr->UsersCredentials[0u].Pwd, Auth_InitCfg.RootUserCfg.RootUserPwd, root_pwd_len);
  Auth_Ptr->UsersCredentials[0u].User.Rights = AUTH_RIGHT_ROOT;

  Auth_RootUserHandle.UserObjPtr = Auth_Ptr->UsersCredentials[0u].User.Name;

  Auth_Ptr->LockHandle = KAL_LockCreate("Auth Lock",
                                        KAL_OPT_CREATE_NONE,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_Init: failed with err:", RTOS_ERR_LOG_ARG_GET(*p_err)));
  } else {
    LOG_VRB(("Auth_Init: call completed."));
  }

  return;
}

/****************************************************************************************************//**
 *                                               Auth_CreateUser()
 *
 * @brief    Creates a user profile and fills the properties of the user profile structure provided.
 *
 * @param    p_name  Pointer to the user name string.
 *
 * @param    p_pwd   Pointer to the password string.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_NO_MORE_RSRC
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   Handle of the created user profile.
 *******************************************************************************************************/
AUTH_USER_HANDLE Auth_CreateUser(const CPU_CHAR *p_name,
                                 const CPU_CHAR *p_pwd,
                                 RTOS_ERR       *p_err)
{
  AUTH_USER_CREDENTIALS *p_user_cred;
  AUTH_USER_HANDLE      handle = Auth_UserHandleNull;
  CPU_SIZE_T            name_len;
  CPU_SIZE_T            pwd_len;
  CPU_SIZE_T            i;
  CPU_INT16S            cmp_result;
  RTOS_ERR              local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  name_len = Str_Len_N(p_name, Auth_InitCfg.ResourceCfg.NameLenMax + 1);
  pwd_len = Str_Len_N(p_pwd, Auth_InitCfg.ResourceCfg.PwdLenMax  + 1);

  RTOS_ASSERT_DBG_ERR_SET((name_len <= Auth_InitCfg.ResourceCfg.NameLenMax), *p_err, RTOS_ERR_INVALID_ARG, handle);
  RTOS_ASSERT_DBG_ERR_SET((pwd_len <= Auth_InitCfg.ResourceCfg.PwdLenMax), *p_err, RTOS_ERR_INVALID_ARG, handle);

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_CreateUser: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  if (Auth_Ptr->UserNbr >= Auth_InitCfg.ResourceCfg.NbUserMax) {
    RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NO_MORE_RSRC, ("Auth_CreateUser: too many existing users."));
    goto exit_release;
  }

  for (i = 0; i < Auth_Ptr->UserNbr; ++i) {
    cmp_result = Str_Cmp(Auth_Ptr->UsersCredentials[i].User.Name, p_name);
    if (cmp_result == 0) {
      RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_ALREADY_EXISTS, ("Auth_CreateUser: user already exists."));
      goto exit_release;
    }
  }

  p_user_cred = &Auth_Ptr->UsersCredentials[Auth_Ptr->UserNbr];
  Auth_Ptr->UserNbr++;

  name_len = DEF_MIN(name_len + 1, Auth_InitCfg.ResourceCfg.NameLenMax);
  pwd_len = DEF_MIN(pwd_len + 1, Auth_InitCfg.ResourceCfg.PwdLenMax);

  (void)Str_Copy_N(p_user_cred->User.Name,
                   p_name,
                   name_len);

  (void)Str_Copy_N(p_user_cred->Pwd,
                   p_pwd,
                   pwd_len);

  p_user_cred->User.Rights = AUTH_RIGHT_NONE;

  handle.UserObjPtr = (void *)p_user_cred->User.Name;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit_release:
  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

exit:

  return (handle);
}

/****************************************************************************************************//**
 *                                           Auth_ChangePassword()
 *
 * @brief    Changes the user's password.
 *
 * @param    p_user      Pointer to user object.
 *
 * @param    p_pwd       Pointer to the new password.
 *
 * @param    p_as_user   Pointer to the user profile with the permission level to perform the action.
 *                       Must be the same as permission level p_user or the ROOT user.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_PERMISSION
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_IS_OWNER
 *                           - RTOS_ERR_WOULD_OVF
 *******************************************************************************************************/

#if 0
void Auth_ChangePassword(AUTH_USER       *p_user,
                         const CPU_CHAR  *p_pwd,
                         const AUTH_USER *p_as_user,
                         RTOS_ERR        *p_err)
{
  CPU_SIZE_T name_len;
  CPU_SIZE_T pwd_len;
  CPU_SIZE_T i;
  CPU_INT16S cmp_result;
  RTOS_ERR   local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  name_len = Str_Len_N(p_user->Name, AUTH_NAME_MAX_LENGTH + 1);
  pwd_len = Str_Len_N(p_pwd, AUTH_PWD_MAX_LENGTH  + 1);

  RTOS_ASSERT_DBG_ERR_SET((pwd_len <= Auth_InitCfg.ResourceCfg.PwdLenMax), *p_err, RTOS_ERR_INVALID_ARG,; );

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if ((Str_Cmp(p_as_user->Name, p_user->Name) != 0)
      && (DEF_BIT_IS_CLR(p_as_user->Rights, AUTH_RIGHT_ROOT) == DEF_YES)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_PERMISSION);
    goto exit_release;
  }

  for (i = 0; i < Auth_Ptr->UserNbr; ++i) {
    cmp_result = Str_Cmp_N(Auth_UsersCredentials[i].User.Name, p_user->Name, name_len);
    if (cmp_result == 0) {
      (void)Str_Copy_N(Auth_UsersCredentials[i].Pwd,
                       p_pwd,
                       pwd_len);

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit_release;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);

exit_release:
  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  PP_UNUSED_PARAM(local_err);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                               Auth_GetUser()
 *
 * @brief    Gets the specific user profile structure according to the name provided.
 *
 * @param    p_name  Pointer to the user name string to retrieve.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   Handle of the user associated with the name provided.
 *******************************************************************************************************/
AUTH_USER_HANDLE Auth_GetUser(const CPU_CHAR *p_name,
                              RTOS_ERR       *p_err)
{
  AUTH_USER_HANDLE handle = Auth_UserHandleNull;
  AUTH_USER        *p_user = DEF_NULL;
  RTOS_ERR         local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_GetUser: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  p_user = Auth_GetUserHandler(p_name, p_err);

  if (p_user != DEF_NULL) {
    handle.UserObjPtr = (void *)p_user->Name;
  }

  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

exit:

  return (handle);
}

/****************************************************************************************************//**
 *                                           Auth_GetUserRight()
 *
 * @brief    Gets the rights of the user associated with the specified user handle.
 *
 * @param    user_handle     User handle of the user profile from which to retrieve the rights.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   User's rights,   if no errors.
 *           AUTH_RIGHT_NONE, otherwise.
 *******************************************************************************************************/
AUTH_RIGHT Auth_GetUserRight(AUTH_USER_HANDLE user_handle,
                             RTOS_ERR         *p_err)
{
  AUTH_USER  *p_user = DEF_NULL;
  AUTH_RIGHT rights = AUTH_RIGHT_NONE;
  RTOS_ERR   local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, AUTH_RIGHT_NONE);

  RTOS_ASSERT_DBG_ERR_SET((AUTH_HANDLE_IS_NULL(user_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR, AUTH_RIGHT_NONE);

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_GetUser: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  p_user = Auth_GetUserHandler((const  CPU_CHAR *)user_handle.UserObjPtr, p_err);

  if (p_user != DEF_NULL) {
    rights = p_user->Rights;
  }

  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, rights);

exit:

  return (rights);
}

/****************************************************************************************************//**
 *                                       Auth_ValidateCredentials()
 *
 * @brief    Validates the user and password tuple with known users.
 *
 * @param    p_name  Pointer to the user name string.
 *
 * @param    p_pwd   Pointer to the password string.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_INVALID_CREDENTIALS
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @return   Handle of the user associated with the provided name and password, if no errors.
 *******************************************************************************************************/
AUTH_USER_HANDLE Auth_ValidateCredentials(const CPU_CHAR *p_name,
                                          const CPU_CHAR *p_pwd,
                                          RTOS_ERR       *p_err)
{
  AUTH_USER_HANDLE handle = Auth_UserHandleNull;
  CPU_INT08U       i;
  CPU_INT16S       cmp_result;
  RTOS_ERR         local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  if (Str_Len(p_name) > Auth_InitCfg.ResourceCfg.NameLenMax
      || Str_Len(p_pwd) > Auth_InitCfg.ResourceCfg.PwdLenMax) {
    RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_INVALID_CREDENTIALS, ("Auth_ValidateCredentials: invalid credentials."));
    goto exit;
  }

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_ValidateCredentials: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  for (i = 0; i < Auth_Ptr->UserNbr; ++i) {
    cmp_result = Str_Cmp(Auth_Ptr->UsersCredentials[i].User.Name, p_name);
    if (cmp_result == 0) {
      cmp_result = Str_Cmp(Auth_Ptr->UsersCredentials[i].Pwd, p_pwd);
      if (cmp_result == 0) {
        handle.UserObjPtr = (void *)Auth_Ptr->UsersCredentials[i].User.Name;

        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        goto exit_release;
      }
      break;
    }
  }

  RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_INVALID_CREDENTIALS, ("Auth_ValidateCredentials: invalid credentials."));

exit_release:
  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, Auth_UserHandleNull);

exit:

  return (handle);
}

/****************************************************************************************************//**
 *                                               Auth_GrantRight()
 *
 * @brief    Grants a specific right to a user as another user (limits the rights granted).
 *
 * @param    right           The new right to grant.
 *
 * @param    user_handle     User handle of the user that will receive the new right.
 *
 * @param    as_user_handle  User handle of the user with the permission level to perform this task.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_PERMISSION
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void Auth_GrantRight(AUTH_RIGHT       right,
                     AUTH_USER_HANDLE user_handle,
                     AUTH_USER_HANDLE as_user_handle,
                     RTOS_ERR         *p_err)
{
  AUTH_USER  *p_user;
  AUTH_USER  *p_as_user;
  CPU_SIZE_T name_len;
  CPU_INT08U i;
  CPU_INT16S cmp_result;
  RTOS_ERR   local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((AUTH_HANDLE_IS_NULL(user_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((AUTH_HANDLE_IS_NULL(as_user_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_GrantRight: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  p_user = Auth_GetUserHandler((const  CPU_CHAR *)user_handle.UserObjPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_as_user = Auth_GetUserHandler((const  CPU_CHAR *)as_user_handle.UserObjPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (((DEF_BIT_IS_SET(p_as_user->Rights, AUTH_RIGHT_MNG) == DEF_NO)
       || (DEF_BIT_IS_SET(p_as_user->Rights, right) == DEF_NO))
      && (DEF_BIT_IS_SET(p_as_user->Rights, AUTH_RIGHT_ROOT) == DEF_NO)) {
    RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_PERMISSION, ("Auth_GrantRight: user does not have permission."));
    goto exit_release;
  }

  name_len = Str_Len_N(p_user->Name, Auth_InitCfg.ResourceCfg.NameLenMax + 1);

  for (i = 0; i < Auth_Ptr->UserNbr; ++i) {
    cmp_result = Str_Cmp_N(Auth_Ptr->UsersCredentials[i].User.Name, p_user->Name, name_len);
    if (cmp_result == 0) {
      DEF_BIT_SET(Auth_Ptr->UsersCredentials[i].User.Rights, right);

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit_release;
    }
  }

  RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NOT_FOUND, ("Auth_GrantRight: user not found."));

exit_release:
  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit:

  return;
}

/****************************************************************************************************//**
 *                                           Auth_RevokeRight()
 *
 * @brief    Revokes a specific right of a specified user.
 *
 * @param    right           Right to revoke.
 *
 * @param    user_handle     User handle of the user that will have right revoked.
 *
 * @param    as_user_handle  User handle of the user with the permission level to perform this task.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_PERMISSION
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void Auth_RevokeRight(AUTH_RIGHT       right,
                      AUTH_USER_HANDLE user_handle,
                      AUTH_USER_HANDLE as_user_handle,
                      RTOS_ERR         *p_err)
{
  AUTH_USER  *p_user;
  AUTH_USER  *p_as_user;
  CPU_SIZE_T name_len;
  CPU_INT08U i;
  CPU_INT16S cmp_result;
  RTOS_ERR   local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((AUTH_HANDLE_IS_NULL(user_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((AUTH_HANDLE_IS_NULL(as_user_handle) == DEF_NO), *p_err, RTOS_ERR_NULL_PTR,; );

  KAL_LockAcquire(Auth_Ptr->LockHandle, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Auth_RevokeRight: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  p_user = Auth_GetUserHandler((const  CPU_CHAR *)user_handle.UserObjPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_as_user = Auth_GetUserHandler((const  CPU_CHAR *)as_user_handle.UserObjPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  //                                                               This implementation allows the ROOT user ...
  //                                                               ... to revoke it's own ROOT right.
  if (((DEF_BIT_IS_SET(p_as_user->Rights, AUTH_RIGHT_MNG) == DEF_NO)
       || (DEF_BIT_IS_SET(p_as_user->Rights, right) == DEF_NO)   )
      && (DEF_BIT_IS_SET(p_as_user->Rights, AUTH_RIGHT_ROOT) == DEF_NO)    ) {
    RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_PERMISSION, ("Auth_RevokeRight: user does not have permission."));
    goto exit_release;
  }

  name_len = Str_Len_N(p_user->Name, Auth_InitCfg.ResourceCfg.NameLenMax + 1);

  for (i = 0; i < Auth_Ptr->UserNbr; ++i) {
    cmp_result = Str_Cmp_N(Auth_Ptr->UsersCredentials[i].User.Name, p_user->Name, name_len);
    if (cmp_result == 0) {
      DEF_BIT_CLR(Auth_Ptr->UsersCredentials[i].User.Rights, right);

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit_release;
    }
  }

  RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NOT_FOUND, ("Auth_RevokeRight: user not found."));

exit_release:
  KAL_LockRelease(Auth_Ptr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

exit:

  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Auth_GetUserHandler()
 *
 * @brief    Gets the user structure with the given name.
 *
 * @param    p_name  Pointer to the user name string to retrieve.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   Pointer to the user object, if no errors.
 *           DEF_NULL, otherwise.
 *
 * @note     (1) This function must only be called while the authentication lock is acquired.
 *******************************************************************************************************/
static AUTH_USER *Auth_GetUserHandler(const CPU_CHAR *p_name,
                                      RTOS_ERR       *p_err)
{
  AUTH_USER  *p_user = DEF_NULL;
  CPU_SIZE_T name_len;
  CPU_INT08U i;
  CPU_INT16S cmp_result;

  name_len = Str_Len_N(p_name, Auth_InitCfg.ResourceCfg.NameLenMax + 1);

  for (i = 0; i < Auth_Ptr->UserNbr; ++i) {
    cmp_result = Str_Cmp_N(Auth_Ptr->UsersCredentials[i].User.Name, p_name, name_len);
    if (cmp_result == 0) {
      p_user = &Auth_Ptr->UsersCredentials[i].User;

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      goto exit;
    }
  }

  RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NOT_FOUND, ("Auth_GetUserHandler: user not found."));

exit:
  return (p_user);
}
