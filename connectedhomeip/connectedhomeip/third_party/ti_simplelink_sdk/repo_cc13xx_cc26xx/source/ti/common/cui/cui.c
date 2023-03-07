/******************************************************************************

 @file CUI.c

 @brief This file contains the interface implementation of the Combined User
         Interface.

 @detail The interface is designed to be shared between clients.
         As such a client can request access to resources whether they be
         Buttons, LEDs or UART Display without the fear that another client
         already has ownership over that resource.

         If a resource is already taken by another client then the interface
         will respond with that information.

         Only a client that has been given access to a resource may utilize
         the resource. Therefore, any calls a client makes to read/write a
         resource will be ignored if the client does not have the access
         required.

 Group: LPRF SW RND
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
 
/******************************************************************************
 Includes
 *****************************************************************************/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#ifndef CUI_POSIX
#include <ti/sysbios/BIOS.h>
#endif
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SystemP.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/utils/Random.h>
#include <ti/drivers/apps/LED.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include "ti_drivers_config.h"

#include "cui.h"


#define CUI_INITIAL_STATUS_OFFSET 5
#define CUI_LABEL_VAL_SEP ": "
#define CUI_MAX_LABEL_AND_SEP_LEN (MAX_STATUS_LINE_LABEL_LEN + (sizeof(CUI_LABEL_VAL_SEP)))

/*
 * Ascii Escape characters to be used by testing scripts to bookend the
 * information being printed to the UART
 */
#define CUI_MENU_START_CHAR         0x01 // SOH (start of heading) ascii character
#define CUI_STATUS_LINE_START_CHAR  0x02 // SOT (start of text) ascii character
#define CUI_END_CHAR                0x03 // ETX (end of text) ascii character

#define CUI_NL_CR               "\n\r" // New line carriage return

#define CUI_ESC_UP              "\033[A"
#define CUI_ESC_DOWN            "\033[B"
#define CUI_ESC_RIGHT           "\033[C"
#define CUI_ESC_LEFT            "\033[D"
#define CUI_ESC_ESC             "\033\0\0\0\0"

/*
 * Escape sequences for terminal control.
 * Any sequences with '%' in them require require additional information to be used
 *  as is.
 */
#define CUI_ESC_TRM_MODE            "\033[20"    // Set line feed mode for the terminal

#define CUI_ESC_CLR                 "\033[2J"    // Clear the entire screen
#define CUI_ESC_CLR_UP              "\033[1J"    // Clear screen from cursor up
#define CUI_ESC_CLR_STAT_LINE_VAL   "\033[2K"    // Clear the status line

#define CUI_ESC_CUR_HIDE            "\033[?25l"  // Hide cursor
#define CUI_ESC_CUR_SHOW            "\033[?25h"  // Show cursor
#define CUI_ESC_CUR_HOME            "\033[H"     // Move cursor to the top left of the terminal
#define CUI_ESC_CUR_MENU_BTM        "\033[3;%dH" // Move cursor to the bottom right of the menu
#define CUI_ESC_CUR_LINE            "\033[%d;0H" // Move cursor to a line of choice
#define CUI_ESC_CUR_ROW_COL         "\033[%d;%dH"// Move cursor to row and col

#define CUI_NUM_UART_CHARS          5
#define CUI_MENU_START_ESCAPE_LEN   32
#define CUI_NL_CR_LEN               2
#define CUI_ETX_LEN                 1

/******************************************************************************
 Constants
 *****************************************************************************/
typedef enum
{
    CUI_RELEASED = 0,
    // Some specific value so that uninitialized memory does not cause problems
    CUI_ACQUIRED = 0xDEADBEEF,
} CUI_rscStatus_t;


// Internal representation of a menu
typedef struct
{
    CUI_menu_t* pMenu;
    uint32_t clientHash;
} CUI_menuResource_t;

// Internal representation of a status line
typedef struct
{
    uint32_t clientHash;
    uint32_t lineOffset;
    char label[CUI_MAX_LABEL_AND_SEP_LEN];
    CUI_rscStatus_t status;
    bool refreshInd;
} CUI_statusLineResource_t;

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*
 * [General Global Variables]
 */
static bool gModuleInitialized = false;

static CUI_clientHandle_t gClientHandles[MAX_CLIENTS];
static uint32_t gMaxStatusLines[MAX_CLIENTS];

static SemaphoreP_Params gSemParams;
static SemaphoreP_Handle gClientsSem;
static SemaphoreP_Struct gClientsSemStruct;

/*
 * [UART Specific Global Variables]
 */
static UART2_Handle gUartHandle = NULL;
#ifndef CUI_MIN_FOOTPRINT
static uint8_t gUartRxBuffer[CUI_NUM_UART_CHARS];
#endif

static SemaphoreP_Handle gUartSem;
static SemaphoreP_Struct gUartSemStruct;

static uint8_t gTxBuff[512];
static size_t gTxLen = 0;
static size_t gTxSent = 0;

#ifndef CUI_MIN_FOOTPRINT
/*
 * [Menu Global Variables]
 */
static CUI_menu_t* gpCurrMenu;
static CUI_menu_t* gpMainMenu;
static size_t gCurrMenuItemEntry = 0;
static size_t gPrevMenuItemEntry = 0;
static bool gCursorActive;
static CUI_cursorInfo_t gCursorInfo;

static CUI_menuResource_t gMenuResources[MAX_REGISTERED_MENUS];

static SemaphoreP_Handle gMenuSem;
static SemaphoreP_Struct gMenuSemStruct;

char menuBuff[CUI_MENU_START_ESCAPE_LEN             // Escape characters
              + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN   // Additional new line and return char
              + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN   // Additional new line and return char
              + MAX_MENU_LINE_LEN + CUI_ETX_LEN];   // Additional ETX char

static char gpMultiMenuTitle[] = " TI DMM Application ";

/*
 * This a special menu that is only utilized when 2 or more menus have been
 * registered to the CUI. This menu will then be the top most Main Menu where
 * each menu that was registered will now be a sub menu.
 */
uint8_t     cuiMultiMenuData[sizeof(CUI_menu_t) + ((MAX_REGISTERED_MENUS + 1) * sizeof(CUI_menuItem_t))];
CUI_menu_t *cuiMultiMenu = (CUI_menu_t *)&cuiMultiMenuData;
#endif /* CUI_MIN_FOOTPRINT */

/*
 * [Status Line Variables]
 */
static CUI_statusLineResource_t* gStatusLineResources[MAX_CLIENTS];

static SemaphoreP_Handle gStatusSem;
static SemaphoreP_Struct gStatusSemStruct;

/******************************************************************************
 Local Functions Prototypes
 *****************************************************************************/
static CUI_retVal_t CUI_publicAPIChecks(const CUI_clientHandle_t _clientHandle);
static CUI_retVal_t CUI_acquireStatusLine(const CUI_clientHandle_t _clientHandle, const char* _pLabel, const bool _refreshInd, uint32_t* _pLineId);
static CUI_retVal_t CUI_validateHandle(const CUI_clientHandle_t _clientHandle);
static int CUI_getClientIndex(const CUI_clientHandle_t _clientHandle);
static void UartWriteCallback(UART2_Handle _handle, void *_buf, size_t _size, void *_userArg, int_fast16_t _status);
#ifndef CUI_MIN_FOOTPRINT
static void UartReadCallback(UART2_Handle _handle, void *_buf, size_t _size, void *_userArg, int_fast16_t _status);
#endif
static CUI_retVal_t CUI_updateRemLen(size_t* _currRemLen, char* _buff, size_t _buffSize);
static CUI_retVal_t CUI_writeString(void * _buffer, size_t _size);
#ifndef CUI_MIN_FOOTPRINT
static void CUI_menuActionNavigate(uint8_t _navDir);
static void CUI_menuActionExecute(void);
static void CUI_dispMenu(bool _menuPopulated);
static void CUI_callMenuUartUpdateFn();
static void CUI_updateCursor(void);
static bool CUI_handleMenuIntercept(CUI_menuItem_t* _pItemEntry, uint8_t _input);
static bool CUI_handleMenuList(CUI_menuItem_t* _pItemEntry, uint8_t _input);
static CUI_retVal_t CUI_findMenu(CUI_menu_t* _pMenu, CUI_menu_t* _pDesiredMenu, uint32_t* _pPrevItemIndex);
#endif
/******************************************************************************
 * Public CUI APIs
 *****************************************************************************/
