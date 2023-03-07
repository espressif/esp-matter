
/******************************************************************************

 @file CUI.h

 @brief This file contains the interface of the Common User Interface. (CUI)

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

/**
@headerfile  cui.h
$Date: 2018-12-18 12:13:13 -0800 (Tue, 18 Dec 2018) $
$Revision: 42528 $

@mainpage CUI API Documentation

Overview
============================

    This file contains the interface of the Common User Interface or CUI.

    The CUI controls the access to User Interface resources. On a launchpad
    device, these resources are made up of Buttons, LEDs and UART i/o.

    The CUI provides an API that can be used to request for access to resources
    and then read/write to those resources. In order to request and acquire a
    resource the notion of a 'client' is used. The Client handle is used to
    successfully communicate with the CUI API. Once you open a client with the
    CUI you may then request access to a resource. If the resource is available
    the CUI will give the client access to the resource.

Resource Types
============================

    As described earlier, there are three different resources the CUI controls
    access to. (Buttons, LEDs and UART i/o).

    For Buttons and LEDs the process for requesting and using the resource is
    quite simple and is nearly identical to how you would use the Button or LED
    drivers to do the same.

    For UART i/o there are two resources to consider. Firstly there is the
    notion of a 'menu'. A menu specific to your application can be registered with
    the CUI. The CUI will handle menu navigation, reading and writing. A menu
    can be as simple or as complex as desired. The second UART i/o is the notion
    of a 'status line'. Where the menu is designed to be a display
    driven by the input of the user, the status lines are capable of displaying
    the latest value of any particular set of information the developer chooses.
    A great example of a menu item would be any sort of toggle led event your
    device can perform or maybe a commissioning action and an example of a
    status line could be current network status or any application state
    information. More details on menus and status lines can be found below.

Usage
==========================

    This documentation provides a basic usage summary and a set of examples in
    the form of commented code fragments. Detailed descriptions of the APIs are
    provided in subsequent sections.

    Initilization is the first step.

    @code
    // Import the CUI definitions
    #include "cui.h"

    CUI_params_t cuiParams;
    CUI_paramsInit(&cuiParams);

    // One-time initialization of the CUI
    if (CUI_SUCCESS != CUI_init(&cuiParams)) {
        // handle failure
    }
    @endcode

    By calling CUI_paramsInit() you are setting the cuiParams to their default
    values.

        cuiParams.manageBtns = true
        cuiParams.manageLeds = true
        cuiParams.manageUart = true

    If your application requires special management of buttons, leds, or uart
    the developer can set the parameters appropriately before calling CUI_init()

    After the CUI has been initialized you can then open a client.

    @code
    CUI_clientParams_t clientParams;
    CUI_clientParamsInit(&clientParams);

    strncpy(clientParams.clientName, "My first CUI application", MAX_CLIENT_NAME_LEN);
    clientParams.maxStatusLines = 5;

    CUI_clientHandle_t clientHandle = CUI_clientOpen(&clientParams);
    if (clientHandle == NULL) {
        // handle failure
    }
    @endcode

    By Calling CUI_clientParamsInit() you are setting the clientParams to their
    default values.

        strcpy(clientParams.clientName, "");
        clientParams.maxStatusLines = 0;

    <b>It is currently required that your clientName not be empty.</b>
    It can be anything you would like as it is only used to create a simple hash
    value.

    If you would like to add status lines to your application, you will need to
    update clientParams.maxStatusLines as the code snippet above shows.

    Now that you have a valid client you may start requesting access to
    resources. When requesting a button resource you must specify which button
    you are requesting as well as a callback that the CUI can use to notify you
    of a button change event that matches the CUI_btnPressCB_t prototype.

    @code
    CUI_btnRequest_t rightBtnReq;
    rightBtnReq.index = CONFIG_BTN_RIGHT;
    rightBtnReq.appCB = myBtnChangeHandler;

    if (CUI_SUCCESS != CUI_btnResourceRequest(clientHandle, &rightBtnReq) {
        //handle failure
    }
    @endcode

    If the resource was available to acquire then you will now have access to the
    right button.

    Alternatively, if you would like to read the button state without requiring
    an application callback, you can request the button in a sort of polling mode
    by using a NULL appCB. Then simply request the button's value whenever you
    require it.

    @code
    CUI_btnRequest_t rightBtnReq;
    rightBtnReq.index = CONFIG_BTN_RIGHT;
    rightBtnReq.appCB = NULL;

    if (CUI_SUCCESS != CUI_btnResourceRequest(clientHandle, &rightBtnReq) {
        //handle failure
    }

    bool btnState = false;
    retVal = CUI_btnGetValue(clientHandle, CONFIG_BTN_RIGHT, &btnState);
    if (!btnState) {
        //handle button logic
    }
    @endcode

    If you would like to change the btnMode of a button you already have access
    to then you can use the CUI_btnSetCb() API. By using a non-NULL value for the
    third parameter you will be using a sort of call back mode. If you use a NULL
    value for the third parameter, you will be using a polling mode.

    @code
    CUI_retVal retVal;
    retVal = CUI_btnSetCb(clientHandle, CONFIG_BTN_RIGHT, appChangeKeyCB);
    if (retVal != CUI_SUCCESS) {
        //handle failure
    }
    @endcode

    Buttons are great but only part of the picture. Let's request a LED as well.
    This time the only information you need to provide is the index of the LED you
    wish to acquire.

    @code
    CUI_ledRequest_t greenLedReq;
    greenLedReq.index = CONFIG_LED_GREEN;
    if (CUI_SUCCESS != CUI_ledResourceRequest(clientHandle, &greenLedReq) {
        // Handle failure
    }
    @endcode

    If the led was available you will have gained access to it and when you wish
    to change the state of the led you need only to call the CUI LED APIs.

    @code
    CUI_ledToggle(clientHandle, CONFIG_LED_GREEN);
    CUI_ledBlink(clientHandle, CONFIG_LED_GREEN, 5);
    CUI_ledBlink(clientHandle, CONFIG_LED_GREEN, CUI_BLINK_CONTINUOUS);
    CUI_ledOn(clientHandle, CONFIG_LED_GREEN);
    CUI_ledOff(clientHandle, CONFIG_LED_GREEN);
    @endcode

    One note on CUI_ledBlink is that you can choose to blink continuously or you
    can choose to blink a specific number of times. If you call CUI_ledBlink while
    the led is already blinking. The only thing that will happen is that the
    number of blinks will be reset to the new value.

    Now you have full control over buttons and LEDs. But you may want to let the
    user know when you have joined a network for instance. Well this is the
    perfect use for a status line. First we must request access to an available
    status line. Status lines are handed out upon request in numeric order and the
    number of status lines available to you are determined by clientParams.maxStatusLines
    when you opened your client.

    @code
    uint32_t connStatusLine;
    if (CUI_SUCCESS != CUI_statusLineResourceRequest(clientHandle, "Conn Status", &connStatusLine) {
        // handle failure
    }
    @endcode

    If clientParams.maxStatusLines haven't already been requested from the CUI you will
    have been given access to that line. When a status line is acquired, a
    default line value will be printed in the form of "LABEL: --".
    So in the example above we will see "Conn Status: --" printed on the screen.

    If you wish to tell the user a different value for that status line you need
    to tell the CUI what that new value is. Using a variatic function you can
    pass in a format string and any number of optional items to be formated
    according to your format string. Similar to printf() if you are familiar.

    @code
    char lineFormat[MAX_STATUS_LINE_VALUE_LEN];
    strncpy(lineFormat, "Successfully Connected", MAX_STATUS_LINE_VALUE_LEN);
    if (CUI_SUCCESS != CUI_statusLinePrintf(clientHandle, connStatusLine, lineFormat) {
        //handle failure
    }
    @endcode

    Now you will see "Conn Status: Successfully Connected".

    If you wish to spice your status lines up a bit more, there are a few color
    codes available to you by default. To make use of these, simply surround the
    text you wish to be given a color between a color macro and the reset macro.

    @code
    CUI_COLOR_RESET
    CUI_COLOR_RED
    CUI_COLOR_GREEN
    CUI_COLOR_YELLOW
    CUI_COLOR_MAGENTA
    CUI_COLOR_CYAN
    CUI_COLOR_WHITE
    @endcode

    For instance, you could use red for something bad, and green for something good.
    These can be used for status lines or menu lines. Any text that is being displayed
    can be colored with these macros

    @code
    CUI_statusLinePrintf(clientHandle, connStatusLine, CUI_COLOR_RED "Connection Failure :(" CUI_COLOR_RESET);

    CUI_statusLinePrintf(clinetHandle, connStatusLine, CUI_COLOR_GREEN "Connection Successful! :)" CUI_COLOR_RESET);
    @endcode

    Lastly in order to create a menu you must first declare your menu layout.
    A menu can be as simple or as complicated as you want. At the root of every application's
    menu though are three things.
        Main Menu
        Sub Menus
        Menu Actions

    A Main menu as it sounds is the top level menu of your application, this is
    what you will use to register your entire menu with the CUI. Your main menu
    can consist of any number of sub menus and menu actions. In turn, those
    sub menus can consist of any number of sub menus and menu actions as well.
    This creates a tree topology that will go as far as you desire it.

    A menu action can be of two variants. Either a normal action or an
    interceptable action. A normal action is used best for triggering sections
    of code via UART input. Say for instance you want to toggle the state of
    your led or trigger your device to connect to an open network. Then a normal
    action is exactly what you want. You may have several actions you want to
    perform that are very similar. A normal action can help with this by
    providing you with the itemEntry that is calling your action. This can be
    used in order to have a single action function in your code that can handle
    multiple different situations based upon the itemEntry that is received.
    Though if you want to do something more complex like modify your device's
    channel mask or panId, then an interceptable action is required.

    Interceptable actions are created such that instead of triggering a one off
    section of code, they actually start to intercept the input from the UART.
    Say for instance you want to modify the channel mask. The user selects the
    action and then all UART input can be processed by your action code.
    You can handle this input however you want. Then when the user de-selects the
    action the UART input will be again handled by the CUI and menu navigation
    will continue as normal.

    As menus can become rather complex this file provides some helpful macros in
    order to simplify menu definition. Here's an example of declaring a main menu
    that has a normal action as well as a sub menu. Within the sub menu there is
    an interceptable action.

    @code
    CUI_SUB_MENU(mySubMenu, " My Sub Menu ", 1, myMainMenu)
        CUI_MENU_ITEM_INT_ACTION("< Edit Channel Mask >", editChannelMaskFn)
    CUI_SUB_MENU_END

    CUI_MAIN_MENU(myMainMenu, " My Main Menu ", 2, processMenuUpdateFn)
        CUI_MENU_ITEM_ACTION("<  Toggle LED   >", toggleLedFn)
        CUI_MENU_ITEM_SUBMENU(&mySubMenu)
    CUI_MAIN_MENU_END
    @endcode

    If you are wondering how the macros work you can look at the implementation.
    Here is the prototype for the CUI_MAIN_MENU and CUI_SUB_MENU macros though.

    @code
        #define CUI_MAIN_MENU(_menuSymbol, _pMenuTitle, _numItems, _pMenuUpdateFn)
        #define CUI_SUB_MENU(_menuSymbol, _pMenuTitle, _numItems, _pUpperMenu)
    @endcode

    Now you can see that the sub menu was defined to be the 'mySubMenu' symbol
    with a menu title of " My Sub Menu ", '1' menu item and it's parent menu is
    the 'myMainMenu' symbol.

    Similarly the main menu was declared to be the 'myMainMenu' symbol with a
    menu title of " My Main Menu " and 2 menu items. The last parameter is
    different between them though because of two reasons. Firstly, a main menu
    has no notion of a parent menu because it is already the top level menu.
    Secondly, '_pMenuUpdateFn' is a parameter so that the CUI can gain
    processing context whenever menu processing needs to occur.

    Since the CUI does not create an rtos task, it technically does not have
    it's own processing context. This means that if any CUI menu processing is
    to take place, such as the user navigating left and right through the
    different menus, some other rtos task must own the processing time for this
    to occur. This function simply needs to end up calling CUI_processMenuUpdate.
    Whether it does that directly or it posts an event to a semaphore so that
    the task can call CUI_processMenuUpdate() when there is time to do so is
    up to the developer. <b>To be clear, the '_pMenuUpdateFn' is required for your
    menu to operate correctly.</b> Without it the menu will fail to register.

    Now that you have a main menu defined it still needs to be registered with
    the CUI.

    @code
    if (CUI_SUCCESS != CUI_registerMenu(clientHandle, &myMainMenu) {
        //handle failure
    }
    @endcode

    As long as no more than MAX_REGISTERED_MENUS have already been registered and
    your menu was defined correctly the registration will be successful and your
    main menu will appear over UART.

    Notice how there is the capability to register MAX_REGISTERED_MENUS?
    This is a feature so that when two examples are merged together to create
    a DMM (Dynamic Multi protocol Manager) example, the menus of each standalone
    application's menu can exist safely without impacting the other menu. When
    multiple menus are registered the CUI will introduce a new top most main
    menu. This main menu will then have a sub menu for each menu that was
    registered with the CUI. The title of this new multi-menu will default to
    " TI DMM Application ", but can be changed by providing the address to a
    different valid c-string to the CUI_updateMultiMenuTitle() function.

    @code
    const char newTitle[] = " My First Multi Menu Project ";
    CUI_updateMultiMenuTitle(newTitle);
    @endcode
    @endcode

    When developing an interceptable action, it is important to know what to
    expect to receive as input. Your interceptable action should match the
    CUI_pFnIntercept_t typdef.
        typedef void (*CUI_pFnIntercept_t)(const char _input, char* _lines[3], CUI_cursorInfo_t * _curInfo);
    The first parameter, _input, will be a single byte indicating what type of
    input the user has pressed. There are a few special input types for your use.

    @code
    CUI_ITEM_PREVIEW
    CUI_ITEM_INTERCEPT_START
    CUI_ITEM_INTERCEPT_STOP
    CUI_ITEM_INTERCEPT_CANCEL
    CUI_INPUT_UP
    CUI_INPUT_DOWN
    CUI_INPUT_RIGHT
    CUI_INPUT_LEFT
    CUI_INPUT_BACK
    CUI_INPUT_ESC
    @endcode

    CUI_ITEM_PREVIEW allows your action  to display some information on the
    menu without the user executing your action. This is useful for
    configurable application variables such as panId. When the user navigates
    over top of the theoretical "< PAN ID >" screen, it could also display what
    the current panId is without the need for the user to execute on the action.
    In the case of a CUI_ITEM_PREVIEW event, your interceptable action will only
    be able to provide a preview on the first two lines. The third line will be
    maintained by the cui to be the item's description regardless of what you
    put in the third line.

    CUI_ITEM_INTERCEPT_START is an indication to your action that the user has
    decided to execute on your action. This is where you could perform some sort
    of setup steps for the lifetime of the action if necessary.

    CUI_ITEM_INTERCEPT_STOP is an indication to your action that the user has
    completed their time in your action. This is where you could perform some
    clean up code or finalization code for the end of the action.

    CUI_ITEM_INTERCEPT_CANCEL is an indication to your action that the user
    decided to cancel any current progress within your action. What
    should/can be done in this case depends on what your action is doing.
    (i.e) If your action is modifying the Pan ID. CUI_ITEM_INTERCEPT_START
    could clear the local copy of the pan ID, the user can modify the local copy.
    Then if you receive a CUI_ITEM_INTERCEPT_CANCEL input, you should clear out
    the local copy and make sure not to save the changes the user had made.

    CUI_ITEM_INTERCEPT_START and CUI_ITEM_INTERCEPT_STOP are useful for
    configurable variables such as panID because when CUI_ITEM_INTERCEPT_START
    occurs you can obtain the latest panId from the stack and save a static local
    copy of it during the lifetime of the action. Then while the user is modifying
    the panId you are only updating the static local version of the variable.
    Finally, when the user has completed the modifications to the panId,
    CUI_ITEM_INTERCEPT_STOP will come through and you can set the new panId as the
    official panId in the stack. This process allows you to limit the number of
    stack APIs you need to call.

    The UP, DOWN, RIGHT, LEFT inputs come directly from the arrow keys on the
    user's keyboard. Similarly, the BACK input is from the backspace key on the
    keyboard. As with all of these inputs. You as the developer of the
    interceptable action get to choose whether to act on them. They are only
    there if you want to use them.

    Besides those special inputs, any ascii characters the user types will be
    provided to you via the same _input variable. It is again up to you as the
    developer to handle this input as you deem necessary. A few more macros
    are provided to you in order to simplify your logic. You can use these
    to determine if the input is what you expect or not. They will return
    TRUE or FALSE depending on the _input provided.
        CUI_IS_INPUT_NUM(_input)
        CUI_IS_INPUT_ALPHA(_input)
        CUI_IS_INPUT_ALPHA_NUM(_input)
        CUI_IS_INPUT_HEX(_input)
        CUI_IS_INPUT_BINARY(_input)

    The second param to CUI_pFnIntercept_t are the _pLines[3] that the menu will
    print out. By putting c strings in these buffers you can share any
    information with the user while they are either previewing your action or
    during the interceptable period of your action. When writing data to these
    buffers take care not to write more than MAX_MENU_LINE_LEN per line. This
    will cause undefined behavior and possibly cause your application to assert.
    These buffers are provided to you out of kindness, so treat them with the
    due respect and the CUI will behave accordingly :)

    The last param to CUI_pFnIntercept_t is a pointer to a CUI_cursorInfo_t
    struct. This is here only if you want to use it. If you do not modify this
    struct then no behavior will change. Though if you would like a cursor to
    appear on the menu to indicate to the user where some information is, then
    you can set the cursor.col and cursor.row values. By doing this the CUI will
    maintain the cursor on the screen as you have requested. There is no need to
    turn the cursor off as after receiving an input of CUI_ITEM_INTERCEPT_STOP
    the CUI will automatically remove the cursor. In the case of CUI_ITEM_PREVIEW,
    the cursor will be ignored. This means that the cursor can only be on the
    screen as long as the user is currently using your interceptable item action.

--------------------------------------------------------------------------------
**/
#ifndef CUI_H
#define CUI_H

