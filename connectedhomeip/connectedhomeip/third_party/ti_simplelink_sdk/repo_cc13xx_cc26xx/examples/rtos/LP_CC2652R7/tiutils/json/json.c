/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== json.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* Display Header files */
#include <ti/display/Display.h>

#include <ti/utils/json/json.h>

#define EXAMPLE_TEMPLATE                \
"{"                                     \
  "\"firstName\": string,"              \
  "\"lastName\": string,"               \
  "\"isAlive\": boolean,"               \
  "\"age\": int32,"                     \
  "\"address\": {"                      \
    "\"streetAddress\": string,"        \
    "\"city\": string,"                 \
    "\"state\": string,"                \
    "\"postalCode\": string"            \
  "},"                                  \
  "\"phoneNumbers\": ["                 \
    "{"                                 \
      "\"type\": string,"               \
      "\"number\": string"              \
    "},"                                \
    "{"                                 \
      "\"type\": string,"               \
      "\"number\": string"              \
    "},"                                \
    "{"                                 \
      "\"type\": string,"               \
      "\"number\": string"              \
    "}"                                 \
  "],"                                  \
  "\"children\": [raw],"                \
  "\"spouse\": boolean"                 \
"}"

#define EXAMPLE_JSONBUF                     \
"{"                                         \
  "\"firstName\": \"John\","                \
  "\"lastName\": \"Smith\","                \
  "\"isAlive\": true,"                      \
  "\"age\": 25,"                            \
  "\"address\": {"                          \
    "\"streetAddress\": \"21 2nd Street\"," \
    "\"city\": \"New York\","               \
    "\"state\": \"NY\","                    \
    "\"postalCode\": \"10021-3100\""        \
  "},"                                      \
  "\"phoneNumbers\": ["                     \
    "{"                                     \
      "\"type\": \"home\","                 \
      "\"number\": \"212 555-1234\""        \
    "},"                                    \
    "{"                                     \
      "\"type\": \"office\","               \
      "\"number\": \"646 555-4567\""        \
    "},"                                    \
    "{"                                     \
      "\"type\": \"mobile\","               \
      "\"number\": \"123 456-7890\""        \
    "}"                                     \
  "],"                                      \
  "\"children\": [],"                       \
  "\"spouse\": null"                        \
"}"

static char keyBuf[37];     /* max string to hold ("phoneNumbers".[X]."type") */
static char jsonBuf[1024];  /* max string to hold serialized JSON buffer */
static char phoneType[10];  /* max string to hold phone types (e.g. "mobile"); */

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    int i;
    void *valueBuf;
    int16_t retVal;
    int32_t age;
    int32_t phoneNumbers;
    uint16_t valueSize;
    uint16_t jsonBufSize;
    Json_Handle hTemplate;
    Json_Handle hObject;
    Display_Handle display;

    Display_init();

    /* Open an available UART display using default params. */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open a display */
        while (1);
    }

    /* create a template from a buffer containing a template */
    retVal = Json_createTemplate(&hTemplate, EXAMPLE_TEMPLATE,
            strlen(EXAMPLE_TEMPLATE));
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error creating the JSON template");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "JSON template created\n");
    }

    /* create a default-sized JSON object from the template */
    retVal = Json_createObject(&hObject, hTemplate, 0);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error creating JSON object");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "JSON object created from template\n");
    }

    /* parse EXAMPLE_JSONBUF */
    retVal = Json_parse(hObject, EXAMPLE_JSONBUF, strlen(EXAMPLE_JSONBUF));
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error parsing the JSON buffer");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "JSON buffer parsed\n");
    }

    /*
     * Get the "firstName" value.  Note that this approach first
     * detects the size of the result, then calloc()'s memory for it
     * and retrieves the value.
     */
    retVal = Json_getValue(hObject, "\"firstName\"", NULL, &valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error getting the firstName buffer size");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "firstName buffer size: %d\n", valueSize);
    }

    valueBuf = calloc(1, valueSize + 1);
    retVal = Json_getValue(hObject, "\"firstName\"", valueBuf, &valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error getting the firstName value");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "JSON buffer parsed, firstName: %s\n",
                valueBuf);
    }
    free(valueBuf);

    /* increment the "age" value by one */
    valueSize = sizeof(age);
    retVal = Json_getValue(hObject, "\"age\"", &age, &valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error getting the age value");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "age value: %d\n", age);
    }

    age++;

    valueSize = sizeof(age);
    retVal = Json_setValue(hObject, "\"age\"", &age, valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error setting the age value");
        while (1);
    }

    valueSize = sizeof(age);
    retVal = Json_getValue(hObject, "\"age\"", &age, &valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error getting the new age value");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "new age value: %d\n", age);
    }

    /* serialize the current JSON object (now with a new age) */
    jsonBufSize = 1024;
    retVal = Json_build(hObject, jsonBuf, &jsonBufSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error serializing JSON data");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "serialized data:\n%s", jsonBuf);
    }

    /* determine how many phone numbers are present */
    phoneNumbers = Json_getArrayMembersCount(hObject, "\"phoneNumbers\"");
    if (retVal != 0) {
        Display_printf(display, 0, 0,
                "Error getting phoneNumbers array size");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "phoneNumbers array size: %d\n",
                phoneNumbers);
    }

    /* print the mobile phone number */
    for (i = 0; i < phoneNumbers; i++) {
        sprintf(keyBuf, "\"phoneNumbers\".[%d].\"type\"", i);

        valueSize = sizeof(phoneType);
        memset(phoneType, 0, valueSize);

        retVal = Json_getValue(hObject, keyBuf, phoneType, &valueSize);
        if (retVal != 0) {
            Display_printf(display, 0, 0, "Error getting phoneNumbers[%d]",
                    i);
            while (1);
        }

        if (strcmp(phoneType, "mobile") == 0) {
            /* found it, get the value, then stop looking */
            sprintf(keyBuf, "\"phoneNumbers\".[%d].\"number\"", i);

            retVal = Json_getValue(hObject, keyBuf, NULL, &valueSize);

            valueBuf = calloc(1, valueSize + 1);
            retVal = Json_getValue(hObject, keyBuf, valueBuf, &valueSize);
            if (retVal != 0) {
                Display_printf(display, 0, 0,
                        "Error getting phoneNumbers[%d]", i);
                while (1);
            }
            else {
                Display_printf(display, 0, 0,
                        "mobile phone number: %s\n", valueBuf);
            }

            free(valueBuf);
            break;
        }
    }

    /* print the second (index #1) phone number */
    retVal = Json_getValue(hObject, "\"phoneNumbers\".[1].\"number\"", NULL,
            &valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error getting phoneNumbers[1]");
        while (1);
    }

    valueBuf = calloc(1, valueSize + 1);
    retVal = Json_getValue(hObject, "\"phoneNumbers\".[1].\"number\"", valueBuf,
            &valueSize);
    if (retVal != 0) {
        Display_printf(display, 0, 0, "Error getting phoneNumbers[1] value");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "phoneNumbers[1] value: %s\n", valueBuf);
    }
    free(valueBuf);

    /* done, cleanup */
    retVal = Json_destroyObject(hObject);

    retVal = Json_destroyTemplate(hTemplate);

    Display_printf(display, 0, 0, "Finished JSON example");

    return (0);
}