/*********************************************************************
 * @fn          CUI_init
 *
 * @brief       Initialize the CUI module. This function must be called
 *                  before any other CUI functions.
 *
 * @params      _pParams - A pointer to a CUI_params_t struct
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_init(CUI_params_t* _pParams)
{
    /*
     *  Do nothing if the module has already been initialized or if
     *  CUI_init has been called without trying to manage any of the three
     *  resources (btns, leds, uart)
     */
    if (!gModuleInitialized && _pParams->manageUart)
    {
        // Semaphore Setup
        SemaphoreP_Params_init(&gSemParams);
        //set all sems in this module to be binary sems
        gSemParams.mode = SemaphoreP_Mode_BINARY;

        // Client Setup
        {
            gClientsSem = SemaphoreP_construct(&gClientsSemStruct, 1, &gSemParams);

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                /*
                 * A client handle of 0 indicates that the client has not been
                 * registered.
                 */
                gClientHandles[i] = 0U;
            }
        }

        {
            // UART semaphore setup
            gUartSem = SemaphoreP_construct(&gUartSemStruct, 1, &gSemParams);
#ifndef CUI_MIN_FOOTPRINT
            gMenuSem = SemaphoreP_construct(&gMenuSemStruct, 1, &gSemParams);
#endif
            {
                UART2_Params uartParams;

                UART2_Params_init(&uartParams);
                uartParams.baudRate = 115200;
                uartParams.writeMode     = UART2_Mode_CALLBACK;
                uartParams.writeCallback = UartWriteCallback;
#ifndef CUI_MIN_FOOTPRINT
                uartParams.readMode       = UART2_Mode_CALLBACK;
                uartParams.readCallback   = UartReadCallback;
                uartParams.readReturnMode = UART2_ReadReturnMode_PARTIAL;
#endif
                gUartHandle = UART2_open(CONFIG_DISPLAY_UART, &uartParams);
                if (NULL == gUartHandle)
                {
                    return CUI_FAILURE;
                }
                else
                {
#ifndef CUI_MIN_FOOTPRINT
                    // kick off the first read
                    UART2_read(gUartHandle, gUartRxBuffer, sizeof(gUartRxBuffer), NULL);
#endif

                    char clearScreenStr[] = CUI_ESC_CLR CUI_ESC_TRM_MODE CUI_ESC_CUR_HIDE;

                    if (CUI_SUCCESS != CUI_writeString(clearScreenStr, strlen(clearScreenStr)))
                    {
                        UART2_close(gUartHandle);
                        return CUI_FAILURE;
                    }
                }
            }

#ifndef CUI_MIN_FOOTPRINT
            // Multi Menu Initialization
            {
                memset(gMenuResources, 0, sizeof(gMenuResources));
                /*
                 * No additional initialization is needed in the case of a single
                 * menu being registered to the CUI module. In the case of 2 or more
                 * menus being registered the global cuiMultiMenu object will
                 * be used as the top level menu and every registered menu will be a
                 * sub menu of the cuiMultiMenu instead.
                 */
                memset(cuiMultiMenu, 0, sizeof(&cuiMultiMenu));
                cuiMultiMenu->uartUpdateFn  = NULL;
                cuiMultiMenu->pTitle        = gpMultiMenuTitle;
                cuiMultiMenu->numItems      = MAX_REGISTERED_MENUS + 1;
                cuiMultiMenu->pUpper        = NULL;
            }
#endif

            // Status Lines Setup
            {
                gStatusSem = SemaphoreP_construct(&gStatusSemStruct, 1, &gSemParams);
            }
        }

        SemaphoreP_post(gUartSem);
        gModuleInitialized = true;
        return CUI_SUCCESS;
    }

    return CUI_FAILURE;
}

/*********************************************************************
 * @fn          CUI_paramsInit
 *
 * @brief       Initialize a CUI_clientParams_t struct to a known state.
 *                  The known state in this case setting each resource
 *                  management flag to true
 *
 * @params      _pParams - A pointer to an un-initialized CUI_params_t struct
 *
 * @return      none
 */
void CUI_paramsInit(CUI_params_t* _pParams)
{
    _pParams->manageUart = true;
}

/*********************************************************************
 * @fn          CUI_clientOpen
 *
 * @brief       Open a client with the CUI module. A client is required
 *                  to request/acquire resources
 *
 * @param       _pParams - Pointer to a CUI client params struct.
 *                  _pParams.clientName must be set before passing to CUI_open
 *
 * @return      NULL on failure. Otherwise success.
 */
CUI_clientHandle_t CUI_clientOpen(CUI_clientParams_t* _pParams)
{
    static size_t numClients = 0;

    if (!gModuleInitialized)
    {
        return 0U;
    }

    SemaphoreP_pend(gClientsSem, SemaphoreP_WAIT_FOREVER);

    if (numClients >= MAX_CLIENTS)
    {
        return 0U;
    }

    uint32_t randomNumber;
    if (Random_STATUS_SUCCESS != Random_seedAutomatic()) {
         return CUI_FAILURE;
    }

    uint8_t attempts = 0;
    do
    {
        randomNumber = Random_getNumber();
        attempts++;
    }while(!randomNumber && (attempts <= 5));


    gClientHandles[numClients] = randomNumber;

    if (_pParams->maxStatusLines)
    {
        gMaxStatusLines[numClients] = _pParams->maxStatusLines;
        gStatusLineResources[numClients] = malloc(_pParams->maxStatusLines * sizeof(gStatusLineResources[0][0]));
        if (gStatusLineResources[numClients] == NULL)
        {
            return CUI_FAILURE;
        }
        memset(gStatusLineResources[numClients], 0, _pParams->maxStatusLines * sizeof(gStatusLineResources[0][0]));
    }

    numClients++;

    SemaphoreP_post(gClientsSem);

    return randomNumber;
}

/*********************************************************************
 * @fn          CUI_clientParamsInit
 *
 * @brief       Initialize a CUI_clientParams_t struct to a known state.
 *
 * @param       _pClientParams - Pointer to params struct
 *
 * @return      void
 */
void CUI_clientParamsInit(CUI_clientParams_t* _pClientParams)
{
    strcpy(_pClientParams->clientName, "");
    _pClientParams->maxStatusLines = 0;
}

/*********************************************************************
 * @fn          CUI_close
 *
 * @brief       Close the CUI module. Release all resources and memory.
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_close()
{
    // Only close the module if it's been initialized
    if (gModuleInitialized)
    {
        SemaphoreP_pend(gStatusSem, SemaphoreP_WAIT_FOREVER);

        char clearScreenStr[] = CUI_ESC_CLR CUI_ESC_TRM_MODE CUI_ESC_CUR_HIDE;
        CUI_writeString(clearScreenStr, strlen(clearScreenStr));
        for (uint8_t i = 0; i < MAX_CLIENTS; i++)
        {
            if (gStatusLineResources[i])
            {
                free(gStatusLineResources[i]);
            }
        }
        UART2_close(gUartHandle);
        SemaphoreP_post(gStatusSem);
    }

    gModuleInitialized = false;

    return CUI_SUCCESS;

}
#ifndef CUI_MIN_FOOTPRINT
/******************************************************************************
 * Menu CUI APIs
 *****************************************************************************/