#include <stdlib.h>
#include <stdint.h>
#include <ti/drivers/apps/Button.h>
#include <ti/drivers/apps/LED.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * MACROS
 */
/*
 * CUI Module configurable defines. These can be used by the user allow for
 * things such as more clients to be opened or changing the maximum length of a
 * menu line.
 *
 * !!!!
 * !!!! Do not modify these values directly. Either use an .opt file or Project
 * !!!!     level defines
 * !!!!
 *
 * By increasing these values the cui module will need to use more memory.
 * You can therefore reduce the amount of memory this module will require by
 * decreasing these values.
 *
 * RAM Usage per client         = ~56 bytes
 * RAM Usage per menus          = ~20 bytes
 * RAM Usage per status lines   = ~76 bytes
 *
 * Reducing MAX_*_LEN defines will reduce the RAM usage, but the reduced memory
 * value will not scale to the number of clients/menus/status lines. In most
 * situations changing MAX_*_LEN defines will not produce any appreciable memory
 * savings.
 */

/*
 * Client Configurable Defines
 */
#ifndef MAX_CLIENTS
#define MAX_CLIENTS                 2
#endif
#ifndef MAX_CLIENT_NAME_LEN
#define MAX_CLIENT_NAME_LEN         64
#endif


/*
 * Menu Configurable Defines
 */
