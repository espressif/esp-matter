/***************************************************************************//**
 * @file
 * @brief HTTP Server Rest Hooks Example
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_str.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/http_server.h>
#include  <net/include/http_server_addon_rest.h>

#include  "ex_http_server_rest_hooks.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                             APP CONFIGURATION DEFINES - CAN BE MODIFIED
 *******************************************************************************************************/

#define  EX_HTTP_SERVER_REST_USER_MAX_NBR                       10u

#define  EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN                  100u
#define  EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN             50u
#define  EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN              50u
#define  EX_HTTP_SERVER_REST_JOB_TITLE_STR_MAX_LEN             100u

#define  EX_HTTP_SERVER_REST_HTTPs_CONN_NBR_MAX                6u

/********************************************************************************************************
 *                                             OTHER DEFINES
 *******************************************************************************************************/

#define  EX_HTTP_SERVER_REST_GENDER_STR_MAX_LEN             7u
#define  EX_HTTP_SERVER_REST_AGE_STR_MAX_LEN                4u

/********************************************************************************************************
 *                                            JSON DEFINES
 *******************************************************************************************************/

#define  EX_HTTP_SERVER_REST_JSON_OBJ_CHARS_NBR                2u
#define  EX_HTTP_SERVER_REST_JSON_TBL_CHARS_NBR                2u
#define  EX_HTTP_SERVER_REST_JSON_FIELD_MIN_CHARS_NBR          6u
#define  EX_HTTP_SERVER_REST_JSON_FIELD_MAX_CHARS_NBR          8u

#define  EX_HTTP_SERVER_REST_JSON_LIST_USER_START_STR          "{\"Users\":["
#define  EX_HTTP_SERVER_REST_JSON_LIST_USER_END_STR            "]}"

#define  EX_HTTP_SERVER_REST_JSON_KEY_ID_STR_NAME              "User ID"
#define  EX_HTTP_SERVER_REST_JSON_KEY_LINK_STR_NAME            "Link"
#define  EX_HTTP_SERVER_REST_JSON_KEY_FIRST_NAME_STR_NAME      "First Name"
#define  EX_HTTP_SERVER_REST_JSON_KEY_LAST_NAME_STR_NAME       "Last Name"
#define  EX_HTTP_SERVER_REST_JSON_KEY_GENDER_STR_NAME          "Gender"
#define  EX_HTTP_SERVER_REST_JSON_KEY_AGE_STR_NAME             "Age"
#define  EX_HTTP_SERVER_REST_JSON_KEY_JOB_STR_NAME             "Job Title"

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       USER INFO FIELD DATA TYPE
 *******************************************************************************************************/

typedef  enum  Ex_HTTP_Server_REST_UserField {
  EX_HTTP_SERVER_REST_USER_FIELD_ID,
  EX_HTTP_SERVER_REST_USER_FIELD_LINK,
  EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME,
  EX_HTTP_SERVER_REST_USER_FIELD_LAST_NAME,
  EX_HTTP_SERVER_REST_USER_FIELD_GENDER,
  EX_HTTP_SERVER_REST_USER_FIELD_AGE,
  EX_HTTP_SERVER_REST_USER_FIELD_JOB
} EX_HTTP_SERVER_REST_USER_FIELD;

/********************************************************************************************************
 *                                           USER DATA TYPE
 *
 * Note(s): (1) The user data type will be use to create a double chain list of users.
 *******************************************************************************************************/

typedef  struct  Ex_HTTP_Server_REST_User EX_HTTP_SERVER_REST_USER;

struct  Ex_HTTP_Server_REST_User {
  CPU_INT32U               ID;
  CPU_CHAR                 Link[EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN];
  CPU_CHAR                 FirstName[EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN];
  CPU_CHAR                 LastName[EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN];
  CPU_CHAR                 Gender[EX_HTTP_SERVER_REST_GENDER_STR_MAX_LEN];
  CPU_INT08U               Age;
  CPU_CHAR                 JobTitle[EX_HTTP_SERVER_REST_JOB_TITLE_STR_MAX_LEN];
  CPU_INT32U               RdRefCtr;
  CPU_INT32U               WrRefCtr;
  EX_HTTP_SERVER_REST_USER *PrevPtr;
  EX_HTTP_SERVER_REST_USER *NextPtr;
};

/********************************************************************************************************
 *                                       FREE USER ID DATA TYPE
 *
 * Note(s): (1) The free user ID data type will be use to create a simple chain list of Free ID.
 *
 *          (2) Free ID are ID that have already been used but are now free because the user was deleted.
 *
 *          (3) This allows to reuse ID from deleted user.
 *******************************************************************************************************/

typedef  struct  Ex_HTTP_Server_REST_FreeUserID EX_HTTP_SERVER_REST_FREE_USER_ID;

struct  Ex_HTTP_Server_REST_FreeUserID {
  CPU_INT32U                       ID;
  EX_HTTP_SERVER_REST_FREE_USER_ID *NextPtr;
};

/********************************************************************************************************
 *                                     APPLICATION DATA DATA TYPE
 *
 * Notes(s): (1) The application data structure is used to store some information relative to one HTTP
 *               transaction that must be accessible through all the calls to the same hook function.
 *
 *               For example, when a request is received to get a specific user infos, in the Init state,
 *               we will try to found that user and store a the user pointer in the app data so that when
 *               we fall in the TX state we can retrive the user pointer.
 *******************************************************************************************************/

typedef  struct  Ex_HTTP_Server_REST_Data EX_HTTP_SERVER_REST_DATA;