/*********************************************************************
 * @fn          CUI_registerMenu
 *
 * @brief       Register a menu with the CUI module
 *
 * @param       _clientHandle - Client to register the menu to
 *              _pMenu - Pointer to a CUI_menu_t struct
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_registerMenu(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu)
{
    CUI_retVal_t retVal = CUI_publicAPIChecks(_clientHandle);
    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    if (NULL == _pMenu)
    {
        return CUI_INVALID_PARAM;
    }

    if (NULL == _pMenu->uartUpdateFn)
    {
        return CUI_MISSING_UART_UPDATE_FN;
    }

    SemaphoreP_pend(gMenuSem, SemaphoreP_WAIT_FOREVER);

    int freeIndex = -1;
    int numMenus = 0;
    for (int i = 0; i < MAX_REGISTERED_MENUS; i++)
    {
        if (!(gMenuResources[i].clientHash) &&
              (NULL == gMenuResources[i].pMenu))
        {
            if (-1 == freeIndex)
            {
                // Find the first empty hole in the array
                freeIndex = i;
            }
        }
        else if (_pMenu == gMenuResources[i].pMenu)
        {
            // Do not allow multiple of the same menu to be registered
            SemaphoreP_post(gMenuSem);
            return CUI_INVALID_PARAM;
        }
        else
        {
            numMenus++;
        }
    }

    if (-1 == freeIndex)
    {
        SemaphoreP_post(gMenuSem);

        return CUI_MAX_MENUS_REACHED;
    }

    gMenuResources[freeIndex].clientHash = _clientHandle;
    gMenuResources[freeIndex].pMenu = _pMenu;

    if (numMenus > 0)
    {
        if (1 == numMenus)
        {
            /*
             * Someone (a rtos task) needs to own the processing time for the
             * cuiMultiMenu. The task that first registered a menu will be
             * that owner. Any additional menu's processing time will be owned
             * by the task that registered them.
             */
            cuiMultiMenu->uartUpdateFn = gpMainMenu->uartUpdateFn;

            /*
             * The first menu that was registered needs to be added as the first
             * sub menu of the cuiMultiMenu object
             */
            cuiMultiMenu->menuItems[0].itemType = CUI_MENU_ITEM_TYPE_SUBMENU;
            cuiMultiMenu->menuItems[0].interceptActive = false;
            cuiMultiMenu->menuItems[0].pDesc = NULL;
            cuiMultiMenu->menuItems[0].item.pSubMenu = gpMainMenu;
            gpMainMenu->pUpper = cuiMultiMenu;

            /*
             * Change the old main menu Help action to a back action
             */
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].itemType = CUI_MENU_ITEM_TYPE_ACTION;
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].interceptActive = false;
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].pDesc = CUI_MENU_ACTION_BACK_DESC;
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].item.pFnAction = (CUI_pFnAction_t) CUI_menuActionBack;
        }

        /*
         * Add the new menu being registered to the cuiMultiMenu as a sub
         * menu object
         */
        cuiMultiMenu->menuItems[numMenus].itemType = CUI_MENU_ITEM_TYPE_SUBMENU;
        cuiMultiMenu->menuItems[numMenus].interceptActive = false;
        cuiMultiMenu->menuItems[numMenus].pDesc = NULL;
        cuiMultiMenu->menuItems[numMenus].item.pSubMenu = _pMenu;
        _pMenu->pUpper = cuiMultiMenu;

        /*
         * Change the registering menu Help action to a back action
         */
        _pMenu->menuItems[_pMenu->numItems - 1].itemType = CUI_MENU_ITEM_TYPE_ACTION;
        _pMenu->menuItems[_pMenu->numItems - 1].interceptActive = false;
        _pMenu->menuItems[_pMenu->numItems - 1].pDesc = CUI_MENU_ACTION_BACK_DESC;
        _pMenu->menuItems[_pMenu->numItems - 1].item.pFnAction = (CUI_pFnAction_t) CUI_menuActionBack;

        /*
         * The Help screen must always be the last initialized item in the
         * cuiMultiMenu
         */
        cuiMultiMenu->menuItems[numMenus + 1].itemType = CUI_MENU_ITEM_TYPE_INTERCEPT;
        cuiMultiMenu->menuItems[numMenus + 1].interceptActive = false;
        cuiMultiMenu->menuItems[numMenus + 1].pDesc = CUI_MENU_ACTION_HELP_DESC;
        cuiMultiMenu->menuItems[numMenus + 1].item.pFnIntercept = (CUI_pFnIntercept_t) CUI_menuActionHelp;

        if (1 == numMenus)
        {
            /*
             * At this point there should be 3 items.
             * [previous menu]
             * [new menu]
             * [help action]
             */
            cuiMultiMenu->numItems = 3;
        }
        else
        {
            /*
             * At this point there should be one more item.
             * [previous menu]
             *  ...
             * [new menu]
             * [help action]
             */
            cuiMultiMenu->numItems++;
        }

        /*
         * The global menu pointers need to be modified
         * to reflect the new menu structure.
         */
        gpMainMenu = cuiMultiMenu;
        gpCurrMenu = cuiMultiMenu;
    }
    else
    {
        /*
         * Set global pointers to the new main menu
         */
        gpMainMenu = _pMenu;
        gpCurrMenu = _pMenu;
    }

    /* Default to the Help item that was given to it */
    gCurrMenuItemEntry = gpMainMenu->numItems - 1;

    CUI_dispMenu(false);
    SemaphoreP_post(gMenuSem);

    return CUI_SUCCESS;
}