#ifndef MAX_REGISTERED_MENUS
#define MAX_REGISTERED_MENUS        4
#endif
#ifndef MAX_MENU_LINE_LEN
#define MAX_MENU_LINE_LEN           128
#endif

/*
 * Status Line Configurable Defines
 */
#ifndef MAX_STATUS_LINE_LABEL_LEN
#define MAX_STATUS_LINE_LABEL_LEN   32
#endif
#ifndef MAX_STATUS_LINE_VALUE_LEN
#define MAX_STATUS_LINE_VALUE_LEN   128
#endif

#ifndef CUI_MIN_FOOTPRINT
/*
 * Creates a main menu. A main menu must have a non NULL uart update function.
 * This will be verified when registering the menu. numItems is incremented by
 * one to allow for a common default "Back" or "Help" menu item between menus.
 */
#define CUI_MAIN_MENU(_menuSymbol, _pMenuTitle, _numItems, _pMenuUpdateFn) \
        CUI_menu_t _menuSymbol = { \
        .uartUpdateFn=_pMenuUpdateFn, \
        .pTitle=_pMenuTitle, \
        .numItems=_numItems + 1, \
        .pUpper=NULL, \
            .menuItems = {
/*
 * Creates a sub menu. This will be verified when registering the menu. numItems
 *  is incremented by one to allow for a common default "Back" or "Help" menu
 *  item between menus.
 */
#define CUI_SUB_MENU(_menuSymbol, _pMenuTitle, _numItems, _pUpperMenu) \
        extern CUI_menu_t _pUpperMenu; \
        CUI_menu_t _menuSymbol = { \
        .uartUpdateFn=NULL, \
        .pTitle=_pMenuTitle, \
        .numItems=_numItems + 1, \
        .pUpper=&_pUpperMenu, \
            .menuItems = {

/*
 * Inserts _pSubMenu into the .menuItems[] of a parent menu.
 */
#define CUI_MENU_ITEM_SUBMENU(_pSubMenu) { \
        .pDesc=NULL, \
        .itemType=CUI_MENU_ITEM_TYPE_SUBMENU, \
        .item.pSubMenu=(&_pSubMenu)},

/*
 * Inserts an action into the .menuItems[] of a parent menu.
 */
#define CUI_MENU_ITEM_ACTION(_pItemDesc, _pFnAction) { \
        .pDesc=(_pItemDesc), \
        .itemType=CUI_MENU_ITEM_TYPE_ACTION, \
        .interceptActive=false, \
        .item.pFnAction=(_pFnAction)},

/*
 * Inserts an interceptable action into the .menuItems[] of a parent menu.
 */
#define CUI_MENU_ITEM_INT_ACTION(_pItemDesc, _pFnIntercept) { \
            .pDesc=(_pItemDesc), \
            .itemType=CUI_MENU_ITEM_TYPE_INTERCEPT, \
            .interceptActive=false, \
            .item.pFnIntercept=(_pFnIntercept)},

/*
 * Inserts a list action into the .menuItems[] of a parent menu.
 */
#define CUI_MENU_ITEM_LIST_ACTION(_pItemDesc, _maxListItems, _pFnListAction) { \
            .pDesc=(_pItemDesc), \
            .itemType=CUI_MENU_ITEM_TYPE_LIST, \
            .interceptActive=false, \
            .item.pList=&((CUI_list_t){ \
                .pFnListAction=(_pFnListAction), \
                .maxListItems=_maxListItems, \
                .currListIndex=0})},

