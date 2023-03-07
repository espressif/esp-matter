/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "cJSON.h"
#include "fsl_debug_console.h"

#include "fsl_device_registers.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_UART          UART2
#define DEMO_UART_CLK_FREQ CLOCK_GetUartClkFreq(2)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static bool create_json(void);
static bool parse_json(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
char *string                 = NULL;
const char *g_demoStrings[4] = {"This", "is", "cJSON", "demo"};
const char *g_category       = "demo_apps";
const char *g_name           = "cjson";

/*******************************************************************************
 * Code
 ******************************************************************************/
static bool create_json(void)
{
    cJSON *root          = NULL;
    cJSON *sdk_version   = NULL;
    cJSON *cjson_version = NULL;
    cJSON *example_info  = NULL;
    cJSON *category      = NULL;
    cJSON *name          = NULL;
    cJSON *demo_strings  = NULL;

    /* Create JSON root object */
    root = cJSON_CreateObject();
    if (root == NULL)
    {
        goto fail;
    }

    /* Create sdk version and added it to the root object */
    sdk_version = cJSON_CreateNumber(2);
    if (sdk_version == NULL)
    {
        goto fail;
    }
    cJSON_AddItemToObject(root, "sdk version", sdk_version);

    /* Add cJSON version to the root object */
    cjson_version = cJSON_AddStringToObject(root, "cJSON version", cJSON_Version());
    if (cjson_version == NULL)
    {
        goto fail;
    }

    /* Create example_info object and add it to the root object */
    example_info = cJSON_AddObjectToObject(root, "example info");
    if (example_info == NULL)
    {
        goto fail;
    }

    /* Create category and name of string and add them to the example_info */
    category = cJSON_AddStringToObject(example_info, "category", "demo_apps");
    if (category == NULL)
    {
        goto fail;
    }
    name = cJSON_AddStringToObject(example_info, "name", "cjson");
    if (name == NULL)
    {
        goto fail;
    }

    /* Add demo array to the root object */
    demo_strings = cJSON_CreateStringArray(g_demoStrings, 4);
    if (demo_strings == NULL)
    {
        goto fail;
    }
    cJSON_AddItemToObject(root, "demo strings", demo_strings);

    /* Print the object to string */
    string = cJSON_Print(root);
    if (string == NULL)
    {
        PRINTF("Failed to print JSON to data buffer!\r\n");
        goto fail;
    }
    cJSON_Delete(root);
    return true;

fail:
    /* Any error occured during the creation of the JSON or its subnode, delete the JSON */
    cJSON_Delete(root);
    PRINTF("Failed to create JSON!\r\n");
    return false;
}

static bool parse_json(void)
{
    cJSON *sdk_version      = NULL;
    cJSON *cjson_version    = NULL;
    cJSON *demo_strings     = NULL;
    cJSON *example_info     = NULL;
    cJSON *example_category = NULL;
    cJSON *example_name     = NULL;

    /* Parse the string as JSON */
    cJSON *info = cJSON_Parse(string);
    if (info == NULL)
    {
        /* Check where the error occurs */
        char const *err = cJSON_GetErrorPtr();
        if (err != NULL)
        {
            PRINTF("Error occurred when parsing before:\r\n");
            PRINTF("%s\r\n", err);
        }
        goto fail;
    }

    /* Checks whether the info obtained from the JSON is correct. */

    /* Checks sdk version */
    sdk_version = cJSON_GetObjectItemCaseSensitive(info, "sdk version");
    if (!cJSON_IsNumber(sdk_version) || (sdk_version->valueint != 2U))
    {
        PRINTF("Error parsing sdk version.\r\n");
        goto fail;
    }

    /* Checks cJSON version */
    cjson_version = cJSON_GetObjectItemCaseSensitive(info, "cJSON version");
    if (!cJSON_IsString(cjson_version) || (0U != strcmp(cjson_version->valuestring, cJSON_Version())))
    {
        PRINTF("Error parsing cJSON version.\r\n");
        goto fail;
    }

    /* Checks example info */
    example_info = cJSON_GetObjectItemCaseSensitive(info, "example info");
    if (!cJSON_IsObject(example_info))
    {
        PRINTF("Error parsing example info.\r\n");
        goto fail;
    }

    /* Checks example category */
    example_category = cJSON_GetObjectItemCaseSensitive(example_info, "category");
    if (!cJSON_IsString(example_category) || (0U != strcmp(example_category->valuestring, g_category)))
    {
        PRINTF("Error parsing example category.\r\n");
        goto fail;
    }

    /* Checks example name */
    example_name = cJSON_GetObjectItemCaseSensitive(example_info, "name");
    if (!cJSON_IsString(example_name) || (0U != strcmp(example_name->valuestring, g_name)))
    {
        PRINTF("Error parsing example name.\r\n");
        goto fail;
    }

    /* Check tool chains */
    demo_strings = cJSON_GetObjectItemCaseSensitive(info, "demo strings");
    if (!cJSON_IsArray(demo_strings) || (cJSON_GetArraySize(demo_strings) != ARRAY_SIZE(g_demoStrings)))
    {
        PRINTF("Demo string info is wrong.\r\n");
        goto fail;
    }
    for (uint8_t i = 0U; i < ARRAY_SIZE(g_demoStrings); i++)
    {
        cJSON *demo_string = cJSON_GetArrayItem(demo_strings, (int)i);
        if ((demo_string == NULL) || (!cJSON_IsString(demo_string)))
        {
            PRINTF("Demo string info is wrong.\r\n");
            goto fail;
        }
        if (0U != strcmp(demo_string->valuestring, g_demoStrings[i]))
        {
            PRINTF("Demo string info is wrong.\r\n");
            goto fail;
        }
    }

    return true;

fail:
    cJSON_Delete(info);
    return false;
}

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    PRINTF("This is cJSON example.\r\n");
    PRINTF(
        "This example constructs a JSON stucture using cJSON, print it to string and then parses the string as JSON "
        "and checks the info.\r\n");

    /* 1. Construct JSON as follows and print it to a string */
    /*
     * {
     *    "sdk version": 2,
     *    "cJSON version": "x.y.z",
     *    "example info": {
     *        "category": "demo_apps",
     *        "name": "cjson",
     *    }
     *    "demo strings":	["This", "is", "cJSON", "demo"]
     * }
     */
    if (!create_json())
    {
        return -1;
    }
    PRINTF("JSON created successfully:\r\n");
    PRINTF(string);
    PRINTF("\r\n");

    /* 2. Parse the string as JSON and checks the data */
    if (parse_json())
    {
        PRINTF("JSON parsed successfully.\r\n");
    }
    else
    {
        PRINTF("Error occured during parsing!\r\n");
    }

    while (1)
    {
    }
}