/*********************************************************************
 * @fn          CUI_deRegisterMenu
 *
 * @brief       De-registers a menu with the CUI module
 *
 * @param       _clientHandle - Client that owns the menu
 *              _pMenu - Pointer to the CUI_menu_t struct to remove
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_deRegisterMenu(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu)
{
    char buff[32];
    CUI_retVal_t retVal = CUI_publicAPIChecks(_clientHandle);

    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    if (NULL == _pMenu)
    {
        return CUI_INVALID_PARAM;
    }

    if (NULL == _pMenu->uartUpdateFn)
    {
        return CUI_MISSING_UART_UPDATE_FN;
    }

    SemaphoreP_pend(gMenuSem, SemaphoreP_WAIT_FOREVER);

    int matchingIndex = -1;
    int numMenus = 0;
    for (int i = 0; i < MAX_REGISTERED_MENUS; i++)
    {
        if ((_clientHandle == gMenuResources[i].clientHash) &&
              (_pMenu == gMenuResources[i].pMenu))
        {
            if (-1 == matchingIndex)
            {
                matchingIndex = i;
            }
        }
        if ((0U != gMenuResources[i].clientHash) &&
              (NULL != gMenuResources[i].pMenu))
        {
            numMenus++;
        }
    }

    if (-1 == matchingIndex)
    {
        SemaphoreP_post(gMenuSem);

        return CUI_RESOURCE_NOT_ACQUIRED;
    }

    if (numMenus > 1)
    {
        /*
         * Reduce the number of menus in the multi menu by 1
         */
        if (3 == cuiMultiMenu->numItems)
        {
            /*
             * We should go back to a single menu. Remove the multi Menu.
             * There will only be one other valid menu in the array. Find it,
             * and use that as the single main menu.
             */
            uint8_t newMainMenuIndex = 0;
            for (int i = 0; i < MAX_REGISTERED_MENUS; i++)
            {
                if (0U != gMenuResources[i].clientHash &&
                        NULL != gMenuResources[i].pMenu &&
                        i != matchingIndex)
                {
                    newMainMenuIndex = i;
                    break;
                }
            }

            gpMainMenu = gMenuResources[newMainMenuIndex].pMenu;
            gpCurrMenu = gpMainMenu;

            cuiMultiMenu->numItems = 0;

            /* Default to the Help item that was given to it */
            gCurrMenuItemEntry = gpMainMenu->numItems - 1;

            gpMainMenu->menuItems[gpMainMenu->numItems - 1].itemType = CUI_MENU_ITEM_TYPE_INTERCEPT;
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].interceptActive = false;
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].pDesc = CUI_MENU_ACTION_HELP_DESC;
            gpMainMenu->menuItems[gpMainMenu->numItems - 1].item.pFnIntercept = (CUI_pFnIntercept_t) CUI_menuActionHelp;
        }
        else
        {
            /*
             * Locate where _pMenu(The menu to be removed) lies within the
             * cuiMultiMenu[] array.
             */
            int multiMenuIndex = 0;
            for (int i = 0; i < MAX_REGISTERED_MENUS; i++)
            {
                if (cuiMultiMenu->menuItems[i].item.pSubMenu == _pMenu)
                {
                    multiMenuIndex = i;
                    break;
                }
            }

            /*
             * Shift the remaining items in the cuiMultiMenu down to cover the
             * menu that is being de-registered.
             */
            for (int i = multiMenuIndex; i < MAX_REGISTERED_MENUS; i++)
            {
                /*
                 *  It is safe to use this i+1 value because cuiMultiMenu was
                 * declared to contain MAX_REGISTERED_MENUS + 1 menuItems.
                 */
                memcpy(&(cuiMultiMenu->menuItems[i]), &(cuiMultiMenu->menuItems[i+1]), sizeof(cuiMultiMenu->menuItems[0]));
            }

            // Decrement the count of items in cuiMultiMenu
            cuiMultiMenu->numItems--;

            if (gpCurrMenu == _pMenu)
            {
                /*
                 * If the menu being removed was the one currently being
                 * displayed, then we need to choose a new thing to display.
                 * The easiest solution here is to display the Help action.
                 */
                gpCurrMenu = cuiMultiMenu;
                gCurrMenuItemEntry = cuiMultiMenu->numItems;
            }
            else if (gpCurrMenu == cuiMultiMenu)
            {
                /*
                 * If the menu being removed was not currently being displayed,
                 * and we were currently looking at an option in cuiMultiMenu,
                 * then it's safest just to go back to the Help screen.
                 */
                gCurrMenuItemEntry = cuiMultiMenu->numItems - 1;
            }
            else
            {
                /*
                 * If the currently displayed menu had nothing to do with this
                 * operation, it is safe to leave the display where it is.
                 * Do not update gpCurrMenu or gCurrMenuItemEntry.
                 */
            }
        }

        CUI_dispMenu(false);

    }
    else
    {
        gpMainMenu = NULL;
        gpCurrMenu = NULL;

        /* Default to the Help item that was given to it */
        gCurrMenuItemEntry = 0;

        SystemP_snprintf(buff, sizeof(buff),
            CUI_ESC_CUR_HIDE CUI_ESC_CUR_MENU_BTM CUI_ESC_CLR_UP CUI_ESC_CUR_HOME,
            MAX_MENU_LINE_LEN);
        CUI_writeString(buff, strlen(buff));
    }

    gMenuResources[matchingIndex].clientHash = 0U;
    gMenuResources[matchingIndex].pMenu = NULL;

    SemaphoreP_post(gMenuSem);

    return CUI_SUCCESS;
}


/*********************************************************************
 * @fn          CUI_updateMultiMenuTitle
 *
 * @brief       De-registers a menu with the CUI module
 *
 * @param       _pTitle - Pointer to the new multi-menu title
 *
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_updateMultiMenuTitle(const char* _pTitle)
{
    if (NULL == _pTitle)
    {
        return CUI_INVALID_PARAM;
    }

    cuiMultiMenu->pTitle = _pTitle;

    /*
     * Display the updated title if the top level menu is already
     *  being shown.
     */
    if (gpCurrMenu == cuiMultiMenu)
    {
        CUI_dispMenu(false);
    }

    return CUI_SUCCESS;
}

/*********************************************************************
 * @fn          CUI_menuNav
 *
 * @brief       Navigate to a specific entry of a menu that has already been
 *              registered
 *
 * @param       _clientHandle - Client that owns the menu
 *              _pMenu - Pointer to an already registered menu
 *              _itemIndex - The index of the menuItems[] array to select from
 *                  the menu
 *
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_menuNav(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu, const uint32_t _itemIndex)
{
    CUI_retVal_t retVal = CUI_publicAPIChecks(_clientHandle);
    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    if (NULL == _pMenu)
    {
        return CUI_INVALID_PARAM;
    }

    if (_itemIndex > _pMenu->numItems - 1)
    {
        return CUI_INVALID_PARAM;
    }

    CUI_retVal_t menuRetVal;
    uint32_t prevItemIndex = 0;
    for (int i = 0; i < MAX_REGISTERED_MENUS; i++)
    {
        prevItemIndex = i;
        /*
         * Verify that the menu is apart of a registered Main Menu
         */
        menuRetVal = CUI_findMenu(gMenuResources[i].pMenu, _pMenu, &prevItemIndex);
        if (CUI_SUCCESS == menuRetVal)
        {
            /*
             * Make sure that the client Attempting to navigate to this menu
             * is the owner of the menu.
             */
            if (gMenuResources[i].clientHash != _clientHandle)
            {
                return CUI_INVALID_CLIENT_HANDLE;
            }
            break;
        }
    }

    if (menuRetVal)
    {
        return CUI_INVALID_PARAM;
    }

    /*
     * If the menu is found to be already registered, then it is safe to nav
     * there. It is guaranteed that the user can navigate away afterwards
     */
    gPrevMenuItemEntry = prevItemIndex;
    gCurrMenuItemEntry = _itemIndex;
    gpCurrMenu = _pMenu;

    CUI_dispMenu(false);

    return CUI_SUCCESS;
}