/*
 * Helper macros to add generic Help and Back screens to all menus
 * The CUI will use these for you. Do not use these in an application.
 */
#define CUI_MENU_ITEM_HELP CUI_MENU_ITEM_INT_ACTION(CUI_MENU_ACTION_HELP_DESC, (CUI_pFnIntercept_t) CUI_menuActionHelp)
#define CUI_MENU_ITEM_BACK CUI_MENU_ITEM_ACTION(CUI_MENU_ACTION_BACK_DESC, (CUI_pFnAction_t) CUI_menuActionBack)
#define CUI_MAIN_MENU_END CUI_MENU_ITEM_HELP }};
#define CUI_SUB_MENU_END CUI_MENU_ITEM_BACK }};
#define CUI_MENU_ACTION_BACK_DESC  "<      BACK      >"
#define CUI_MENU_ACTION_HELP_DESC  "<      HELP      >"
#else

#define CUI_MAIN_MENU(_menuSymbol, _pMenuTitle, _numItems, _pMenuUpdateFn) \
        CUI_menu_t _menuSymbol;

#define CUI_SUB_MENU(_menuSymbol, _pMenuTitle, _numItems, _pUpperMenu) \
        CUI_menu_t _menuSymbol;

#define CUI_MENU_ITEM_SUBMENU(_pSubMenu)