struct  Ex_HTTP_Server_REST_Data {
  EX_HTTP_SERVER_REST_USER       *UserPtr;                      // Current user being process by the request.
  EX_HTTP_SERVER_REST_USER_FIELD FieldType;                     // Store the current user info field that must be transmitted.
  EX_HTTP_SERVER_REST_DATA       *NextPtr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_GetPage(const HTTPs_REST_RESOURCE    *p_resource,
                                                         const HTTPs_REST_MATCHED_URI *p_uri,
                                                         const HTTPs_REST_STATE       state,
                                                         void                         **p_data,
                                                         const HTTPs_INSTANCE         *p_instance,
                                                         HTTPs_CONN                   *p_conn,
                                                         void                         *p_buf,
                                                         const CPU_SIZE_T             buf_len,
                                                         CPU_SIZE_T                   *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_GetUserListHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                 const HTTPs_REST_MATCHED_URI *p_uri,
                                                                 const HTTPs_REST_STATE       state,
                                                                 void                         **p_data,
                                                                 const HTTPs_INSTANCE         *p_instance,
                                                                 HTTPs_CONN                   *p_conn,
                                                                 void                         *p_buf,
                                                                 const CPU_SIZE_T             buf_len,
                                                                 CPU_SIZE_T                   *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_GetUserInfoHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                 const HTTPs_REST_MATCHED_URI *p_uri,
                                                                 const HTTPs_REST_STATE       state,
                                                                 void                         **p_data,
                                                                 const HTTPs_INSTANCE         *p_instance,
                                                                 HTTPs_CONN                   *p_conn,
                                                                 void                         *p_buf,
                                                                 const CPU_SIZE_T             buf_len,
                                                                 CPU_SIZE_T                   *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_SetUserInfoHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                 const HTTPs_REST_MATCHED_URI *p_uri,
                                                                 const HTTPs_REST_STATE       state,
                                                                 void                         **p_data,
                                                                 const HTTPs_INSTANCE         *p_instance,
                                                                 HTTPs_CONN                   *p_conn,
                                                                 void                         *p_buf,
                                                                 const CPU_SIZE_T             buf_len,
                                                                 CPU_SIZE_T                   *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_CreateUserHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                const HTTPs_REST_MATCHED_URI *p_uri,
                                                                const HTTPs_REST_STATE       state,
                                                                void                         **p_data,
                                                                const HTTPs_INSTANCE         *p_instance,
                                                                HTTPs_CONN                   *p_conn,
                                                                void                         *p_buf,
                                                                const CPU_SIZE_T             buf_len,
                                                                CPU_SIZE_T                   *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_DeleteUserHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                const HTTPs_REST_MATCHED_URI *p_uri,
                                                                const HTTPs_REST_STATE       state,
                                                                void                         **p_data,
                                                                const HTTPs_INSTANCE         *p_instance,
                                                                HTTPs_CONN                   *p_conn,
                                                                void                         *p_buf,
                                                                const CPU_SIZE_T             buf_len,
                                                                CPU_SIZE_T                   *p_buf_len_used);

static CPU_SIZE_T Ex_HTTP_Server_REST_JSON_GetUserFieldSize(EX_HTTP_SERVER_REST_USER *p_user);

static CPU_SIZE_T Ex_HTTP_Server_REST_JSON_GetUserInfoSize(EX_HTTP_SERVER_REST_USER *p_user);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_WrUserToBuf(EX_HTTP_SERVER_REST_USER *p_user,
                                                                  void                     *p_buf,
                                                                  const CPU_SIZE_T         buf_len,
                                                                  const CPU_SIZE_T         buf_len_max,
                                                                  CPU_SIZE_T               *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_WrUserListToBuf(EX_HTTP_SERVER_REST_DATA *p_app_data,
                                                                      void                     *p_buf,
                                                                      const CPU_SIZE_T         buf_len,
                                                                      const CPU_SIZE_T         buf_len_max,
                                                                      CPU_SIZE_T               *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_WrUserInfoToBuf(EX_HTTP_SERVER_REST_USER *p_user,
                                                                      void                     *p_buf,
                                                                      const CPU_SIZE_T         buf_len,
                                                                      const CPU_SIZE_T         buf_len_max,
                                                                      CPU_SIZE_T               *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_ParseUser(EX_HTTP_SERVER_REST_DATA     *p_app_data,
                                                                EX_HTTP_SERVER_REST_USER     *p_user,
                                                                const HTTPs_REST_MATCHED_URI *p_uri,
                                                                void                         *p_buf,
                                                                const CPU_SIZE_T             buf_len,
                                                                const CPU_SIZE_T             buf_len_max,
                                                                CPU_SIZE_T                   *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_ParseUserInfo(EX_HTTP_SERVER_REST_DATA *p_app_data,
                                                                    EX_HTTP_SERVER_REST_USER *p_user,
                                                                    void                     *p_buf,
                                                                    const CPU_SIZE_T         data_len,
                                                                    const CPU_SIZE_T         buf_len_max,
                                                                    CPU_SIZE_T               *p_buf_len_used);

static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_ParseField(EX_HTTP_SERVER_REST_USER_FIELD field_type,
                                                                 CPU_CHAR                       **p_val_found,
                                                                 CPU_SIZE_T                     *p_val_len,
                                                                 void                           *p_buf,
                                                                 const CPU_SIZE_T               data_len,
                                                                 const CPU_SIZE_T               buf_len_max,
                                                                 CPU_SIZE_T                     *p_buf_len_used);

static EX_HTTP_SERVER_REST_USER *Ex_HTTP_Server_REST_FindUser(CPU_INT08U user_id);

static EX_HTTP_SERVER_REST_USER *Ex_HTTP_Server_REST_GetUser(void);

static void Ex_HTTP_Server_REST_RemoveUser(EX_HTTP_SERVER_REST_USER *p_user);

static CPU_INT08U Ex_HTTP_Server_REST_GetUserID(void);

static EX_HTTP_SERVER_REST_FREE_USER_ID *Ex_HTTP_Server_REST_GetFreeUserID(CPU_INT08U user_id);

static void Ex_HTTP_Server_REST_RemoveFreeUserID(void);

static EX_HTTP_SERVER_REST_DATA *Ex_HTTP_Server_REST_GetDataBlk(void);

static void Ex_HTTP_Server_REST_RemoveDataBlk(EX_HTTP_SERVER_REST_DATA *p_app_data);

static CPU_SIZE_T Ex_HTTP_Server_REST_GetStrLenOfIntDec(CPU_INT32U i);

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       REST RESOURCE VARIABLES
 *******************************************************************************************************/

//                                                                 Resource associated with the User List.
HTTPs_REST_RESOURCE Ex_HTTP_Server_REST_ListResource = {
  "/users",                                                     // Resource URI
  DEF_NULL,                                                     // No HTTP headers to keep.
  0,                                                            // No HTTP headers to keep.
  {
    DEF_NULL,                                               // Method Delete
    &Ex_HTTP_Server_REST_GetUserListHook,                   // Method Get
    DEF_NULL,                                               // Method Head
    &Ex_HTTP_Server_REST_CreateUserHook,                    // Method Post
    DEF_NULL                                                // Method Put
  }
};

//                                                                 Resource associated with a specific user.
HTTPs_REST_RESOURCE Ex_HTTP_Server_REST_UserResource = {
  "/users/{user_id}",
  DEF_NULL,
  0,
  {
    &Ex_HTTP_Server_REST_DeleteUserHook,                        // Method Delete
    &Ex_HTTP_Server_REST_GetUserInfoHook,                       // Method Get
    DEF_NULL,                                                   // Method Head
    DEF_NULL,                                                   // Method Post
    &Ex_HTTP_Server_REST_SetUserInfoHook                        // Method Put
  }
};

//                                                                 Resource associated with files.
HTTPs_REST_RESOURCE Ex_HTTP_Server_REST_FileResource = {
  "/{file}",
  DEF_NULL,
  0,
  {
    DEF_NULL,                                                   // Method Delete
    &Ex_HTTP_Server_REST_GetPage,                               // Method Get
    DEF_NULL,                                                   // Method Head
    DEF_NULL,                                                   // Method Post
    DEF_NULL                                                    // Method Put
  }
};

/********************************************************************************************************
 *                                       APP DATA VARIABLES
 *******************************************************************************************************/

static MEM_DYN_POOL Ex_HTTP_Server_REST_UserPool;                                  // Pool of user objects.
static MEM_DYN_POOL Ex_HTTP_Server_REST_FreeUserID_Pool;                           // Pool of Free User ID objects.
static MEM_DYN_POOL Ex_HTTP_Server_REST_DataPool;                                  // Pool of app data objects.

static CPU_INT32U Ex_HTTP_Server_REST_UserCtr;                                     // Counter of the current number of users.

static EX_HTTP_SERVER_REST_USER         *Ex_HTTP_Server_REST_UserFirstPtr;         // Pointer to the Head of the user list.
static EX_HTTP_SERVER_REST_USER         *Ex_HTTP_Server_REST_UserLastPtr;          // Pointer to the End of the user list.
static EX_HTTP_SERVER_REST_FREE_USER_ID *Ex_HTTP_Server_REST_FreeUserID_ListPtr;   // Pointer to the Head of the Free user ID list.
static EX_HTTP_SERVER_REST_DATA         *Ex_HTTP_Server_REST_DataListPtr;          // Pointer to the Head of the app data object list.

static CPU_CHAR *Ex_HTTP_Server_REST_UserID_Str;                                   // Pointer to the mem seg to temp copy the ID to str.

static CPU_INT32U Ex_HTTP_Server_REST_ListRdRefCtr;                                // Counter for the nbr of req accessing the list in rd.

static CPU_INT32U Ex_HTTP_Server_REST_ListWrRefCtr;                                // Counter for the nbr of req accessing the list in wr.

/****************************************************************************************************//**
 *                                      Ex_HTTP_Server_REST_MemInit()
 *
 * @brief  Allocate the necessary memory for the REST application.
 *******************************************************************************************************/
void Ex_HTTP_Server_REST_MemInit(void)
{
  CPU_SIZE_T char_nbr;
  RTOS_ERR   err;

  Ex_HTTP_Server_REST_UserCtr = 0u;
  Ex_HTTP_Server_REST_UserFirstPtr = DEF_NULL;
  Ex_HTTP_Server_REST_UserLastPtr = DEF_NULL;
  Ex_HTTP_Server_REST_FreeUserID_ListPtr = DEF_NULL;
  Ex_HTTP_Server_REST_DataListPtr = DEF_NULL;

  char_nbr = Ex_HTTP_Server_REST_GetStrLenOfIntDec(EX_HTTP_SERVER_REST_USER_MAX_NBR);
  Ex_HTTP_Server_REST_UserID_Str = (CPU_CHAR *)Mem_SegAlloc("User ID String",
                                                            DEF_NULL,
                                                            char_nbr,
                                                            &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Mem_DynPoolCreate("User Pool",
                    &Ex_HTTP_Server_REST_UserPool,
                    DEF_NULL,
                    sizeof(EX_HTTP_SERVER_REST_USER),
                    sizeof(CPU_SIZE_T),
                    1u,
                    EX_HTTP_SERVER_REST_USER_MAX_NBR,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Mem_DynPoolCreate("Free User ID Pool",
                    &Ex_HTTP_Server_REST_FreeUserID_Pool,
                    DEF_NULL,
                    sizeof(EX_HTTP_SERVER_REST_FREE_USER_ID),
                    sizeof(CPU_SIZE_T),
                    1u,
                    EX_HTTP_SERVER_REST_USER_MAX_NBR,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  Mem_DynPoolCreate("App Data Pool",
                    &Ex_HTTP_Server_REST_DataPool,
                    DEF_NULL,
                    sizeof(EX_HTTP_SERVER_REST_DATA),
                    sizeof(CPU_SIZE_T),
                    1u,
                    EX_HTTP_SERVER_REST_HTTPs_CONN_NBR_MAX,
                    &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      Ex_HTTP_Server_REST_GetPage()
 *
 * @brief  Hook function that will be called when GET request is received with the any URI that is not
 *         "/users" or /users/{user_id}.
 *         Therefore, this hook function is to catch all the HTTP requests that are not specific to REST
 *         but are related to the web page browsing, for example get html/js/image files for the view in
 *         the web browser.
 *
 * @param  p_resource      Pointer to REST resource.
 *
 * @param  p_uri           Pointer to the REST URI of the resource.
 *
 * @param  state           REST State :
 *                             - HTTPs_REST_STATE_INIT:  Bound to the initialize hook of the HTTP server (OnInstanceInitHook).
 *                             - HTTPs_REST_STATE_RX:    Bound to the RX hook of the HTTP server (OnReqBodyRxHook).
 *                             - HTTPs_REST_STATE_TX:    Bound to the ready hook (OnReqRdySignalHook) and the TX chunk hook (OnRespChunkHook).
 *                             - HTTPs_REST_STATE_CLOSE: Bound to the 'OnTransCompleteHook' and 'OnConnCloseHook'.
 *                             - HTTPs_REST_STATE_ERROR: Should not occur.
 *
 * @param  p_data          Pointer to the connection application data pointer.
 *
 * @param  p_instance      Pointer to the HTTP server instance object.
 *
 * @param  p_conn          Pointer to the HTTP connection object.
 *
 * @param  p_buf           Pointer to the connection buffer to read/write data.
 *
 * @param  buf_len         RX state: Data length available to be read.
 *                         TX state: Buffer length available to write.
 *
 * @param  p_buf_len_used  RX state: Must be updated by the hook to indicate the length of the data read by the app.
 *                         TX state: Must be updated by the hook to indicate the length of the data written by the app.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *
 * @note   (1) Since the default behavior of the HTTP core is to serve web pages, this hook function
 *             does nothing and let the server core process the request.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_GetPage(const HTTPs_REST_RESOURCE    *p_resource,
                                                         const HTTPs_REST_MATCHED_URI *p_uri,
                                                         const HTTPs_REST_STATE       state,
                                                         void                         **p_data,
                                                         const HTTPs_INSTANCE         *p_instance,
                                                         HTTPs_CONN                   *p_conn,
                                                         void                         *p_buf,
                                                         const CPU_SIZE_T             buf_len,
                                                         CPU_SIZE_T                   *p_buf_len_used)
{
  PP_UNUSED_PARAM(p_resource);
  PP_UNUSED_PARAM(p_uri);
  PP_UNUSED_PARAM(state);
  PP_UNUSED_PARAM(p_data);
  PP_UNUSED_PARAM(p_instance);
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(p_buf_len_used);

  return (HTTPs_REST_HOOK_STATE_CONTINUE);                      // See note #1.
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_GetUserListHook()
 *
 * @brief  Hook function that will be called when GET request is received with the "/users" URI.
 *         This function, will set the response body data to a JSON with the user list.
 *
 * @param  p_resource      Pointer to REST resource.
 *
 * @param  p_uri           Pointer to the REST URI of the resource.
 *
 * @param  state           REST State :
 *                             - HTTPs_REST_STATE_INIT:  Bound to the initialize hook of the HTTP server (OnInstanceInitHook).
 *                             - HTTPs_REST_STATE_RX:    Bound to the RX hook of the HTTP server (OnReqBodyRxHook).
 *                             - HTTPs_REST_STATE_TX:    Bound to the ready hook (OnReqRdySignalHook) and the TX chunk hook (OnRespChunkHook).
 *                             - HTTPs_REST_STATE_CLOSE: Bound to the 'OnTransCompleteHook' and 'OnConnCloseHook'.
 *                             - HTTPs_REST_STATE_ERROR: Should not occur.
 *
 * @param  p_data          Pointer to the connection application data pointer.
 *
 * @param  p_instance      Pointer to the HTTP server instance object.
 *
 * @param  p_conn          Pointer to the HTTP connection object.
 *
 * @param  p_buf           Pointer to the connection buffer to read/write data.
 *
 * @param  buf_len         RX state: Data length available to be read.
 *                         TX state: Buffer length available to write.
 *
 * @param  p_buf_len_used  RX state: Must be updated by the hook to indicate the length of the data read by the app.
 *                         TX state: Must be updated by the hook to indicate the length of the data written by the app.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_GetUserListHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                 const HTTPs_REST_MATCHED_URI *p_uri,
                                                                 const HTTPs_REST_STATE       state,
                                                                 void                         **p_data,
                                                                 const HTTPs_INSTANCE         *p_instance,
                                                                 HTTPs_CONN                   *p_conn,
                                                                 void                         *p_buf,
                                                                 const CPU_SIZE_T             buf_len,
                                                                 CPU_SIZE_T                   *p_buf_len_used)
{
  EX_HTTP_SERVER_REST_DATA *p_app_data;
  HTTPs_REST_HOOK_STATE    result;
  RTOS_ERR                 err;

  PP_UNUSED_PARAM(p_resource);
  PP_UNUSED_PARAM(p_uri);

  switch (state) {
    case HTTPs_REST_STATE_INIT:
      //                                                           Increment read reference counter of list.
      if (Ex_HTTP_Server_REST_ListWrRefCtr == 0) {
        Ex_HTTP_Server_REST_ListRdRefCtr++;
      } else {
        return (HTTPs_REST_HOOK_STATE_STAY);
      }
      //                                                           Get application data blk for the transaction.
      p_app_data = Ex_HTTP_Server_REST_GetDataBlk();
      if (p_app_data == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_ERROR);
      }
      *p_data = (void *)p_app_data;
      //                                                           Set Response body parameters to use Chunk hook.
      HTTPs_RespBodySetParamStaticData(p_instance,
                                       p_conn,
                                       HTTP_CONTENT_TYPE_JSON,
                                       DEF_NULL,
                                       0,
                                       DEF_NO,
                                       &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      break;

    case HTTPs_REST_STATE_RX:
      //                                                           Nothing to do. It's a GET request, so no body.
      break;

    case HTTPs_REST_STATE_TX:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
                                                                // Write the user list to the connection buffer.
      result = Ex_HTTP_Server_REST_JSON_WrUserListToBuf(p_app_data,
                                                        p_buf,
                                                        buf_len,
                                                        p_conn->BufLen,
                                                        p_buf_len_used);
      if (result != HTTPs_REST_HOOK_STATE_CONTINUE) {
        return (result);
      }
      break;

    case HTTPs_REST_STATE_CLOSE:
      //                                                           Free the application data blk.
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;
      Ex_HTTP_Server_REST_RemoveDataBlk(p_app_data);
      *p_data = DEF_NULL;
      Ex_HTTP_Server_REST_ListRdRefCtr--;
      break;

    default:
      break;
  }

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_GetUserInfoHook()
 *
 * @brief  Hook function that will be called when GET request is received with the "/users/{user_id}" URI.
 *         This function, will set the response body data to a JSON with the user info specified by the
 *         user ID.
 *
 * @param  p_resource      Pointer to REST resource.
 *
 * @param  p_uri           Pointer to the REST URI of the resource.
 *
 * @param  state           REST State :
 *                             - HTTPs_REST_STATE_INIT:  Bound to the initialize hook of the HTTP server (OnInstanceInitHook).
 *                             - HTTPs_REST_STATE_RX:    Bound to the RX hook of the HTTP server (OnReqBodyRxHook).
 *                             - HTTPs_REST_STATE_TX:    Bound to the ready hook (OnReqRdySignalHook) and the TX chunk hook (OnRespChunkHook).
 *                             - HTTPs_REST_STATE_CLOSE: Bound to the 'OnTransCompleteHook' and 'OnConnCloseHook'.
 *                             - HTTPs_REST_STATE_ERROR: Should not occur.
 *
 * @param  p_data          Pointer to the connection application data pointer.
 *
 * @param  p_instance      Pointer to the HTTP server instance object.
 *
 * @param  p_conn          Pointer to the HTTP connection object.
 *
 * @param  p_buf           Pointer to the connection buffer to read/write data.
 *
 * @param  buf_len         RX state: Data length available to be read.
 *                         TX state: Buffer length available to write.
 *
 * @param  p_buf_len_used  RX state: Must be updated by the hook to indicate the length of the data read by the app.
 *                         TX state: Must be updated by the hook to indicate the length of the data written by the app.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *
 *
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_GetUserInfoHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                 const HTTPs_REST_MATCHED_URI *p_uri,
                                                                 const HTTPs_REST_STATE       state,
                                                                 void                         **p_data,
                                                                 const HTTPs_INSTANCE         *p_instance,
                                                                 HTTPs_CONN                   *p_conn,
                                                                 void                         *p_buf,
                                                                 const CPU_SIZE_T             buf_len,
                                                                 CPU_SIZE_T                   *p_buf_len_used)
{
  CPU_INT08U               user_id = DEF_INT_08U_MAX_VAL;
  EX_HTTP_SERVER_REST_DATA *p_app_data;
  EX_HTTP_SERVER_REST_USER *p_user;
  HTTPs_REST_HOOK_STATE    result;
  RTOS_ERR                 err;

  PP_UNUSED_PARAM(p_resource);

  switch (state) {
    case HTTPs_REST_STATE_INIT:
      //                                                           Get application data blk for the transaction.
      p_app_data = Ex_HTTP_Server_REST_GetDataBlk();
      if (p_app_data == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_ERROR);
      }
      *p_data = (void *)p_app_data;
      //                                                           Retrieve User ID from Rx URI.
      if (p_uri->WildCardsNbr > 0) {
        user_id = Str_ParseNbr_Int32U(p_uri->WildCards[0].ValPtr,
                                      DEF_NULL,
                                      10);
      }
      //                                                           Retrieve User with user ID.
      p_user = Ex_HTTP_Server_REST_FindUser(user_id);
      if (p_user == DEF_NULL) {
        p_conn->StatusCode = HTTP_STATUS_NOT_FOUND;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Check if user is reference by a written transaction.
      if (p_user->WrRefCtr != 0) {
        p_conn->StatusCode = HTTP_STATUS_CONFLICT;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      } else {
        p_user->RdRefCtr++;                                     // Increment read reference counter of user.
      }

      p_app_data->UserPtr = p_user;

      //                                                           Set Response body parameters to use Chunk hook.
      HTTPs_RespBodySetParamStaticData(p_instance,
                                       p_conn,
                                       HTTP_CONTENT_TYPE_JSON,
                                       DEF_NULL,
                                       0,
                                       DEF_NO,
                                       &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      break;

    case HTTPs_REST_STATE_RX:
      //                                                           Nothing to do. It's a GET request, so no body.
      break;

    case HTTPs_REST_STATE_TX:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
      p_user = p_app_data->UserPtr;

      //                                                           Write User Info to response body.
      result = Ex_HTTP_Server_REST_JSON_WrUserInfoToBuf(p_user,
                                                        p_buf,
                                                        buf_len,
                                                        p_conn->BufLen,
                                                        p_buf_len_used);
      if (result != HTTPs_REST_HOOK_STATE_CONTINUE) {
        return (result);
      }
      break;

    case HTTPs_REST_STATE_CLOSE:
      //                                                           Decrement the Read Reference Counter of the user.
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;
      p_user = p_app_data->UserPtr;
      if (p_user != DEF_NULL) {
        p_user->RdRefCtr--;
      }
      p_app_data->UserPtr = DEF_NULL;

      //                                                           Free the application data blk.
      Ex_HTTP_Server_REST_RemoveDataBlk(p_app_data);
      *p_data = DEF_NULL;
      break;

    default:
      break;
  }

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_SetUserInfoHook()
 *
 * @brief  Hook function that will be called when PUT request is received with the "/users/{user_id}" URI.
 *         This function, will parse the JSON with the new user info received in the request body and set
 *         the new user info.
 *         The hook will also set the response body data to a JSON with the user first name and last name
 *         to update the list preview.
 *
 * @param  p_resource      Pointer to REST resource.
 *
 * @param  p_uri           Pointer to the REST URI of the resource.
 *
 * @param  state           REST State :
 *                             - HTTPs_REST_STATE_INIT:  Bound to the initialize hook of the HTTP server (OnInstanceInitHook).
 *                             - HTTPs_REST_STATE_RX:    Bound to the RX hook of the HTTP server (OnReqBodyRxHook).
 *                             - HTTPs_REST_STATE_TX:    Bound to the ready hook (OnReqRdySignalHook) and the TX chunk hook (OnRespChunkHook).
 *                             - HTTPs_REST_STATE_CLOSE: Bound to the 'OnTransCompleteHook' and 'OnConnCloseHook'.
 *                             - HTTPs_REST_STATE_ERROR: Should not occur.
 *
 * @param  p_data          Pointer to the connection application data pointer.
 *
 * @param  p_instance      Pointer to the HTTP server instance object.
 *
 * @param  p_conn          Pointer to the HTTP connection object.
 *
 * @param  p_buf           Pointer to the connection buffer to read/write data.
 *
 * @param  buf_len         RX state: Data length available to be read.
 *                         TX state: Buffer length available to write.
 *
 * @param  p_buf_len_used  RX state: Must be updated by the hook to indicate the length of the data read by the app.
 *                         TX state: Must be updated by the hook to indicate the length of the data written by the app.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_SetUserInfoHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                 const HTTPs_REST_MATCHED_URI *p_uri,
                                                                 const HTTPs_REST_STATE       state,
                                                                 void                         **p_data,
                                                                 const HTTPs_INSTANCE         *p_instance,
                                                                 HTTPs_CONN                   *p_conn,
                                                                 void                         *p_buf,
                                                                 const CPU_SIZE_T             buf_len,
                                                                 CPU_SIZE_T                   *p_buf_len_used)
{
  EX_HTTP_SERVER_REST_DATA *p_app_data;
  EX_HTTP_SERVER_REST_USER *p_user;
  CPU_INT08U               user_id = DEF_INT_08U_MAX_VAL;
  HTTPs_REST_HOOK_STATE    result;
  RTOS_ERR                 err;

  PP_UNUSED_PARAM(p_resource);

  switch (state) {
    case HTTPs_REST_STATE_INIT:
      //                                                           Get application data blk for the transaction.
      p_app_data = Ex_HTTP_Server_REST_GetDataBlk();
      if (p_app_data == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_ERROR);
      }
      *p_data = (void *)p_app_data;
      //                                                           Retrieve User ID from Rx URI.
      if (p_uri->WildCardsNbr > 0) {
        user_id = Str_ParseNbr_Int32U(p_uri->WildCards[0].ValPtr,
                                      DEF_NULL,
                                      10);
      }
      //                                                           Retrieve User with user ID.
      p_user = Ex_HTTP_Server_REST_FindUser(user_id);
      if (p_user == DEF_NULL) {
        p_conn->StatusCode = HTTP_STATUS_NOT_FOUND;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Check if user is already reference elsewhere.
      if ((Ex_HTTP_Server_REST_ListRdRefCtr != 0u)
          || (p_user->WrRefCtr != 0u)
          || (p_user->RdRefCtr != 0u)) {
        p_conn->StatusCode = HTTP_STATUS_CONFLICT;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      } else {
        p_user->WrRefCtr++;                                     // Increment write reference counter of user.
        Ex_HTTP_Server_REST_ListWrRefCtr++;                     // Increment write reference counter of list.
      }

      p_app_data->UserPtr = p_user;
      p_app_data->FieldType = EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME;
      //                                                           Set Response body parameters to use Chunk hook.
      HTTPs_RespBodySetParamStaticData(p_instance,
                                       p_conn,
                                       HTTP_CONTENT_TYPE_JSON,
                                       DEF_NULL,
                                       0,
                                       DEF_NO,
                                       &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      break;

    case HTTPs_REST_STATE_RX:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
      p_user = p_app_data->UserPtr;
      //                                                           If no user was found, just continue to send a 404.
      if (p_user == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           If no data available to read yet, just continue.
      if (buf_len == 0) {
        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Parse the user info contain in the request body.
      result = Ex_HTTP_Server_REST_JSON_ParseUserInfo(p_app_data,
                                                      p_user,
                                                      p_buf,
                                                      buf_len,
                                                      p_conn->BufLen,
                                                      p_buf_len_used);
      if (result != HTTPs_REST_HOOK_STATE_CONTINUE) {
        return (result);
      }
      *p_buf_len_used = buf_len;
      break;

    case HTTPs_REST_STATE_TX:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
      p_user = p_app_data->UserPtr;

      //                                                           Write User name to response body.
      result = Ex_HTTP_Server_REST_JSON_WrUserToBuf(p_user,
                                                    p_buf,
                                                    buf_len,
                                                    p_conn->BufLen,
                                                    p_buf_len_used);
      if (result != HTTPs_REST_HOOK_STATE_CONTINUE) {
        return (result);
      }
      break;

    case HTTPs_REST_STATE_CLOSE:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;                    // Retrieve application data blk.
      p_user = p_app_data->UserPtr;
      if (p_user != DEF_NULL) {
        p_user->WrRefCtr--;                                     // Decrement the Write Reference Counter of the user.
        Ex_HTTP_Server_REST_ListWrRefCtr--;                                 // Decrement the Write Reference Counter of the list.
      }
      p_app_data->UserPtr = DEF_NULL;
      //                                                           Free the application data blk.
      Ex_HTTP_Server_REST_RemoveDataBlk(p_app_data);
      *p_data = DEF_NULL;
      break;

    default:
      break;
  }

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_CreateUserHook()
 *
 * @brief  Hook function that will be called when POST request is received with the "/users" URI.
 *         This function, will parse the JSON with the new user name received in the request body and create
 *         a new user to add to the list.
 *         The hook will also set the response body data to a JSON with the user first name and last name
 *         to update the list preview.
 *
 * @param  p_resource      Pointer to REST resource.
 *
 * @param  p_uri           Pointer to the REST URI of the resource.
 *
 * @param  state           REST State :
 *                             - HTTPs_REST_STATE_INIT:  Bound to the initialize hook of the HTTP server (OnInstanceInitHook).
 *                             - HTTPs_REST_STATE_RX:    Bound to the RX hook of the HTTP server (OnReqBodyRxHook).
 *                             - HTTPs_REST_STATE_TX:    Bound to the ready hook (OnReqRdySignalHook) and the TX chunk hook (OnRespChunkHook).
 *                             - HTTPs_REST_STATE_CLOSE: Bound to the 'OnTransCompleteHook' and 'OnConnCloseHook'.
 *                             - HTTPs_REST_STATE_ERROR: Should not occur.
 *
 * @param  p_data          Pointer to the connection application data pointer.
 *
 * @param  p_instance      Pointer to the HTTP server instance object.
 *
 * @param  p_conn          Pointer to the HTTP connection object.
 *
 * @param  p_buf           Pointer to the connection buffer to read/write data.
 *
 * @param  buf_len         RX state: Data length available to be read.
 *                         TX state: Buffer length available to write.
 *
 * @param  p_buf_len_used  RX state: Must be updated by the hook to indicate the length of the data read by the app.
 *                         TX state: Must be updated by the hook to indicate the length of the data written by the app.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_CreateUserHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                const HTTPs_REST_MATCHED_URI *p_uri,
                                                                const HTTPs_REST_STATE       state,
                                                                void                         **p_data,
                                                                const HTTPs_INSTANCE         *p_instance,
                                                                HTTPs_CONN                   *p_conn,
                                                                void                         *p_buf,
                                                                const CPU_SIZE_T             buf_len,
                                                                CPU_SIZE_T                   *p_buf_len_used)
{
  EX_HTTP_SERVER_REST_DATA *p_app_data;
  EX_HTTP_SERVER_REST_USER *p_user;
  HTTPs_REST_HOOK_STATE    result;
  RTOS_ERR                 err;

  PP_UNUSED_PARAM(p_resource);

  switch (state) {
    case HTTPs_REST_STATE_INIT:
      //                                                           Get application data blk for the transaction.
      p_app_data = Ex_HTTP_Server_REST_GetDataBlk();
      if (p_app_data == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_ERROR);
      }
      *p_data = (void *)p_app_data;
      //                                                           Check if the user list is reference elsewhere.
      if (Ex_HTTP_Server_REST_ListRdRefCtr != 0) {
        p_conn->StatusCode = HTTP_STATUS_CONFLICT;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Create and add new user.
      p_user = Ex_HTTP_Server_REST_GetUser();
      if (p_user == DEF_NULL) {
        p_conn->StatusCode = HTTP_STATUS_FORBIDDEN;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return(HTTPs_REST_HOOK_STATE_CONTINUE);
      }

      Ex_HTTP_Server_REST_UserCtr++;
      p_user->WrRefCtr++;
      Ex_HTTP_Server_REST_ListWrRefCtr++;

      p_app_data->UserPtr = p_user;
      p_app_data->FieldType = EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME;

      //                                                           Set Response body parameters to use Chunk hook.
      HTTPs_RespBodySetParamStaticData(p_instance,
                                       p_conn,
                                       HTTP_CONTENT_TYPE_JSON,
                                       DEF_NULL,
                                       0,
                                       DEF_NO,
                                       &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
      break;

    case HTTPs_REST_STATE_RX:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
      p_user = p_app_data->UserPtr;
      //                                                           If no user was created, just continue.
      if (p_user == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           If no data available to read yet, just continue.
      if (buf_len == 0) {
        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Parse the user name contain in the request body.
      result = Ex_HTTP_Server_REST_JSON_ParseUser(p_app_data,
                                                  p_user,
                                                  p_uri,
                                                  p_buf,
                                                  buf_len,
                                                  p_conn->BufLen,
                                                  p_buf_len_used);
      if (result != HTTPs_REST_HOOK_STATE_CONTINUE) {
        return (result);
      }
      *p_buf_len_used = buf_len;
      p_conn->StatusCode = HTTP_STATUS_CREATED;
      break;

    case HTTPs_REST_STATE_TX:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
      p_user = p_app_data->UserPtr;

      //                                                           Write User name to response body.
      result = Ex_HTTP_Server_REST_JSON_WrUserToBuf(p_user,
                                                    p_buf,
                                                    buf_len,
                                                    p_conn->BufLen,
                                                    p_buf_len_used);
      if (result != HTTPs_REST_HOOK_STATE_CONTINUE) {
        return (result);
      }
      break;

    case HTTPs_REST_STATE_CLOSE:
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;         // Retrieve application data blk.
      p_user = p_app_data->UserPtr;
      if (p_user != DEF_NULL) {
        p_user->WrRefCtr--;                                     // Decrement the Write Reference Counter of the user.
        Ex_HTTP_Server_REST_ListWrRefCtr--;                     // Decrement the Write Reference Counter of the list.
      }
      p_app_data->UserPtr = DEF_NULL;
      //                                                           Free the application data blk.
      Ex_HTTP_Server_REST_RemoveDataBlk(p_app_data);
      *p_data = DEF_NULL;
      break;

    default:
      break;
  }

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_DeleteUserHook()
 *
 * @brief  Hook function that will be called when DELETE request is received with the "/users/{user_id}" URI.
 *         This function, will simply remove the user specified by the user id of the URI from the list.
 *
 * @param  p_resource      Pointer to REST resource.
 *
 * @param  p_uri           Pointer to the REST URI of the resource.
 *
 * @param  state           REST State :
 *                             - HTTPs_REST_STATE_INIT:  Bound to the initialize hook of the HTTP server (OnInstanceInitHook).
 *                             - HTTPs_REST_STATE_RX:    Bound to the RX hook of the HTTP server (OnReqBodyRxHook).
 *                             - HTTPs_REST_STATE_TX:    Bound to the ready hook (OnReqRdySignalHook) and the TX chunk hook (OnRespChunkHook).
 *                             - HTTPs_REST_STATE_CLOSE: Bound to the 'OnTransCompleteHook' and 'OnConnCloseHook'.
 *                             - HTTPs_REST_STATE_ERROR: Should not occur.
 *
 * @param  p_data          Pointer to the connection application data pointer.
 *
 * @param  p_instance      Pointer to the HTTP server instance object.
 *
 * @param  p_conn          Pointer to the HTTP connection object.
 *
 * @param  p_buf           Pointer to the connection buffer to read/write data.
 *
 * @param  buf_len         RX state: Data length available to be read.
 *                         TX state: Buffer length available to write.
 *
 * @param  p_buf_len_used  RX state: Must be updated by the hook to indicate the length of the data read by the app.
 *                         TX state: Must be updated by the hook to indicate the length of the data written by the app.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *
 * @note   (1) If the user specify by the user ID of the URL is not found a 404 (Not Found) status code
 *             will be send back.
 *
 * @note   (2) If the user is already used by another transaction, a 409 (Conflict) status code will
 *             be send back.
 *
 * @note   (3) If the delete is successful, a 204 (No Content) status code will be send back.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_DeleteUserHook(const HTTPs_REST_RESOURCE    *p_resource,
                                                                const HTTPs_REST_MATCHED_URI *p_uri,
                                                                const HTTPs_REST_STATE       state,
                                                                void                         **p_data,
                                                                const HTTPs_INSTANCE         *p_instance,
                                                                HTTPs_CONN                   *p_conn,
                                                                void                         *p_buf,
                                                                const CPU_SIZE_T             buf_len,
                                                                CPU_SIZE_T                   *p_buf_len_used)
{
  EX_HTTP_SERVER_REST_DATA *p_app_data;
  EX_HTTP_SERVER_REST_USER *p_user;
  CPU_INT08U               user_id = DEF_INT_08U_MAX_VAL;
  RTOS_ERR                 err;

  PP_UNUSED_PARAM(p_resource);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_len);
  PP_UNUSED_PARAM(p_buf_len_used);

  switch (state) {
    case HTTPs_REST_STATE_INIT:
      //                                                           Get application data blk for the transaction.
      p_app_data = Ex_HTTP_Server_REST_GetDataBlk();
      if (p_app_data == DEF_NULL) {
        return (HTTPs_REST_HOOK_STATE_ERROR);
      }
      *p_data = (void *)p_app_data;
      //                                                           Retrieve User ID from Rx URI.
      if (p_uri->WildCardsNbr > 0) {
        user_id = Str_ParseNbr_Int32U(p_uri->WildCards[0u].ValPtr,
                                      DEF_NULL,
                                      10u);
      }
      //                                                           Retrieve User with user ID.
      p_user = Ex_HTTP_Server_REST_FindUser(user_id);
      if (p_user == DEF_NULL) {
        p_conn->StatusCode = HTTP_STATUS_NOT_FOUND;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return(HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Check if the user is already reference elsewhere.
      if ((Ex_HTTP_Server_REST_ListRdRefCtr != 0u)
          || (p_user->WrRefCtr != 0u)
          || (p_user->RdRefCtr != 0u)) {
        p_conn->StatusCode = HTTP_STATUS_CONFLICT;

        HTTPs_RespBodySetParamNoBody(p_instance,
                                     p_conn,
                                     &err);
        APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

        return (HTTPs_REST_HOOK_STATE_CONTINUE);
      }
      //                                                           Remove User from list.
      Ex_HTTP_Server_REST_RemoveUser(p_user);
      Ex_HTTP_Server_REST_UserCtr--;
      //                                                           Set Response body parameters to no body.
      HTTPs_RespBodySetParamNoBody(p_instance,
                                   p_conn,
                                   &err);
      APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

      p_conn->StatusCode = HTTP_STATUS_NO_CONTENT;
      break;

    case HTTPs_REST_STATE_RX:
      //                                                           Nothing to do. It's a DELETE request with no body.
      break;

    case HTTPs_REST_STATE_TX:
      //                                                           Nothing to do. The response will not have a body.
      break;

    case HTTPs_REST_STATE_CLOSE:
      //                                                           Free the application data blk.
      p_app_data = (EX_HTTP_SERVER_REST_DATA *)*p_data;
      p_app_data->UserPtr = DEF_NULL;
      Ex_HTTP_Server_REST_RemoveDataBlk(p_app_data);
      *p_data = DEF_NULL;
      break;

    default:
      break;
  }

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                               Ex_HTTP_Server_REST_JSON_GetUserFieldSize()
 *
 * @brief  Calculate the necessary buffer size to write a User Field from the user list for a
 *         specific user.
 *
 * @param  p_user  Pointer to the user object.
 *
 * @return  size of data to write.
 *******************************************************************************************************/
static CPU_SIZE_T Ex_HTTP_Server_REST_JSON_GetUserFieldSize(EX_HTTP_SERVER_REST_USER *p_user)
{
  CPU_SIZE_T char_nbr;
  CPU_SIZE_T tot_size = 0u;

  //                                                               Size of all JSON specific characters { } , : "
  tot_size += EX_HTTP_SERVER_REST_JSON_OBJ_CHARS_NBR
              + EX_HTTP_SERVER_REST_JSON_FIELD_MAX_CHARS_NBR * 3
              + EX_HTTP_SERVER_REST_JSON_FIELD_MIN_CHARS_NBR;

  //                                                               Size of all the field keys.
  tot_size += sizeof(EX_HTTP_SERVER_REST_JSON_KEY_ID_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_FIRST_NAME_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_LAST_NAME_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_LINK_STR_NAME);

  //                                                               Size of all the field values.
  tot_size += Str_Len_N(p_user->FirstName, EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN)
              + Str_Len_N(p_user->LastName, EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN)
              + Str_Len_N(p_user->Link, EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN);

  char_nbr = Ex_HTTP_Server_REST_GetStrLenOfIntDec(p_user->ID);
  Str_FmtNbr_Int32U(p_user->ID, char_nbr, 10, '\0', DEF_NO, DEF_YES, Ex_HTTP_Server_REST_UserID_Str);
  tot_size += Str_Len_N(Ex_HTTP_Server_REST_UserID_Str, char_nbr);

  return (tot_size);
}

/****************************************************************************************************//**
 *                               Ex_HTTP_Server_REST_JSON_GetUserInfoSize()
 *
 * @brief  Calculate the necessary buffer size to write user info fields for a specific user.
 *
 * @param  p_user  Pointer to the user object.
 *
 * @return  size of data to write.
 *******************************************************************************************************/
static CPU_SIZE_T Ex_HTTP_Server_REST_JSON_GetUserInfoSize(EX_HTTP_SERVER_REST_USER *p_user)
{
  CPU_CHAR   str[EX_HTTP_SERVER_REST_AGE_STR_MAX_LEN];
  CPU_SIZE_T char_nbr;
  CPU_SIZE_T tot_size = 0;

  //                                                               Size of all JSON specific characters { } , : "
  tot_size += EX_HTTP_SERVER_REST_JSON_OBJ_CHARS_NBR
              + EX_HTTP_SERVER_REST_JSON_FIELD_MAX_CHARS_NBR * 5
              + EX_HTTP_SERVER_REST_JSON_FIELD_MIN_CHARS_NBR;

  //                                                               Size of all the field keys.
  tot_size += sizeof(EX_HTTP_SERVER_REST_JSON_KEY_ID_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_FIRST_NAME_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_LAST_NAME_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_GENDER_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_AGE_STR_NAME)
              + sizeof(EX_HTTP_SERVER_REST_JSON_KEY_JOB_STR_NAME);

  //                                                               Size of all the field values.
  tot_size += Str_Len_N(p_user->FirstName, EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN)
              + Str_Len_N(p_user->LastName, EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN)
              + Str_Len_N(p_user->Gender, EX_HTTP_SERVER_REST_GENDER_STR_MAX_LEN)
              + Str_Len_N(p_user->JobTitle, EX_HTTP_SERVER_REST_JOB_TITLE_STR_MAX_LEN);

  char_nbr = Ex_HTTP_Server_REST_GetStrLenOfIntDec(p_user->ID);
  Str_FmtNbr_Int32U(p_user->ID, char_nbr, 10, '\0', DEF_NO, DEF_YES, Ex_HTTP_Server_REST_UserID_Str);
  tot_size += Str_Len_N(Ex_HTTP_Server_REST_UserID_Str, char_nbr);

  Str_FmtNbr_Int32U(p_user->Age, EX_HTTP_SERVER_REST_AGE_STR_MAX_LEN, 10, '\0', DEF_NO, DEF_YES, &str[0]);
  tot_size += Str_Len_N(&str[0], EX_HTTP_SERVER_REST_AGE_STR_MAX_LEN);

  return (tot_size);
}

/****************************************************************************************************//**
 *                                 Ex_HTTP_Server_REST_JSON_WrUserToBuf()
 *
 * @brief  Write JSON response with the following user fields : user ID, first name, last name and link.
 *
 * @param  p_user          Pointer to user object.
 *
 * @param  p_buf           Pointer to connection buffer where to write data.
 *
 * @param  buf_len         Length available in buffer to write data.
 *
 * @param  buf_len_max     The maximum size of the connection buffer.
 *
 * @param  p_buf_len_used  Variable that will received the length of data written to the buffer.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *
 * @note   (1) The connection buffer must be enough big to fit all the fields of one user.
 *             Else the process will fail.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_WrUserToBuf(EX_HTTP_SERVER_REST_USER *p_user,
                                                                  void                     *p_buf,
                                                                  const CPU_SIZE_T         buf_len,
                                                                  const CPU_SIZE_T         buf_len_max,
                                                                  CPU_SIZE_T               *p_buf_len_used)
{
  CPU_CHAR   *p_str;
  CPU_SIZE_T char_nbr;
  CPU_SIZE_T data_len = 0u;

  p_str = p_buf;

  //                                                               Calculate the data length to write.
  data_len = Ex_HTTP_Server_REST_JSON_GetUserFieldSize(p_user);

  if (data_len > buf_len_max) {
    return (HTTPs_REST_HOOK_STATE_ERROR);                       // The configured buffer size is not enough.
  }
  if (data_len > buf_len) {
    return (HTTPs_REST_HOOK_STATE_STAY);                        // not enough space in the buffer, so try next time.
  }

  //                                                               Write JSON data to buffer.
  char_nbr = Ex_HTTP_Server_REST_GetStrLenOfIntDec(p_user->ID);
  Str_FmtNbr_Int32U(p_user->ID, char_nbr, 10u, '\0', DEF_NO, DEF_YES, Ex_HTTP_Server_REST_UserID_Str);
  Str_Copy(p_str, "{\"User ID\": \"");
  Str_Cat(p_str, Ex_HTTP_Server_REST_UserID_Str);
  Str_Cat(p_str, "\", \"First Name\": \"");
  Str_Cat(p_str, p_user->FirstName);
  Str_Cat(p_str, "\", \"Last Name\": \"");
  Str_Cat(p_str, p_user->LastName);
  Str_Cat(p_str, "\", \"Link\":\"");
  Str_Cat(p_str, p_user->Link);
  Str_Cat(p_str, "\"}");

  *p_buf_len_used = Str_Len_N(p_str, buf_len);

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                               Ex_HTTP_Server_REST_JSON_WrUserListToBuf()
 *
 * @brief  Write the JSON user list to the connection buffer.
 *
 * @param  p_app_data      Pointer to application data object for the current transaction.
 *
 * @param  p_buf           Pointer to connection buffer where to write data.
 *
 * @param  buf_len         Length available in buffer to write data.
 *
 * @param  buf_len_max     The maximum size of the connection buffer.
 *
 * @param  p_buf_len_used  Variable that will received the length of data written to the buffer.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *
 * @note   (1) The connection buffer must be enough big to fit all the fields of one user.
 *             Else the process will fail.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_WrUserListToBuf(EX_HTTP_SERVER_REST_DATA *p_app_data,
                                                                      void                     *p_buf,
                                                                      const CPU_SIZE_T         buf_len,
                                                                      const CPU_SIZE_T         buf_len_max,
                                                                      CPU_SIZE_T               *p_buf_len_used)
{
  EX_HTTP_SERVER_REST_USER *p_user;
  CPU_CHAR                 *p_str = p_buf;
  HTTPs_REST_HOOK_STATE    state;
  CPU_SIZE_T               buf_len_rem = buf_len;

  //                                                               Write start of JSON list if not already done.
  if (p_app_data->UserPtr == DEF_NULL) {
    if (sizeof(EX_HTTP_SERVER_REST_JSON_LIST_USER_START_STR) > buf_len) {
      return (HTTPs_REST_HOOK_STATE_STAY);
    }

    Str_Copy(p_str, EX_HTTP_SERVER_REST_JSON_LIST_USER_START_STR);
    p_str += Str_Len(p_str);
    p_user = Ex_HTTP_Server_REST_UserFirstPtr;
  } else {
    p_user = p_app_data->UserPtr;
  }

  //                                                               Write all users of list until buffer is full.
  while (p_user != DEF_NULL) {
    //                                                             Write user fields to buffer.
    state = Ex_HTTP_Server_REST_JSON_WrUserToBuf(p_user, p_str, buf_len_rem, buf_len_max, p_buf_len_used);
    switch (state) {
      case HTTPs_REST_HOOK_STATE_CONTINUE:
        break;

      case HTTPs_REST_HOOK_STATE_STAY:                          // Buffer is full, save the index user for next time.
        p_app_data->UserPtr = p_user;
        *p_buf_len_used = p_str - (CPU_CHAR *)p_buf;
        return (state);

      case HTTPs_REST_HOOK_STATE_ERROR:
      default:
        return (state);
    }

    if (p_user != Ex_HTTP_Server_REST_UserLastPtr) {            // Write comma if user is not the last of the list.
      Str_Cat(p_str, ",");
      p_str++;
    }

    p_str += *p_buf_len_used;
    buf_len_rem -= *p_buf_len_used;

    p_user = p_user->NextPtr;
  }

  Str_Cat(p_str, "]}");                                         // Write end of table list in JSON.
  p_str += 2;
  p_app_data->UserPtr = DEF_NULL;
  *p_buf_len_used = (p_str - (CPU_CHAR *)p_buf);

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                               Ex_HTTP_Server_REST_JSON_WrUserInfoToBuf()
 *
 * @brief  Write the JSON with the user info fields.
 *
 * @param  p_user          Pointer to user object.
 *
 * @param  p_buf           Pointer to connection buffer where to write data.
 *
 * @param  buf_len         Length available in buffer to write data.
 *
 * @param  buf_len_max     The maximum size of the connection buffer.
 *
 * @param  p_buf_len_used  Variable that will received the length of data written to the buffer.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *
 * @note   (1) The connection buffer must be enough big to fit all the info fields of the user.
 *             Else the process will fail.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_WrUserInfoToBuf(EX_HTTP_SERVER_REST_USER *p_user,
                                                                      void                     *p_buf,
                                                                      const CPU_SIZE_T         buf_len,
                                                                      const CPU_SIZE_T         buf_len_max,
                                                                      CPU_SIZE_T               *p_buf_len_used)
{
  CPU_CHAR   *p_str = p_buf;
  CPU_CHAR   str[3u];
  CPU_SIZE_T data_len = 0u;
  CPU_SIZE_T char_nbr;

  //                                                               Calculate data length to write.
  data_len = Ex_HTTP_Server_REST_JSON_GetUserInfoSize(p_user);
  if (data_len > buf_len_max) {
    return (HTTPs_REST_HOOK_STATE_ERROR);
  }
  if (data_len > buf_len) {
    *p_buf_len_used = 0;
    return (HTTPs_REST_HOOK_STATE_STAY);
  }

  //                                                               Write JSON data with User Info to the buffer.
  Str_Copy(p_str, "{\"User ID\": \"");
  char_nbr = Ex_HTTP_Server_REST_GetStrLenOfIntDec(p_user->ID);
  Str_FmtNbr_Int32U(p_user->ID, char_nbr, 10, '\0', DEF_NO, DEF_YES, Ex_HTTP_Server_REST_UserID_Str);
  Str_Cat(p_str, Ex_HTTP_Server_REST_UserID_Str);
  Str_Cat(p_str, "\", \"First Name\": \"");
  Str_Cat(p_str, p_user->FirstName);
  Str_Cat(p_str, "\", \"Last Name\": \"");
  Str_Cat(p_str, p_user->LastName);
  Str_Cat(p_str, "\", \"Gender\": \"");
  Str_Cat(p_str, p_user->Gender);
  Str_Cat(p_str, "\", \"Age\": \"");
  if (p_user->Age != 0) {
    Str_FmtNbr_Int32U(p_user->Age, EX_HTTP_SERVER_REST_AGE_STR_MAX_LEN, 10, '\0', DEF_NO, DEF_YES, &str[0]);
    Str_Cat(p_str, &str[0]);
  }
  Str_Cat(p_str, "\", \"Job Title\": \"");
  Str_Cat(p_str, p_user->JobTitle);
  Str_Cat(p_str, "\"}");

  *p_buf_len_used = Str_Len(p_str);

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_JSON_ParseUser()
 *
 * @brief  Parse JSON data to retrieve the new user first name and last name.
 *
 * @param  p_app_data      Pointer to application data block.
 *
 * @param  p_user          Pointer to user object.
 *
 * @param  p_uri           Pointer to REST URI received in the request.
 *
 * @param  p_buf           Pointer to the connection buffer with the received data.
 *
 * @param  data_len        Length of the data received inside the buffer.
 *
 * @param  buf_len_max     Maximum size of the connection buffer.
 *
 * @param  p_buf_len_used  Variable that will received the length of the data read.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_ParseUser(EX_HTTP_SERVER_REST_DATA     *p_app_data,
                                                                EX_HTTP_SERVER_REST_USER     *p_user,
                                                                const HTTPs_REST_MATCHED_URI *p_uri,
                                                                void                         *p_buf,
                                                                const CPU_SIZE_T             data_len,
                                                                const CPU_SIZE_T             buf_len_max,
                                                                CPU_SIZE_T                   *p_buf_len_used)
{
  CPU_CHAR                       *p_str_val;
  EX_HTTP_SERVER_REST_USER_FIELD field_type;
  HTTPs_REST_HOOK_STATE          state;
  CPU_SIZE_T                     str_len;
  CPU_SIZE_T                     val_len;
  CPU_SIZE_T                     min_len;
  CPU_SIZE_T                     char_nbr;
  CPU_SIZE_T                     data_rd_len = 0u;

  *p_buf_len_used = 0u;
  //                                                               ------- PARSE RECEIVED JSON TO RETRIEVE USER -------
  field_type = p_app_data->FieldType;
  while (field_type <= EX_HTTP_SERVER_REST_USER_FIELD_LAST_NAME) {
    state = Ex_HTTP_Server_REST_JSON_ParseField(field_type,
                                                &p_str_val,
                                                &val_len,
                                                p_buf,
                                                data_len,
                                                buf_len_max,
                                                &data_rd_len);
    if (state != HTTPs_REST_HOOK_STATE_CONTINUE) {
      p_app_data->FieldType = field_type;
      return (state);
    }

    switch (field_type) {
      case EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME:
        min_len = DEF_MIN(val_len, (EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN - 1));
        Str_Copy_N(p_user->FirstName, p_str_val, min_len);
        Str_Copy(p_user->FirstName + min_len, "\0");
        break;

      case EX_HTTP_SERVER_REST_USER_FIELD_LAST_NAME:
        min_len = DEF_MIN(val_len, (EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN - 1));
        Str_Copy_N(p_user->LastName, p_str_val, min_len);
        Str_Copy(p_user->LastName + min_len, "\0");
        break;

      case EX_HTTP_SERVER_REST_USER_FIELD_GENDER:
      case EX_HTTP_SERVER_REST_USER_FIELD_AGE:
      case EX_HTTP_SERVER_REST_USER_FIELD_JOB:
      default:
        break;
    }

    *p_buf_len_used += data_rd_len;

    field_type++;
  }

  Str_Copy_N(p_user->Link, p_uri->ParsedURI.PathPtr, EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN);
  Str_Cat_N(p_user->Link, "/", EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN);
  str_len = Str_Len_N(p_user->Link, EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN);
  char_nbr = Ex_HTTP_Server_REST_GetStrLenOfIntDec(p_user->ID);
  Str_FmtNbr_Int32U(p_user->ID, char_nbr, 10, '\0', DEF_NO, DEF_YES, p_user->Link + str_len);

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                Ex_HTTP_Server_REST_JSON_ParseUserInfo()
 *
 * @brief  Parse JSON data to retrieve the user new info.
 *
 * @param  p_app_data      Pointer to application data block.
 *
 * @param  p_user          Pointer to user object.
 *
 * @param  p_buf           Pointer to the connection buffer with the received data.
 *
 * @param  data_len        Length of the data received inside the buffer.
 *
 * @param  buf_len_max     Maximum size of the connection buffer.
 *
 * @param  p_buf_len_used  Variable that will received the length of the data read.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_ParseUserInfo(EX_HTTP_SERVER_REST_DATA *p_app_data,
                                                                    EX_HTTP_SERVER_REST_USER *p_user,
                                                                    void                     *p_buf,
                                                                    const CPU_SIZE_T         data_len,
                                                                    const CPU_SIZE_T         buf_len_max,
                                                                    CPU_SIZE_T               *p_buf_len_used)
{
  CPU_CHAR                       *p_str_val;
  EX_HTTP_SERVER_REST_USER_FIELD field_type;
  HTTPs_REST_HOOK_STATE          state;
  CPU_SIZE_T                     val_len;
  CPU_SIZE_T                     min_len;
  CPU_SIZE_T                     data_rd_len = 0;

  *p_buf_len_used = 0;
  //                                                               ----- PARSE RECEIVED JSON TO RETRIEVE USER INFO ----
  field_type = p_app_data->FieldType;
  while (field_type <= EX_HTTP_SERVER_REST_USER_FIELD_JOB) {
    state = Ex_HTTP_Server_REST_JSON_ParseField(field_type,
                                                &p_str_val,
                                                &val_len,
                                                p_buf,
                                                data_len,
                                                buf_len_max,
                                                &data_rd_len);
    if (state != HTTPs_REST_HOOK_STATE_CONTINUE) {
      p_app_data->FieldType = field_type;
      return (state);
    }

    switch (field_type) {
      case EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME:
        min_len = DEF_MIN(val_len, (EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN - 1));
        Str_Copy_N(p_user->FirstName, p_str_val, min_len);
        Str_Copy(p_user->FirstName + min_len, "\0");
        break;

      case EX_HTTP_SERVER_REST_USER_FIELD_LAST_NAME:
        min_len = DEF_MIN(val_len, (EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN - 1));
        Str_Copy_N(p_user->LastName, p_str_val, min_len);
        Str_Copy(p_user->LastName + min_len, "\0");
        break;

      case EX_HTTP_SERVER_REST_USER_FIELD_GENDER:
        min_len = DEF_MIN(val_len, (EX_HTTP_SERVER_REST_GENDER_STR_MAX_LEN - 1));
        Str_Copy_N(p_user->Gender, p_str_val, min_len);
        Str_Copy(p_user->Gender + min_len, "\0");
        break;

      case EX_HTTP_SERVER_REST_USER_FIELD_AGE:
        p_user->Age = Str_ParseNbr_Int32U(p_str_val, DEF_NULL, 10);
        break;

      case EX_HTTP_SERVER_REST_USER_FIELD_JOB:
        min_len = DEF_MIN(val_len, (EX_HTTP_SERVER_REST_JOB_TITLE_STR_MAX_LEN - 1));
        Str_Copy_N(p_user->JobTitle, p_str_val, min_len);
        Str_Copy(p_user->JobTitle + min_len, "\0");
        break;

      default:
        break;
    }

    *p_buf_len_used = data_rd_len;
    field_type++;
  }

  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                  Ex_HTTP_Server_REST_JSON_ParseField()
 *
 * @brief  Parse a JSON to retrieve the value of a given user field.
 *
 * @param  field_type      User field type to look for in the JSON:
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_ID
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_LINK
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_lAST_NAME
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_GENDER
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_AGE
 *                             - EX_HTTP_SERVER_REST_USER_FIELD_JOB
 *
 * @param  p_val_found     Variable that will received the pointer to the value found.
 *
 * @param  p_val_len       Length of the value found.
 *
 * @param  p_buf           Pointer to the connection buffer with the received data.
 *
 * @param  data_len        Length of the data received inside the buffer.
 *
 * @param  buf_len_max     Maximum size of the connection buffer.
 *
 * @param  p_buf_len_used  Variable that will received the length of the data read.
 *
 * @return  REST hook state:
 *              - HTTPs_REST_HOOK_STATE_CONTINUE, to continue the transaction processing.
 *              - HTTPs_REST_HOOK_STATE_STAY,     to fall back at the same stage the next time the hook is called.
 *              - HTTPs_REST_HOOK_STATE_ERROR,    when an error occurred in the hook processing.
 *******************************************************************************************************/
static HTTPs_REST_HOOK_STATE Ex_HTTP_Server_REST_JSON_ParseField(EX_HTTP_SERVER_REST_USER_FIELD field_type,
                                                                 CPU_CHAR                       **p_val_found,
                                                                 CPU_SIZE_T                     *p_val_len,
                                                                 void                           *p_buf,
                                                                 const CPU_SIZE_T               data_len,
                                                                 const CPU_SIZE_T               buf_len_max,
                                                                 CPU_SIZE_T                     *p_buf_len_used)
{
  CPU_CHAR   *p_str;
  CPU_CHAR   *p_str_tmp;
  CPU_CHAR   *p_field_name;
  CPU_SIZE_T field_max_len;
  CPU_SIZE_T data_len_max;
  CPU_SIZE_T str_len;

  switch (field_type) {
    case EX_HTTP_SERVER_REST_USER_FIELD_ID:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_ID_STR_NAME;
      field_max_len = 3;
      break;

    case EX_HTTP_SERVER_REST_USER_FIELD_LINK:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_LINK_STR_NAME;
      field_max_len = EX_HTTP_SERVER_REST_LINK_STR_MAX_LEN;
      break;

    case EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_FIRST_NAME_STR_NAME;
      field_max_len = EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN;
      break;

    case EX_HTTP_SERVER_REST_USER_FIELD_LAST_NAME:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_LAST_NAME_STR_NAME;
      field_max_len = EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN;
      break;

    case EX_HTTP_SERVER_REST_USER_FIELD_GENDER:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_GENDER_STR_NAME;
      field_max_len = 7;
      break;

    case EX_HTTP_SERVER_REST_USER_FIELD_AGE:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_AGE_STR_NAME;
      field_max_len = 3;
      break;

    case EX_HTTP_SERVER_REST_USER_FIELD_JOB:
      p_field_name = EX_HTTP_SERVER_REST_JSON_KEY_JOB_STR_NAME;
      field_max_len = EX_HTTP_SERVER_REST_JOB_TITLE_STR_MAX_LEN;
      break;

    default:
      goto exit_error;
  }

  data_len_max = EX_HTTP_SERVER_REST_JSON_FIELD_MAX_CHARS_NBR
                 + sizeof(p_field_name)
                 + field_max_len;

  if (data_len_max > buf_len_max) {
    goto exit_error;
  }

  p_str = p_buf;
  str_len = Str_Len_N(p_str, data_len);

  //                                                               Found field key.
  p_str_tmp = Str_Str_N(p_str, p_field_name, str_len);
  if (p_str_tmp == DEF_NULL) {
    goto exit_stay;
  }
  str_len -= p_str_tmp - p_str;
  p_str = p_str_tmp;
  //                                                               Found " char to found end of field key.
  p_str_tmp = Str_Char_N(p_str, str_len, ASCII_CHAR_QUOTATION_MARK);
  if (p_str_tmp == DEF_NULL) {
    goto exit_stay;
  }
  str_len -= p_str_tmp - p_str;
  if (str_len <= 0) {
    goto exit_stay;
  }
  p_str_tmp++;
  str_len--;
  p_str = p_str_tmp;
  //                                                               Found " char to found start of field value.
  p_str_tmp = Str_Char_N(p_str, str_len, ASCII_CHAR_QUOTATION_MARK);
  if (p_str_tmp == DEF_NULL) {
    goto exit_stay;
  }
  str_len -= p_str_tmp - p_str;
  if (str_len <= 0) {
    goto exit_stay;
  }
  p_str_tmp++;
  str_len--;
  p_str = p_str_tmp;
  //                                                               Found " char to found end of field value.
  p_str_tmp = Str_Char_N(p_str, str_len, ASCII_CHAR_QUOTATION_MARK);
  if (p_str_tmp == DEF_NULL) {
    goto exit_stay;
  }
  str_len -= p_str_tmp - p_str;

  *p_val_found = p_str;
  *p_val_len = p_str_tmp - p_str;

  goto exit_continue;

exit_error:
  return (HTTPs_REST_HOOK_STATE_ERROR);

exit_stay:
  *p_buf_len_used = 0;
  return (HTTPs_REST_HOOK_STATE_STAY);

exit_continue:
  *p_buf_len_used = data_len - str_len;
  return (HTTPs_REST_HOOK_STATE_CONTINUE);
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_REST_FindUser()
 *
 * @brief  Find an already existing user in the user list based on the given user ID.
 *
 * @param  user_id  User ID of the user to look for.
 *
 * @return  Pointer to the user object found.
 *          DEF_NULL, if no user is found.
 *******************************************************************************************************/
static EX_HTTP_SERVER_REST_USER *Ex_HTTP_Server_REST_FindUser(CPU_INT08U user_id)
{
  EX_HTTP_SERVER_REST_USER *p_user;

  p_user = Ex_HTTP_Server_REST_UserFirstPtr;
  while (p_user != DEF_NULL) {
    if (p_user->ID == user_id) {
      break;
    }
    p_user = p_user->NextPtr;
  }

  return (p_user);
}

/****************************************************************************************************//**
 *                                      Ex_HTTP_Server_REST_GetUser()
 *
 * @brief  Get a user object from the user pool and add it to the user list.
 *
 * @param  none.
 *
 * @return  Pointer to the user object retrieved.
 *          DEF_NULL, if no block available or in case of an error.
 *******************************************************************************************************/
static EX_HTTP_SERVER_REST_USER *Ex_HTTP_Server_REST_GetUser(void)
{
  EX_HTTP_SERVER_REST_USER *p_user;
  RTOS_ERR                 err_local;

  p_user = (EX_HTTP_SERVER_REST_USER *)Mem_DynPoolBlkGet(&Ex_HTTP_Server_REST_UserPool,
                                                         &err_local);
  if (p_user == DEF_NULL) {
    return (DEF_NULL);
  }

  if (Ex_HTTP_Server_REST_UserFirstPtr == DEF_NULL) {
    p_user->PrevPtr = DEF_NULL;
    p_user->NextPtr = DEF_NULL;
    Ex_HTTP_Server_REST_UserFirstPtr = p_user;
    Ex_HTTP_Server_REST_UserLastPtr = p_user;
  } else {
    p_user->NextPtr = DEF_NULL;
    p_user->PrevPtr = Ex_HTTP_Server_REST_UserLastPtr;
    Ex_HTTP_Server_REST_UserLastPtr->NextPtr = p_user;
    Ex_HTTP_Server_REST_UserLastPtr = p_user;
  }

  p_user->ID = Ex_HTTP_Server_REST_GetUserID();
  p_user->Age = 0;
  Str_Copy_N(p_user->FirstName, "", EX_HTTP_SERVER_REST_FIRST_NAME_STR_MAX_LEN);
  Str_Copy_N(p_user->LastName, "", EX_HTTP_SERVER_REST_LAST_NAME_STR_MAX_LEN);
  Str_Copy_N(p_user->Gender, "", EX_HTTP_SERVER_REST_GENDER_STR_MAX_LEN);
  Str_Copy_N(p_user->JobTitle, "", EX_HTTP_SERVER_REST_JOB_TITLE_STR_MAX_LEN);
  p_user->RdRefCtr = 0;
  p_user->WrRefCtr = 0;

  (void)&err_local;

  return (p_user);
}

/****************************************************************************************************//**
 *                                    Ex_HTTP_Server_REST_RemoveUser()
 *
 * @brief  Remove a user object from the user list and free it to the user pool.
 *
 * @param  p_user  Pointer to the user object to remove and free.
 *******************************************************************************************************/
static void Ex_HTTP_Server_REST_RemoveUser(EX_HTTP_SERVER_REST_USER *p_user)
{
  RTOS_ERR err_local;

  if (Ex_HTTP_Server_REST_UserFirstPtr == DEF_NULL) {
    return;
  }

  if (p_user == Ex_HTTP_Server_REST_UserFirstPtr) {
    if (p_user->NextPtr != DEF_NULL) {
      Ex_HTTP_Server_REST_UserFirstPtr = p_user->NextPtr;
      Ex_HTTP_Server_REST_UserFirstPtr->PrevPtr = DEF_NULL;
      p_user->NextPtr = DEF_NULL;
    } else {
      Ex_HTTP_Server_REST_UserFirstPtr = DEF_NULL;
      Ex_HTTP_Server_REST_UserLastPtr = DEF_NULL;
    }
  } else if (p_user == Ex_HTTP_Server_REST_UserLastPtr) {
    Ex_HTTP_Server_REST_UserLastPtr = p_user->PrevPtr;
    Ex_HTTP_Server_REST_UserLastPtr->NextPtr = DEF_NULL;
    p_user->PrevPtr = DEF_NULL;
  } else {
    (p_user->PrevPtr)->NextPtr = p_user->NextPtr;
    (p_user->NextPtr)->PrevPtr = p_user->PrevPtr;
    p_user->NextPtr = DEF_NULL;
    p_user->PrevPtr = DEF_NULL;
  }

  (void)Ex_HTTP_Server_REST_GetFreeUserID(p_user->ID);

  Mem_DynPoolBlkFree(&Ex_HTTP_Server_REST_UserPool,
                     p_user,
                     &err_local);

  (void)&err_local;
}

/****************************************************************************************************//**
 *                                     Ex_HTTP_Server_REST_GetUserID()
 *
 * @brief  Get a new user ID from the free user ID list or create a new ID.
 *
 * @param  None.
 *
 * @return  The new user ID number.
 *******************************************************************************************************/
static CPU_INT08U Ex_HTTP_Server_REST_GetUserID(void)
{
  CPU_INT08U user_id;

  if (Ex_HTTP_Server_REST_FreeUserID_ListPtr != DEF_NULL) {
    user_id = Ex_HTTP_Server_REST_FreeUserID_ListPtr->ID;
    Ex_HTTP_Server_REST_RemoveFreeUserID();
  } else {
    user_id = Ex_HTTP_Server_REST_UserCtr + 1u;
  }

  return (user_id);
}

/****************************************************************************************************//**
 *                                   Ex_HTTP_Server_REST_GetFreeUserID()
 *
 * @brief  Get from the pool a free user ID object.
 *
 * @param  user_id  User ID number.
 *
 * @return  Pointer to the free user ID object.
 *******************************************************************************************************/
static EX_HTTP_SERVER_REST_FREE_USER_ID *Ex_HTTP_Server_REST_GetFreeUserID(CPU_INT08U user_id)
{
  EX_HTTP_SERVER_REST_FREE_USER_ID *p_free_user_id;
  RTOS_ERR                         err_local;

  p_free_user_id = (EX_HTTP_SERVER_REST_FREE_USER_ID *)Mem_DynPoolBlkGet(&Ex_HTTP_Server_REST_FreeUserID_Pool,
                                                                         &err_local);
  if (p_free_user_id == DEF_NULL) {
    return (DEF_NULL);
  }

  p_free_user_id->ID = user_id;

  if (Ex_HTTP_Server_REST_FreeUserID_ListPtr == DEF_NULL) {
    Ex_HTTP_Server_REST_FreeUserID_ListPtr = p_free_user_id;
    p_free_user_id->NextPtr = DEF_NULL;
  } else {
    p_free_user_id->NextPtr = Ex_HTTP_Server_REST_FreeUserID_ListPtr;
    Ex_HTTP_Server_REST_FreeUserID_ListPtr = p_free_user_id;
  }

  (void)&err_local;

  return (p_free_user_id);
}

/****************************************************************************************************//**
 *                                 Ex_HTTP_Server_REST_RemoveFreeUserID()
 *
 * @brief  Remove from the free user ID list a block to put it back in the pool.
 *******************************************************************************************************/
static void Ex_HTTP_Server_REST_RemoveFreeUserID(void)
{
  EX_HTTP_SERVER_REST_FREE_USER_ID *p_free_user_id;
  RTOS_ERR                         err_local;

  p_free_user_id = Ex_HTTP_Server_REST_FreeUserID_ListPtr;
  Ex_HTTP_Server_REST_FreeUserID_ListPtr = p_free_user_id->NextPtr;
  p_free_user_id->NextPtr = DEF_NULL;

  Mem_DynPoolBlkFree(&Ex_HTTP_Server_REST_FreeUserID_Pool,
                     p_free_user_id,
                     &err_local);

  (void)&err_local;
}

/****************************************************************************************************//**
 *                                    Ex_HTTP_Server_REST_GetDataBlk()
 *
 * @brief  Get an application data block from the pool.
 *
 * @return  Pointer to the retrieved application data block.
 *          DEF_NULL if no block available or in case of an error.
 *******************************************************************************************************/
static EX_HTTP_SERVER_REST_DATA *Ex_HTTP_Server_REST_GetDataBlk(void)
{
  EX_HTTP_SERVER_REST_DATA *p_app_data;
  RTOS_ERR                 err_local;

  p_app_data = (EX_HTTP_SERVER_REST_DATA *)Mem_DynPoolBlkGet(&Ex_HTTP_Server_REST_DataPool,
                                                             &err_local);
  if (p_app_data == DEF_NULL) {
    return (DEF_NULL);
  }

  if (Ex_HTTP_Server_REST_DataListPtr == DEF_NULL) {
    Ex_HTTP_Server_REST_DataListPtr = p_app_data;
    p_app_data->NextPtr = DEF_NULL;
  } else {
    p_app_data->NextPtr = Ex_HTTP_Server_REST_DataListPtr;
    Ex_HTTP_Server_REST_DataListPtr = p_app_data;
  }

  p_app_data->UserPtr = DEF_NULL;
  p_app_data->FieldType = EX_HTTP_SERVER_REST_USER_FIELD_FIRST_NAME;

  (void)&err_local;

  return (p_app_data);
}

/****************************************************************************************************//**
 *                                   Ex_HTTP_Server_REST_RemoveDataBlk()
 *
 * @brief  Remove an application data block from the list and put it back in the free pool.
 *
 * @param  p_app_data  Pointer to the application data block to free.
 *******************************************************************************************************/
static void Ex_HTTP_Server_REST_RemoveDataBlk(EX_HTTP_SERVER_REST_DATA *p_app_data)
{
  EX_HTTP_SERVER_REST_DATA *p_item;
  EX_HTTP_SERVER_REST_DATA *p_item_prev;
  RTOS_ERR                 err_local;

  p_item = Ex_HTTP_Server_REST_DataListPtr;
  p_item_prev = DEF_NULL;
  while (p_item != DEF_NULL) {
    if (p_app_data == Ex_HTTP_Server_REST_DataListPtr) {
      Ex_HTTP_Server_REST_DataListPtr = p_app_data->NextPtr;
    }

    if (p_item == p_app_data) {
      if (p_item_prev != DEF_NULL) {
        p_item_prev->NextPtr = p_item->NextPtr;
      }
      p_item->NextPtr = DEF_NULL;
      break;
    }

    p_item_prev = p_item;
    p_item = p_item->NextPtr;
  }

  Mem_DynPoolBlkFree(&Ex_HTTP_Server_REST_DataPool,
                     p_app_data,
                     &err_local);

  (void)&err_local;
}

/****************************************************************************************************//**
 *                                 Ex_HTTP_Server_REST_GetStrLenOfIntDec()
 *
 * @brief  Get the number of decimals of a integer number.
 *
 * @param  i  Integer number.
 *
 * @return  Number of decimals.
 *******************************************************************************************************/
static CPU_SIZE_T Ex_HTTP_Server_REST_GetStrLenOfIntDec(CPU_INT32U i)
{
  if (i < 10u) {
    return 1u;
  } else if (i < 100u) {
    return 2u;
  } else if (i < 1000u) {
    return 3u;
  } else if (i < 10000u) {
    return 4u;
  } else if (i < 100000u) {
    return 5u;
  } else if (i < 1000000u) {
    return 6u;
  } else if (i < 10000000u) {
    return 7u;
  } else if (i < 100000000u) {
    return 8u;
  } else {
    return 9u;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL && RTOS_MODULE_FS_AVAIL