/*********************************************************************
 * @fn          CUI_processMenuUpdate
 *
 * @brief       This function should be called whenever there is UART input
 *                  to be processed.
 *
 *              This update process begins by the CUI module calling the
 *                  CUI_pFnClientUartUpdate_t of the main menu that was
 *                  registered. At that point the CUI_pFnClientUartUpdate_t
 *                  function is responsible for calling CUI_processUartUpdate.
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_processMenuUpdate(void)
{
    if (!gModuleInitialized)
    {
        return CUI_FAILURE;
    }

    CUI_menuItem_t* pItemEntry = &(gpCurrMenu->menuItems[gCurrMenuItemEntry]);
    uint8_t input = gUartRxBuffer[0];
    bool inputBad = false;

    // Decode special escape sequences
    if (input == CUI_INPUT_ESC)
    {
        /*
         * If the first character is CUI_INPUT_ESC, then look
         *  for the accepted sequences.
         */
        if (memcmp(gUartRxBuffer, CUI_ESC_UP, sizeof(CUI_ESC_UP)) == 0)
        {
            input = CUI_INPUT_UP;
        }
        else if (memcmp(gUartRxBuffer, CUI_ESC_DOWN, sizeof(CUI_ESC_DOWN)) == 0)
        {
            input = CUI_INPUT_DOWN;
        }
        else if (memcmp(gUartRxBuffer, CUI_ESC_RIGHT, sizeof(CUI_ESC_RIGHT)) == 0)
        {
            input = CUI_INPUT_RIGHT;
        }
        else if (memcmp(gUartRxBuffer, CUI_ESC_LEFT, sizeof(CUI_ESC_LEFT)) == 0)
        {
            input = CUI_INPUT_LEFT;
        }
        else if (memcmp(gUartRxBuffer, CUI_ESC_ESC, sizeof(gUartRxBuffer)))
        {
            // The rx buffer is full of junk. Let's ignore it just in case.
            inputBad = true;
        }
    }

    if (!inputBad)
    {
        // If it is an upper case letter, convert to lowercase
        if (input >= 'A' && input <= 'Z')
        {
            input += 32; // converts any uppercase letter to a lowercase letter
        }
        else
        {
            /*
             * Assume that further input is intended to be handled by an
             * interceptable action
             */
        }

        bool interceptState = pItemEntry->interceptActive;
        bool updateHandled = false;

        /*
         *  Allow the interceptable action, if it is being shown, the chance to
         *  handle the uart input and display output if necessary.
         */
        if (pItemEntry->itemType == CUI_MENU_ITEM_TYPE_INTERCEPT)
        {
            updateHandled = CUI_handleMenuIntercept(pItemEntry, input);
        }
        else if (pItemEntry->itemType == CUI_MENU_ITEM_TYPE_LIST)
        {
            /*
             *  Allow the list action the chance to handle the uart input
             *  and display output if necessary.
             */
            updateHandled = CUI_handleMenuList(pItemEntry, input);

            if (interceptState != pItemEntry->interceptActive)
            {
                /*
                 * If the interceptState has changed, it means the
                 * user has finished with the list.
                 */
                input = CUI_INPUT_ESC;
            }
        }

        if (false == updateHandled)
        {
            switch(input)
            {
                // Up and Down have no effect on menu navigation
                case CUI_INPUT_UP:
                case CUI_INPUT_DOWN:
                    break;
                // Left and Right navigate within a menu
                case CUI_INPUT_RIGHT:
                case CUI_INPUT_LEFT:
                    CUI_menuActionNavigate(input);
                    break;
                case CUI_INPUT_EXECUTE:
                    CUI_menuActionExecute();
                    break;
                case CUI_INPUT_BACK:
                    // if there is a upper menu, navigate to it.
                    if (gpCurrMenu->pUpper)
                    {
                        gpCurrMenu = gpCurrMenu->pUpper;
                        gCurrMenuItemEntry = gPrevMenuItemEntry;
                    }
                    else
                    {
                        // We are already at the main menu.
                        // go back to the help screen
                        gCurrMenuItemEntry = gpCurrMenu->numItems - 1;
                    }
                    CUI_dispMenu(false);
                    break;
                case CUI_INPUT_ESC:
                    if (interceptState && !pItemEntry->interceptActive)
                    {
                        /*
                         * Nothing special to do here. Just display the
                         * menu item outside of intercept.
                         */
                    }
                    else
                    {
                        gpCurrMenu = gpMainMenu;
                        // Display the help screen
                        gCurrMenuItemEntry = gpMainMenu->numItems - 1;
                    }
                    CUI_dispMenu(false);
                    break;
                default :
                    break;
            }
        }
    }

    //Clear the buffer
    memset(gUartRxBuffer, '\0', sizeof(gUartRxBuffer));

    UART2_read(gUartHandle, gUartRxBuffer, sizeof(gUartRxBuffer), NULL);
    return CUI_SUCCESS;
}
#else
CUI_retVal_t CUI_registerMenu(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu) { return CUI_SUCCESS; }

CUI_retVal_t CUI_deRegisterMenu(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu) { return CUI_SUCCESS; }

CUI_retVal_t CUI_updateMultiMenuTitle(const char* _pTitle) { return CUI_SUCCESS; }

CUI_retVal_t CUI_menuNav(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu, const uint32_t _itemIndex) { return CUI_SUCCESS; }

CUI_retVal_t CUI_processMenuUpdate(void) { return CUI_SUCCESS; }
#endif
/******************************************************************************
 * Status Line CUI APIs
 *****************************************************************************/
/*********************************************************************
 * @fn          CUI_statusLineResourceRequest
 *
 * @brief       Request access to a new status line
 *
 * @param       _clientHandle - Valid client handle
 *              _pLabel - C string label for the new status line
 *              _pLineId - Pointer to an unsigned integer. The value of the
 *                  unsigned integer will be set and represent the line number
 *                  you were given access to.
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_statusLineResourceRequest(const CUI_clientHandle_t _clientHandle, const char _pLabel[MAX_STATUS_LINE_LABEL_LEN], const bool _refreshInd, uint32_t* _pLineId)
{
    CUI_retVal_t retVal = CUI_publicAPIChecks(_clientHandle);
    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    if (NULL == _pLabel || NULL == _pLineId)
    {
        return CUI_INVALID_PARAM;
    }

    retVal = CUI_acquireStatusLine(_clientHandle, _pLabel, _refreshInd, _pLineId);
    if (CUI_SUCCESS != retVal)
    {
        /*
         * Set the value of _pLineId to a invalid lineId in case
         * the user tries to print to this line even though it was
         * not successfully acquired.
         */
        //TODO: change _pLineId to be a signed integer so that -1 may be used.
        *_pLineId = 0xFF;
        return retVal;
    }

    /*
     * Print a default '--' value to the line
     */
    CUI_statusLinePrintf(_clientHandle, *_pLineId, "--");

    return CUI_SUCCESS;
}

/*********************************************************************
 * @fn          CUI_statusLinePrintf
 *
 * @brief        Update an acquired status line
 *
 * @param       _clientHandle - Client handle that owns the status line
 *              _lineId - unsigned integer of the line that you are updating.
 *                  This is the value set by CUI_statusLineResourceRequest().
 *              _format - C string printf style format.
 *              ... - Var args to be formated by _format
 *
 * @return      CUI_retVal_t representing success or failure.
 */
CUI_retVal_t CUI_statusLinePrintf(const CUI_clientHandle_t _clientHandle,
        const uint32_t _lineId, const char *_format, ...)
{
    /*
     * This buffer will be passed to CUI_writeString(). The address must be
     * valid at all times. Using a ping pong buffer system will allow a second
     * quick call to CUI_statusLinePrintf to not effect the buffer of a
     * previous unfinished call.
     */
    char statusLineBuff[CUI_MAX_LABEL_AND_SEP_LEN + MAX_STATUS_LINE_VALUE_LEN + 32]; // plus 32 for cursor movement/clearing
    va_list args;

    CUI_retVal_t retVal = CUI_publicAPIChecks(_clientHandle);
    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    // Known to be good since the public api check was successful
    int clientIndex = CUI_getClientIndex(_clientHandle);

    if (_clientHandle != gStatusLineResources[clientIndex][_lineId].clientHash)
    {
        return CUI_INVALID_CLIENT_HANDLE;
    }

    if (CUI_ACQUIRED != gStatusLineResources[clientIndex][_lineId].status)
    {
        return CUI_RESOURCE_NOT_ACQUIRED;
    }

#if !defined(CUI_SCROLL_PRINT)
    uint32_t offset;
#ifndef CUI_MIN_FOOTPRINT
    if (MAX_REGISTERED_MENUS == 0)
    {
        offset = 1;
    }
    else
    {
        offset = CUI_INITIAL_STATUS_OFFSET;
    }
#else
    offset = 1;
#endif
    offset += gStatusLineResources[clientIndex][_lineId].lineOffset;

    //TODO: Remove magic length number
    SystemP_snprintf(statusLineBuff, 32,
        CUI_ESC_CUR_HIDE CUI_ESC_CUR_HOME CUI_ESC_CUR_LINE CUI_ESC_CLR_STAT_LINE_VAL "%c",
         offset, CUI_STATUS_LINE_START_CHAR);
#endif
    size_t availableLen = sizeof(statusLineBuff) - 1;
    size_t buffSize = availableLen;

    // Label must be printed for testing scripts to parse the output easier
    strncat(statusLineBuff, gStatusLineResources[clientIndex][_lineId].label,
            availableLen);

    retVal = CUI_updateRemLen(&availableLen, statusLineBuff, buffSize);
    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    va_start(args, _format);
    SystemP_vsnprintf(&statusLineBuff[strlen(statusLineBuff)], availableLen, _format, args);
    va_end(args);

    retVal = CUI_updateRemLen(&availableLen, statusLineBuff, buffSize);
    if (CUI_SUCCESS != retVal)
    {
        return retVal;
    }

    static char refreshChars[] = {'\\', '|', '/', '-'};
    static uint8_t refreshCharIdx = 0;

    if (gStatusLineResources[clientIndex][_lineId].refreshInd)
    {
        SystemP_snprintf(&statusLineBuff[strlen(statusLineBuff)], availableLen, " %c", refreshChars[refreshCharIdx]);
        refreshCharIdx = (refreshCharIdx + 1) % sizeof(refreshChars);
    }

#if !defined(CUI_SCROLL_PRINT)
    char endChar[] = {CUI_END_CHAR};
    strncat(statusLineBuff, endChar, 1);
#else
    strncat(statusLineBuff, CUI_NL_CR, sizeof(CUI_NL_CR));
#endif

    CUI_writeString(statusLineBuff, strlen(statusLineBuff));

#ifndef CUI_MIN_FOOTPRINT
    // This will check if a cursor is active and put the cursor back
    //  if it is necessary
    CUI_updateCursor();
#endif

    return CUI_SUCCESS;
}