#define CUI_MENU_ITEM_ACTION(_pItemDesc, _pFnAction)

#define CUI_MENU_ITEM_INT_ACTION(_pItemDesc, _pFnIntercept)

#define CUI_MENU_ITEM_LIST_ACTION(_pItemDesc, _maxListItems, _pFnListAction)

#define CUI_MENU_ITEM_HELP
#define CUI_MENU_ITEM_BACK
#define CUI_MAIN_MENU_END
#define CUI_SUB_MENU_END
#define CUI_MENU_ACTION_BACK_DESC  "<      BACK      >"
#define CUI_MENU_ACTION_HELP_DESC  "<      HELP      >"
#endif

#define CUI_IS_INPUT_NUM(_input)        ((_input >= '0') && (_input <= '9'))
#define CUI_IS_INPUT_ALPHA(_input)      ((_input >= 'a') && (_input <= 'z'))
#define CUI_IS_INPUT_ALPHA_NUM(_input)  ((CUI_IS_INPUT_ALPHA(_input)) && (CUI_IS_INPUT_NUM(_input)))
#define CUI_IS_INPUT_HEX(_input)        ((CUI_IS_INPUT_NUM(_input)) || ((_input >= 'a') && (_input <= 'f')))
#define CUI_IS_INPUT_BINARY(_input)     ((_input == '0') || (_input == '1'))



/* Indication of previewing an interceptable item */
#define CUI_ITEM_PREVIEW            0x00

/* Indication item is now intercepting the uart */
#define CUI_ITEM_INTERCEPT_START    0xFE

/* Indication item is done intercepting the uart */
#define CUI_ITEM_INTERCEPT_STOP     0xFF

/* Indication item intercept should be canceled */
#define CUI_ITEM_INTERCEPT_CANCEL   0xF9

#define CUI_INPUT_UP                0xFA // Up Arrow
#define CUI_INPUT_DOWN              0xFB // Down Arrow
#define CUI_INPUT_RIGHT             0xFC // Right Arrow
#define CUI_INPUT_LEFT              0xFD // Left Arrow
#define CUI_INPUT_BACK              0x7F // Backspace Key
#define CUI_INPUT_EXECUTE           0x0D // Enter Key
#define CUI_INPUT_ESC               0x1B // ESC (escape) Key

#define CUI_COLOR_RESET             "\033[0m"
#define CUI_COLOR_RED               "\033[31m"
#define CUI_COLOR_GREEN             "\033[32m"
#define CUI_COLOR_YELLOW            "\033[33m"
#define CUI_COLOR_BLUE              "\033[34m"
#define CUI_COLOR_MAGENTA           "\033[35m"
#define CUI_COLOR_CYAN              "\033[36m"
#define CUI_COLOR_WHITE             "\033[37m"