/*********************************************************************
 *  @fn         CUI_assert
 *
 *  @brief      Without requiring a cuiHandle_t you may print an assert
 *                string and optionally spinLock while flashing the leds.
 *
 * @param       _assertMsg - Char pointer of the message to print
 *              _spinLock - Whether or not to spinLock
 *
 * @return      CUI_retVal_t representing success or failure.
 */
void CUI_assert(const char* _assertMsg, const bool _spinLock)
{
#ifndef CUI_POSIX
    if (BIOS_ThreadType_Main == BIOS_getThreadType())
    {
        /*
         *  UART requires the bios to have been started. If you get stuck
         *  here it is because CUI_assert is being called before
         *  BIOS_start().
         */
        // TODO: solve this issue CUI_ledAssert();
        while(1){};
    }
#endif

    if (!gModuleInitialized)
    {
        CUI_params_t params;
        CUI_paramsInit(&params);
        CUI_init(&params);
    }

    char statusLineBuff[MAX_STATUS_LINE_VALUE_LEN];
    char tmp[32];

    // Display this in the line between the menu and the status lines
    uint32_t offset = CUI_INITIAL_STATUS_OFFSET - 1;

    SystemP_snprintf(tmp, sizeof(tmp),
        CUI_ESC_CUR_HIDE CUI_ESC_CUR_HOME CUI_ESC_CUR_LINE CUI_ESC_CLR_STAT_LINE_VAL "%c",
        offset, CUI_STATUS_LINE_START_CHAR);
    CUI_writeString(tmp, strlen(tmp));

    SystemP_snprintf(statusLineBuff, sizeof(statusLineBuff),  CUI_COLOR_RED "%s%c" CUI_COLOR_RESET, _assertMsg, CUI_END_CHAR);

    CUI_writeString(statusLineBuff, strlen(statusLineBuff));

    // If _spinLock is true, infinite loop and flash the leds
    if (_spinLock)
    {
        extern const uint_least8_t LED_count;
        extern LED_Config LED_config[2];

        LED_Params ledParams;
        LED_Params_init(&ledParams);

        for(uint8_t i = 0; i < LED_count; i++)
        {
            LED_close(&LED_config[i]);
            LED_Handle ledHandle = LED_open(i, &ledParams);
            LED_startBlinking(ledHandle, 50, LED_BLINK_FOREVER);
        }

        while(1){};
    }
}

#ifndef CUI_MIN_FOOTPRINT
void CUI_menuActionBack(const int32_t _itemEntry)
{
    if (NULL != gpCurrMenu->pUpper)
    {
       gpCurrMenu = gpCurrMenu->pUpper;
       gCurrMenuItemEntry = gPrevMenuItemEntry;
    }
}

void CUI_menuActionHelp(char _input, char* _lines[3], CUI_cursorInfo_t* _curInfo)
{
    if (_input == CUI_ITEM_PREVIEW)
    {
        strncat(_lines[1], "Press Enter for Help", MAX_MENU_LINE_LEN);
    }
    else
    {
        strncat(_lines[0], "[Arrow Keys] Navigate Menus | [Enter] Perform Action, Enter Submenu", MAX_MENU_LINE_LEN);
        strncat(_lines[1], "----------------------------|--------------------------------------", MAX_MENU_LINE_LEN);
        strncat(_lines[2], "[Esc] Return to Main Menu   | [Backspace] Return to Parent Menu", MAX_MENU_LINE_LEN);
    }
}
#endif

/*********************************************************************
 * Private Functions
 */
static CUI_retVal_t CUI_updateRemLen(size_t* _currRemLen, char* _buff, size_t _buffSize)
{
    size_t newLen = strlen(_buff);

    if (newLen >= _buffSize - 1)
    {
        return CUI_FAILURE;
    }

    *_currRemLen = (_buffSize - newLen -1);
    return CUI_SUCCESS;
}

static void UartWriteCallback(UART2_Handle _handle, void *_buf, size_t _size, void *_userArg, int_fast16_t _status)
{
    (void)_userArg;
    (void)_status;

    gTxSent += _size;
    if (gTxSent < gTxLen)
    {
        UART2_write(gUartHandle, (const void*)&(gTxBuff[gTxSent]), gTxLen - gTxSent, NULL);
    }
    else
    {
        SemaphoreP_post(gUartSem);
    }
}

#ifndef CUI_MIN_FOOTPRINT
static void UartReadCallback(UART2_Handle _handle, void *_buf, size_t _size, void *_userArg, int_fast16_t _status)
{
    (void)_userArg;
    (void)_status;
    CUI_callMenuUartUpdateFn();
}
#endif

static CUI_retVal_t CUI_writeString(void * _buffer, size_t _size)
{
    // Check pre-conditions
    if((gUartHandle == NULL) || (_buffer == NULL) || (_size > sizeof(gTxBuff)))
    {
        return CUI_UART_FAILURE;
    }
    SemaphoreP_pend(gUartSem, SemaphoreP_WAIT_FOREVER);

    gTxSent = 0;
    gTxLen = _size;
    memcpy(gTxBuff, _buffer, _size);

    UART2_write(gUartHandle, (const void*)gTxBuff, _size, NULL);

    return CUI_SUCCESS;
}

static CUI_retVal_t CUI_publicAPIChecks(const CUI_clientHandle_t _clientHandle)
{
    if (!gModuleInitialized)
    {
        return CUI_MODULE_UNINITIALIZED;
    }

    return CUI_validateHandle(_clientHandle);
}

static CUI_retVal_t CUI_validateHandle(const CUI_clientHandle_t _clientHandle)
{
    if (!_clientHandle)
    {
        return CUI_INVALID_CLIENT_HANDLE;
    }

    if (CUI_getClientIndex(_clientHandle) == -1)
    {
        return CUI_INVALID_CLIENT_HANDLE;
    }
    else
    {
        return CUI_SUCCESS;
    }
}