#define CUI_DEBUG_MSG_START         "\0337"
#define CUI_DEBUG_MSG_END           "\0338"

/******************************************************************************
 * TYPEDEFS
 */

/*
 * [Return Types]
 */
typedef enum CUI_retVal
{
  CUI_SUCCESS,
  CUI_FAILURE,
  CUI_INVALID_CB,
  CUI_RESOURCE_ALREADY_ACQUIRED,
  CUI_RESOURCE_NOT_ACQUIRED,
  CUI_MODULE_UNINITIALIZED,
  CUI_INVALID_CLIENT_HANDLE,
  CUI_MAX_CLIENTS_REACHED,
  CUI_NO_ASYNC_LINES_RELEASED,
  CUI_INVALID_LINE_ID,
  CUI_UNKOWN_VALUE_TYPE,
  CUI_UART_FAILURE,
  CUI_INVALID_PARAM,
  CUI_MAX_MENUS_REACHED,
  CUI_PREV_WRITE_UNFINISHED,
  CUI_MISSING_UART_UPDATE_FN,
  CUI_NOT_MANAGING_UART
} CUI_retVal_t;

/*
 * [General CUI Types]
 */
typedef uint32_t CUI_clientHandle_t;

typedef struct {
    bool manageUart;
} CUI_params_t;

typedef struct {
    char clientName[MAX_CLIENT_NAME_LEN];
    uint8_t maxStatusLines;
} CUI_clientParams_t;

/*
 * [Menu Related Types]
 */
typedef struct {
    int16_t row;
    int16_t col;
} CUI_cursorInfo_t;

typedef void (*CUI_pFnClientMenuUpdate_t)(void);

/* Type definitions for action functions types */
typedef void (*CUI_pFnAction_t)(const int32_t _itemEntry);
typedef void (*CUI_pFnIntercept_t)(const char _input, char* _lines[3], CUI_cursorInfo_t * _curInfo);
typedef void (*CUI_pFnListAction_t)(const uint32_t _listIndex, char* _lines[3], bool _selected);

typedef struct CUI_menu_s CUI_menu_t;
typedef struct CUI_list_s CUI_list_t;

typedef enum CUI_menuItems{
    CUI_MENU_ITEM_TYPE_SUBMENU,
    CUI_MENU_ITEM_TYPE_ACTION,
    CUI_MENU_ITEM_TYPE_INTERCEPT,
    CUI_MENU_ITEM_TYPE_LIST
}CUI_itemType_t;

/* Type definition for a sub menu/action item entry */
typedef struct {
    char* pDesc;                      /* action description. NULL for sub menu */
    CUI_itemType_t itemType;          /* What type of menu item is this */
    bool interceptActive;             /* Is item currently being intercepted */
    union {
        CUI_menu_t* pSubMenu;               /* Sub menu */
        CUI_pFnAction_t   pFnAction;        /* Function for action */
        CUI_pFnIntercept_t pFnIntercept;    /* Function for interceptable action */
        CUI_list_t* pList;                  /* List */
    } item;
} CUI_menuItem_t;

/* Type definition for a menu object */
struct CUI_menu_s {
    CUI_pFnClientMenuUpdate_t uartUpdateFn;     /* Uart Update function */
    const char* pTitle;                         /* Title of this menu */
    uint8_t numItems;                           /* # of item entries */
    CUI_menu_t*  pUpper;                        /* upper menu */
    CUI_menuItem_t menuItems[];                 /* item entries */
};