static CUI_retVal_t CUI_acquireStatusLine(const CUI_clientHandle_t _clientHandle, const char _pLabel[MAX_STATUS_LINE_LABEL_LEN], const bool _refreshInd, uint32_t* _pLineId)
{
    SemaphoreP_pend(gStatusSem, SemaphoreP_WAIT_FOREVER);

    int clientIndex = CUI_getClientIndex(_clientHandle);
    if (clientIndex == -1)
    {
        return CUI_INVALID_PARAM;
    }

    int freeIndex = -1;
    for (int i = 0; i < gMaxStatusLines[clientIndex]; i++)
    {
        if (CUI_RELEASED == gStatusLineResources[clientIndex][i].status)
        {
            freeIndex = i;
            break;
        }
    }

    SemaphoreP_post(gStatusSem);

    if (-1 == freeIndex)
    {
        return CUI_NO_ASYNC_LINES_RELEASED;
    }

    uint32_t offset = 0;
    for (uint32_t i = 0; i < clientIndex; i++)
    {
        offset += gMaxStatusLines[i];
        offset ++; // allow 1 empty line between clients
    }

    offset += freeIndex;

    //Add a ": " to every label
    memset(gStatusLineResources[clientIndex][freeIndex].label, '\0', sizeof(gStatusLineResources[clientIndex][freeIndex].label));
    SystemP_snprintf(gStatusLineResources[clientIndex][freeIndex].label,
             MAX_STATUS_LINE_LABEL_LEN + strlen(CUI_LABEL_VAL_SEP),
             "%s%s", _pLabel, CUI_LABEL_VAL_SEP);
    gStatusLineResources[clientIndex][freeIndex].lineOffset = offset;
    gStatusLineResources[clientIndex][freeIndex].clientHash = _clientHandle;
    gStatusLineResources[clientIndex][freeIndex].status = CUI_ACQUIRED;
    gStatusLineResources[clientIndex][freeIndex].refreshInd = _refreshInd;



    /*
     * Save this "line id" as a way to directly control the line, similarly to
     * how a client can directly control a led or button through pinIds.
     */
    *_pLineId = freeIndex;

    return CUI_SUCCESS;
}

static int CUI_getClientIndex(const CUI_clientHandle_t _clientHandle)
{
    for (uint32_t i = 0; i < MAX_CLIENTS; i++)
    {
        if (_clientHandle == gClientHandles[i])
        {
            return i;
        }
    }
    return -1;
}

#ifndef CUI_MIN_FOOTPRINT
static void CUI_callMenuUartUpdateFn()
{
    /*
     * When a menu is registered it is guaranteed to contain a non NULL
     * menu update function. So if a sub menu doesn't have a valid uart update
     * function, at some point in the menu tree between the current menu and
     * the top level menu there will be a valid uart update function.
     *
     * If somehow the menu object has been corrupted and there is no non NULL
     * uart update function then nothing will be called.
     */
    CUI_menu_t* menu = gpCurrMenu;
    while (NULL != menu)
    {
        if (menu->uartUpdateFn)
        {
            menu->uartUpdateFn();
            break;
        }
        // Try the upper/parent menu to look for a uart update function
        menu = menu->pUpper;
    }

    /*
     * Somehow from gpCurrMenu to the top most menu there was no valid
     * uart update function to call. There is no way to account for this.
     */
}

static void CUI_updateCursor(void)
{
    char buff[32];
    if (gCursorActive)
    {
        SystemP_snprintf(buff, sizeof(buff),
            CUI_ESC_CUR_HOME CUI_ESC_CUR_ROW_COL CUI_ESC_CUR_SHOW,
            gCursorInfo.row, gCursorInfo.col);
        CUI_writeString(buff, strlen(buff));
    }
}

static bool CUI_handleMenuIntercept(CUI_menuItem_t* _pItemEntry, uint8_t _input)
{
    bool updateHandled = false;
    bool interceptStarted = false;

    char *line[3];
    memset(menuBuff, '\0', sizeof(menuBuff));

    line[0] = &menuBuff[CUI_MENU_START_ESCAPE_LEN];
    line[1] = &menuBuff[CUI_MENU_START_ESCAPE_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN];
    line[2] = &menuBuff[CUI_MENU_START_ESCAPE_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN];

    CUI_cursorInfo_t curInfo = {-1, -1};

    if (_pItemEntry->itemType == CUI_MENU_ITEM_TYPE_INTERCEPT)
    {
        if (_pItemEntry->interceptActive)
        {
            // If intercept is active, pressing CUI_INPUT_EXECUTE should
            // disable it so that normal navigation may continue
            if (CUI_INPUT_EXECUTE == _input)
            {
                _pItemEntry->interceptActive = false;

                // send key to application for handling
                if (_pItemEntry->item.pFnIntercept)
                {
                    _pItemEntry->item.pFnIntercept(CUI_ITEM_INTERCEPT_STOP,
                                                   line, &curInfo);
                }
                gCursorActive = false;
                updateHandled = true;
                CUI_dispMenu(false);
            }
            else if (CUI_INPUT_ESC == _input)
            {
                _pItemEntry->interceptActive = false;

                // send key to application for handling
                if (_pItemEntry->item.pFnIntercept)
                {
                    _pItemEntry->item.pFnIntercept(CUI_ITEM_INTERCEPT_CANCEL,
                                                   line, &curInfo);
                }
                gCursorActive = false;
                updateHandled = true;
                CUI_dispMenu(false);
            }
        }
        else if (CUI_INPUT_EXECUTE == _input)
        {
            /*
             * Since this screen is interceptable, pressing 'E' should start
             *  allowing the application to intercept the key presses.
             */
            _pItemEntry->interceptActive = true;
            interceptStarted = true;
        }

        if (_pItemEntry->interceptActive)
        {
            // Tell the Action if interception just started, else send the input directly
            char input = (interceptStarted ? CUI_ITEM_INTERCEPT_START : _input);

            // send key to application for handling
            if (_pItemEntry->item.pFnIntercept)
            {
                _pItemEntry->item.pFnIntercept(input, line, &curInfo);
            }

            updateHandled = true;
            CUI_dispMenu(true);

            // If a cursor should be shown, add this at the end of the string
            if ((curInfo.col != -1) && (curInfo.row != -1))
            {
                gCursorActive = true;
                gCursorInfo.col = curInfo.col;
                gCursorInfo.row = curInfo.row;
                CUI_updateCursor();
            }
            else
            {
                gCursorActive = false;
            }
        }
    }

    return updateHandled;
}

static bool CUI_handleMenuList(CUI_menuItem_t* _pItemEntry, uint8_t _input)
{
    bool updateHandled = false;

    char *line[3];
    memset(menuBuff, '\0', sizeof(menuBuff));

    line[0] = &menuBuff[CUI_MENU_START_ESCAPE_LEN];
    line[1] = &menuBuff[CUI_MENU_START_ESCAPE_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN];
    line[2] = &menuBuff[CUI_MENU_START_ESCAPE_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN];

    if (_pItemEntry->itemType == CUI_MENU_ITEM_TYPE_LIST)
    {
        if (_pItemEntry->interceptActive)
        {
            if (CUI_INPUT_ESC == _input || CUI_INPUT_BACK == _input)
            {
                _pItemEntry->interceptActive = false;
                return updateHandled;
            }

            // update index value
            if (CUI_INPUT_RIGHT ==  _input)
            {
                _pItemEntry->item.pList->currListIndex = (_pItemEntry->item.pList->currListIndex + 1 + _pItemEntry->item.pList->maxListItems) % _pItemEntry->item.pList->maxListItems;
            }
            else if (CUI_INPUT_LEFT == _input)
            {
                _pItemEntry->item.pList->currListIndex = (_pItemEntry->item.pList->currListIndex - 1 + _pItemEntry->item.pList->maxListItems) % _pItemEntry->item.pList->maxListItems;
            }

            if (_pItemEntry->item.pList->pFnListAction)
            {
                _pItemEntry->item.pList->pFnListAction(_pItemEntry->item.pList->currListIndex, line, (CUI_INPUT_EXECUTE == _input));
            }

            // let normal menu navigation handle the update if input was executed
            updateHandled = !(CUI_INPUT_EXECUTE == _input);

            // turn off interceptActive
            _pItemEntry->interceptActive = !(CUI_INPUT_EXECUTE == _input);
        }
        else
        {
            if (CUI_INPUT_EXECUTE == _input)
            {
                _pItemEntry->interceptActive = true;
                if (_pItemEntry->item.pList->pFnListAction)
                {
                    _pItemEntry->item.pList->pFnListAction(_pItemEntry->item.pList->currListIndex, line, false);
                }
                updateHandled = true;
            }
        }

        if (updateHandled)
        {
            strncpy(line[2], _pItemEntry->pDesc, MAX_MENU_LINE_LEN);
            CUI_dispMenu(true);
        }
    }
    return updateHandled;
}