/* Type definition for a list object */
struct CUI_list_s {
    CUI_pFnListAction_t pFnListAction;
    uint16_t maxListItems;
    uint16_t currListIndex;
};

/******************************************************************************
 Function Prototypes
 *****************************************************************************/

/******************************************************************************
 * General CUI APIs
 *****************************************************************************/
/*********************************************************************
 * @fn          CUI_init
 *
 * @brief       Initialize the CUI module. This function must be called
 *                  before any other CUI functions.
 */
CUI_retVal_t CUI_init(CUI_params_t* _pParams);

/*********************************************************************
 * @fn          CUI_paramsInit
 *
 * @brief       Initialize a CUI_params_t struct to a known state.
 *                  The known state in this case setting each resource
 *                  management flag to true
 */
void CUI_paramsInit(CUI_params_t* _pParams);

/*********************************************************************
 * @fn          CUI_clientOpen
 *
 * @brief       Open a client with the CUI module. A client is required
 *                  to request/acquire resources
 */
CUI_clientHandle_t CUI_clientOpen(CUI_clientParams_t* _pParams);

/*********************************************************************
 * @fn          CUI_clientParamsInit
 *
 * @brief       Initialize a CUI_clientParams_t struct to a known state.
 */
void CUI_clientParamsInit(CUI_clientParams_t* _pClientParams);

/*********************************************************************
 * @fn          CUI_close
 *
 * @brief       Close the CUI module. Release all resources and memory.
 */
CUI_retVal_t CUI_close();

/******************************************************************************
 * Menu CUI APIs
 *****************************************************************************/
/*********************************************************************
 * @fn          CUI_registerMenu
 *
 * @brief       Register a menu with the CUI module
 */
CUI_retVal_t CUI_registerMenu(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu);

/*********************************************************************
 * @fn          CUI_deRegisterMenu
 *
 * @brief       De-registers a menu with the CUI module
 */
CUI_retVal_t CUI_deRegisterMenu(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu);

/*********************************************************************
 * @fn          CUI_updateMultiMenuTitle
 *
 * @brief       Changes the default multi menu title
 */
CUI_retVal_t CUI_updateMultiMenuTitle(const char* _pTitle);

/*********************************************************************
 * @fn          CUI_menuNav
 *
 * @brief       Navigate to a specific entry of a menu that has already been
 *              registered
 */
CUI_retVal_t CUI_menuNav(const CUI_clientHandle_t _clientHandle, CUI_menu_t* _pMenu, const uint32_t _itemIndex);

/*********************************************************************
 * @fn          CUI_processMenuUpdate
 *
 * @brief       This function should be called whenever there is UART input
 *                  to be processed.
 */
CUI_retVal_t CUI_processMenuUpdate(void);

/******************************************************************************
 * Status Line CUI APIs
 *****************************************************************************/
/*********************************************************************
 * @fn          CUI_statusLineResourceRequest
 *
 * @brief       Request access to a new status line
 */
CUI_retVal_t CUI_statusLineResourceRequest(const CUI_clientHandle_t _clientHandle, const char _pLabel[MAX_STATUS_LINE_LABEL_LEN], const bool _refreshInd, uint32_t* _pLineId);

/*********************************************************************
 * @fn          CUI_statusLinePrintf
 *
 * @brief        Update an acquired status line
 */
CUI_retVal_t CUI_statusLinePrintf(const CUI_clientHandle_t _clientHandle, const uint32_t _lineId, const char *format, ...);

void CUI_wrappedIncrement(size_t* _pValue, int32_t _incAmt, size_t _maxValue);
/*********************************************************************
 * Assert Debug API
 ********************************************************************/
/*********************************************************************
 *  @fn         CUI_assert
 *
 *  @brief      Without requiring a cuiHandle_t you may print an assert
 *                string and optionally spinLock while flashing the leds.
 *
 *              Note: If you choose to spinLock, this function will close
 *                all existing clients that have been opened and then
 *                enter an infinite loop that flashes the leds.
 */
void CUI_assert(const char* _assertMsg, const bool _spinLock);
#ifndef CUI_MIN_FOOTPRINT
void CUI_menuActionBack(const int32_t _itemEntry);
void CUI_menuActionHelp(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
#endif
#ifdef __cplusplus
}
#endif

#endif /* CUI_H */