static void CUI_dispMenu(bool _menuPopulated)
{
    char *line[3];
    line[0] = &menuBuff[CUI_MENU_START_ESCAPE_LEN];
    line[1] = &menuBuff[CUI_MENU_START_ESCAPE_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN];
    line[2] = &menuBuff[CUI_MENU_START_ESCAPE_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN + MAX_MENU_LINE_LEN + CUI_NL_CR_LEN];

    if (false == _menuPopulated)
    {
        CUI_cursorInfo_t cursorInfo;
        CUI_menuItem_t* itemEntry = &(gpCurrMenu->menuItems[gCurrMenuItemEntry]);

        memset(menuBuff, '\0', sizeof(menuBuff));

        // Show the menu Title. Whenever possible to provide context
        if (gpCurrMenu == gpMainMenu)
        {
            strncpy(line[0], gpCurrMenu->pTitle, MAX_MENU_LINE_LEN);
        }
        else if (gCurrMenuItemEntry != (gpCurrMenu->numItems - 1))
        {
            /*
             *  If the current Menu Item is the 'back' item, leave the
             *  first line empty to keep the back screen clean.
             */
            CUI_menu_t* pMenu = gpCurrMenu;
            while((pMenu->pUpper) && (pMenu->pUpper != cuiMultiMenu))
            {
                pMenu = pMenu->pUpper;
            }
            strncpy(line[0], pMenu->pTitle, MAX_MENU_LINE_LEN);
        }

        // If this is an interceptable item, instead of the title, allow a preview
        if (itemEntry->itemType == CUI_MENU_ITEM_TYPE_INTERCEPT && itemEntry->item.pFnIntercept)
        {
            itemEntry->item.pFnIntercept(CUI_ITEM_PREVIEW, line, &cursorInfo);
        }
        else if (itemEntry->itemType == CUI_MENU_ITEM_TYPE_LIST && itemEntry->item.pList->pFnListAction)
        {
            itemEntry->item.pList->pFnListAction(itemEntry->item.pList->currListIndex, line, false);
        }

        // Guarantee the last line is not overwritten by the intercept function
        if (itemEntry->itemType == CUI_MENU_ITEM_TYPE_SUBMENU)
        {
            // If the curr item is a sub menu, display the sub menu title
            strncpy(line[2], itemEntry->item.pSubMenu->pTitle, MAX_MENU_LINE_LEN);
        }
        else
        {
            // If not, display the items description
            strncpy(line[2], itemEntry->pDesc, MAX_MENU_LINE_LEN);
        }
    }

    /*
     * Start copying the menu into the dispBuff for writing to the UART
     *
     * Copy the first line, then add the newline and carriage return.
     * Do this for all three lines.
     *
     * The memory for the carriage returns, newlines, and the final
     *  CUI_END_CHAR are accounted for in the menuBuff already.
     */
#if !defined(CUI_SCROLL_PRINT)
    SystemP_snprintf(menuBuff, 32,
        CUI_ESC_CUR_HIDE CUI_ESC_CUR_MENU_BTM CUI_ESC_CLR_UP CUI_ESC_CUR_HOME "%c",
        MAX_MENU_LINE_LEN, CUI_MENU_START_CHAR);
#endif
    // Note these memory regions do not overlap
    strncat(menuBuff, line[0], MAX_MENU_LINE_LEN);
    // Set the newline and carriage return
    strncat(menuBuff, CUI_NL_CR, sizeof(CUI_NL_CR));

    strncat(menuBuff, line[1], MAX_MENU_LINE_LEN);
    strncat(menuBuff, CUI_NL_CR, sizeof(CUI_NL_CR));

    strncat(menuBuff, line[2], MAX_MENU_LINE_LEN);

#if !defined(CUI_SCROLL_PRINT)
    char endChar[2] = {CUI_END_CHAR, '\0'};
    strncat(menuBuff, endChar, strlen(endChar));
#else
    strncat(menuBuff, CUI_NL_CR, strlen(CUI_NL_CR));
#endif

    CUI_writeString(menuBuff, strlen(menuBuff));
}

static void CUI_menuActionNavigate(uint8_t _navDir)
{
    // No menu change necessary. There is only one screen
    if (1 == gpCurrMenu->numItems)
    {
        return;
    }

    if (CUI_INPUT_LEFT == _navDir)
    {
        // Wrap menu around from left to right
        gCurrMenuItemEntry =  (gCurrMenuItemEntry - 1 + gpCurrMenu->numItems) % (gpCurrMenu->numItems);
    }
    else if (CUI_INPUT_RIGHT == _navDir)
    {
        // Wrap menu around from right to left
        gCurrMenuItemEntry =  (gCurrMenuItemEntry + 1 + gpCurrMenu->numItems) % (gpCurrMenu->numItems);
    }

    CUI_dispMenu(false);
}

static void CUI_menuActionExecute(void)
{
    if (gpCurrMenu->menuItems[gCurrMenuItemEntry].itemType == CUI_MENU_ITEM_TYPE_SUBMENU)
    {
        /*
         * If Item executed was a SubMenu, then preserve gCurrMenuItemEntry and enter the
         *  submenu.
         */
        gpCurrMenu = gpCurrMenu->menuItems[gCurrMenuItemEntry].item.pSubMenu;
        gPrevMenuItemEntry = gCurrMenuItemEntry;
        gCurrMenuItemEntry = 0;
    }
    else if (gpCurrMenu->menuItems[gCurrMenuItemEntry].itemType == CUI_MENU_ITEM_TYPE_ACTION)
    {
        CUI_pFnAction_t actionFn;
        actionFn = gpCurrMenu->menuItems[gCurrMenuItemEntry].item.pFnAction;
        if (actionFn)
        {
            actionFn(gCurrMenuItemEntry);
        }
    }

    CUI_dispMenu(false);
    return;
}

static CUI_retVal_t CUI_findMenu(CUI_menu_t* _pMenu, CUI_menu_t* _pDesiredMenu, uint32_t* _pPrevItemIndex)
{
    if (_pMenu == _pDesiredMenu)
    {
        return CUI_SUCCESS;
    }

    uint32_t numItems = _pMenu->numItems;
    for (int i = 0; i < numItems; i++)
    {
        *_pPrevItemIndex = i;
        /*
         * If pDesc is NULL, it is a subMenu
         */
        if (NULL == _pMenu->menuItems[i].pDesc)
        {
            CUI_menu_t* subMenu = _pMenu->menuItems[i].item.pSubMenu;

            if (CUI_SUCCESS == CUI_findMenu(subMenu, _pDesiredMenu, _pPrevItemIndex))
            {
                return CUI_SUCCESS;
            }
        }
    }
    return CUI_FAILURE;
}
#endif /* ifndef CUI_MIN_FOOTPRINT */

