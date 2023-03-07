/*
 * Copyright (c) 2014-2018, Texas Instruments Incorporated
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
/**
 *  @file  ti/utils/json/json.h
 *
 *  @addtogroup ti_utils_json_JSON JSON Parser/Builder
 *
 *  @brief      The JSON library provides APIs to parse and build
 *              JSON objects.
 *
 *  To access the JSON APIs, the application should include its header file
 *  as follows:
 *  @code
 *  #include <ti/utils/json/json.h>
 *  @endcode
 *
 *  And add the following JSON library to the link line:
 *  @code
 *  .../source/ti/utils/json/{toolchain}/{isa}/json_{profile}.a
 *  @endcode
 *
 *  # Usage #
 *  This section provides a basic @ref ti_utils_JSON_Synopsis "usage summary"
 *  and a set of @ref ti_utils_JSON_Examples "examples" in the form of commented
 *  code fragments. Detailed descriptions of the JSON APIs are provided in
 *  subsequent sections.
 *
 *  @anchor ti_utils_JSON_Synopsis
 *  ### Synopsis ###
 *
 *  @code
 *  // Import the json library
 *  #include <ti/utils/json/json.h>
 *
 *  #define EXAMPLE_JSONBUF     \
 *  "{"                         \
 *      "\"name\": \"John\","   \
 *      "\"age\": 32,"          \
 *      "\"job\": \"Plumber\""  \
 *  "}"
 *
 *  // Define a template that captures the structure of the JSON to be created or parsed
 *  #define JSON_TEMPLATE   \
 *  "{"                     \
 *      "\"name\": string," \
 *      "\"age\": int32,"   \
 *      "\"job\": string"   \
 *  "}"
 *
 *  // Parse the template into memory
 *  Json_createTemplate(&templateHandle, JSON_TEMPLATE, strlen(JSON_TEMPLATE));
 *
 *  // Allocate memory needed for an object matching the generated template
 *  Json_createObject(&objectHandle, templateHandle, 0);
 *
 *  // Parse JSON data into memory
 *  Json_parse(objectHandle, EXAMPLE_JSONBUF, strlen(EXAMPLE_JSONBUF));
 *
 *  // Set values within the JSON object
 *  Json_setValue(objectHandle, "\"name\"", "Bart", strlen("Bart"));
 *
 *  // Get values from within the JSON object
 *  Json_getValue(objectHandle, "\"age\"", outputBuffer, &bufsize);
 *
 *  // Transform the JSON object into a string
 *  Json_build(objectHandle, outputBuffer, &bufsize);
 *
 *  // Destroy the template and free its memory
 *  Json_destroyTemplate(templateHandle);
 *
 *  // Destroy the JSON object and free its memory
 *  Json_destroyObject(objectHandle);
 *  @endcode
 *
 *  @anchor ti_utils_JSON_Examples
 *  ### Examples ###
 *  * @ref ti_utils_JSON_Example_create "Creating JSON Objects"
 *  * @ref ti_utils_JSON_Example_parse "Parsing JSON Objects"
 *  * @ref ti_utils_JSON_Example_edit "Editing JSON Objects"
 *
 *  The steps of creating a template and allocating memory for a JSON object
 *  through Json_createTemplate() and Json_createObject() are required for
 *  interacting with JSON objects. The template is what the library uses to know
 *  how to parse the JSON it encounters and to format the JSON string it
 *  ultimately builds through Json_build().
 *
 *  @anchor ti_utils_JSON_Example_create
 *  **Creating JSON Objects**: The following example demonstrates how to create
 *  a JSON object and output it as a string.
 *  @code
 *  #include <ti/utils/json/json.h>
 *
 *  // The template is needed to specify the structure of the json object - its
 *  // field names and types
 *  #define EXAMPLE_TEMPLATE    \
 *  "{"                         \
 *      "\"name\": string,"     \
 *      "\"age\": int32,"       \
 *      "\"job\": string,"      \
 *      "\"citizen\": boolean"  \
 *  "}"
 *
 *  void mainThread() {
 *      Json_Handle templateHandle;
 *      Json_Handle objectHandle;
 *      bufsize     = 256;
 *      char        outputBuffer[bufsize];
 *
 *      // The name must be quoted to access the corresponding JSON field
 *      char        *fieldName  = "\"age\"";
 *      int32_t     ageVal      = 42;
 *      bool        citizenship = true;
 *
 *      // Create an internal representation of the template for the library's use
 *      Json_createTemplate(&templateHandle, EXAMPLE_TEMPLATE,
 *                          strlen(EXAMPLE_TEMPLATE));
 *
 *      // Allocate memory needed for an object matching the generated template
 *      Json_createObject(&objectHandle, templateHandle, 0);
 *
 *      // To fill in the object with actual data, call Json_setValue
 *      Json_setValue(objectHandle, fieldName, &ageVal, sizeof(ageVal));
 *
 *      fieldName = "\"citizen\"";
 *      Json_setValue(objectHandle, fieldName, &citizenship, sizeof(uint16_t));
 *
 *      // Output data from the JSON objectHandle into the outputBuffer
 *      Json_build(objectHandle, outputBuffer, &bufsize);
 *      // Any fields not set will not be part of the output
 *  }
 *  @endcode
 *
 *  @anchor ti_utils_JSON_Example_parse
 *  **Parsing JSON Objects**: The following example demonstrates how to parse
 *  a JSON string into memory as a JSON object.
 *  @code
 *  #include <ti/utils/json/json.h>
 *
 *  #define EXAMPLE_JSONBUF     \
 *  "{"                         \
 *      "\"name\": \"John\","   \
 *      "\"age\": 32,"          \
 *      "\"job\": \"Plumber\"," \
 *      "\"citizen\": true"     \
 *  "}"
 *
 *  // The template is needed to specify the structure of the json object
 *  #define EXAMPLE_TEMPLATE    \
 *  "{"                         \
 *      "\"name\": string,"     \
 *      "\"age\": int32,"       \
 *      "\"job\": string,"      \
 *      "\"citizen\": boolean"  \
 *  "}"
 *
 *  void mainThread() {
 *      Json_Handle templateHandle;
 *      Json_Handle objectHandle;
 *      uint16_t    bufsize = 32;
 *      char        outputBuffer[bufsize];
 *      char        *fieldName = "\"job\"";
 *      bool        boolBuffer;
 *      uint16_t    boolBufSize = sizeof(uint16_t);
 *
 *      // Create an internal representation of the template for the library's use
 *      Json_createTemplate(&templateHandle, EXAMPLE_TEMPLATE,
 *                          strlen(EXAMPLE_TEMPLATE));
 *
 *      // Allocate memory needed for an object matching the generated template
 *      Json_createObject(&objectHandle, templateHandle, 0);
 *
 *      // Parse the JSON and fill in the object
 *      Json_parse(objectHandle, EXAMPLE_JSONBUF, strlen(EXAMPLE_JSONBUF));
 *
 *      // Retrieve a value from the parsed json
 *      Json_getValue(objectHandle, fieldName, outputBuffer, &bufsize);
 *
 *      fieldName = "\"citizen\"";
 *      Json_getValue(objectHandle, fieldName, &boolBuffer, &boolBufSize);
 *  }
 *  @endcode
 *
 *  @anchor ti_utils_JSON_Example_edit
 *  **Editing JSON Objects**: The following example demonstrates parsing a
 *  string containing JSON, editing its values, and outputting the updated object.
 *  @code
 *  #include <ti/utils/json/json.h>
 *
 *  // The template is needed to specify the structure of the json object
 *  #define EXAMPLE_TEMPLATE    \
 *  "{"                         \
 *      "\"name\": string,"     \
 *      "\"age\": int32,"       \
 *      "\"job\": string"       \
 *  "}"
 *
 *  #define EXAMPLE_JSONBUF     \
 *  "{"                         \
 *      "\"name\": \"John\","   \
 *      "\"age\": 32,"          \
 *      "\"job\": \"Plumber\""  \
 *  "}"
 *
 *  void mainThread() {
 *      Json_Handle templateHandle;
 *      Json_Handle objectHandle;
 *      uint16_t    bufsize = 256;
 *      char        outputBuffer[bufsize];
 *
 *      // Create an internal representation of the template for the library's use
 *      Json_createTemplate(&templateHandle, EXAMPLE_TEMPLATE,
 *                          strlen(EXAMPLE_TEMPLATE));
 *
 *      // Allocate memory needed for an object matching the generated template
 *      Json_createObject(&objectHandle, templateHandle, 0);
 *
 *      // Parse the JSON
 *      Json_parse(objectHandle, EXAMPLE_JSONBUF, strlen(EXAMPLE_JSONBUF));
 *
 *      // Set a value in the JSON object
 *      Json_setValue(objectHandle, "\"job\"", "Contractor", strlen("Contractor"));
 *      Json_setValue(objectHandle, "\"name\"", "John Sr.", strlen("John Sr."));
 *
 *      // Output data from the JSON objectHandle into the outputBuffer
 *      Json_build(objectHandle, outputBuffer, &bufsize);
 *  }
 *  @endcode
 *
 *  When the JSON objects are no longer needed @ref Json_destroyObject and
 *  @ref Json_destroyTemplate should be called:
 *  @code
 *  void snippet() {
 *      Json_createTemplate(&templateHandle, EXAMPLE_TEMPLATE,
 *                          strlen(EXAMPLE_TEMPLATE));
 *
 *      Json_createObject(&objectHandle, templateHandle, 0);
 *
 *      ...
 *
 *      Json_destroyObject(objectHandle);
 *
 *      Json_destroyTemplate(templateHandle);
 *  }
 *  @endcode
 *
 *  @remark Floating point values are not parsed correctly and should not be
 *  used. This will be fixed in a future release and is tracked by TIUTILS-8.
 *
 *
 *  ============================================================================
 */

#ifndef ti_utils_json_Json__include
#define ti_utils_json_Json__include

/*! @ingroup ti_utils_json_JSON */
/*@{*/

/*
 *  ======== json.h ========
 *  @brief Contains JSON library APIs
 */

#include <stdlib.h>
#include <string.h>
#include "json_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t Json_Handle;

#define JSON_DEFAULT_SIZE                       (1024u)

/*!
 *  @brief      This function creates internal template from the
 *              template text.
 *
 *  @param[out] templateHandle      template handle
 *  @param[in]  templateString      json template text
 *  @param[in]  templateStringLen   json template text length
 *
 *  @remark     The user must free the created template using
 *              Json_destroyTemplate().
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  Json_Handle templateHandle1;
 *  char *templatestr = "{"
 *          "\"name\":string,"
 *          "\"age\":int32,"
 *          "\"car models\":[string,string,string]}";
 *
 *  ret = Json_createTemplate(&templateHandle1, templatestr,
 *          strlen(templatestr));
 *  @endcode
 *
 *  @sa Json_destroyTemplate()
 */
int16_t Json_createTemplate(Json_Handle *templateHandle,
    const char *templateString, uint16_t templateStringLen);


/*!
 *  @brief      This function frees the internal template memory
 *
 *  @param[in]  templateHandle      template handle
 *
 *  @remark     After destroying the template, the Json object will have a
 *              @c NULL pointer in it (see the object struct -
 *              JSON_templateInternal *jsonTemplate).  It is
 *              recommended to first destroy the Json object and then
 *              the template.
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;

 *  ret  = Json_destroyTemplate(templateHandle1);
 *  @endcode
 *
 *  @sa Json_createTemplate()
 */
int16_t Json_destroyTemplate(Json_Handle templateHandle);

/*!
 *  @brief      This function creates an empty Json object
 *
 *  @param[out]   objHandle         json object handle
 *  @param[in]    templateHandle    json template handle
 *  @param[in]    maxObjectSize     json object max size or 0 for
 *                                   #JSON_DEFAULT_SIZE
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @remark     The user must free the created object using Json_destroyObject()
 *
 *  @remark     The user must create a template before its json object
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  Json_Handle h;
 *
 *  ret = Json_createObject(&h, templateHandle, 1024);
 *  @endcode
 *
 *  @sa     Json_createTemplate()
 *  @sa     Json_destroyObject()
 */
int16_t Json_createObject(Json_Handle *objHandle, Json_Handle templateHandle,
        uint16_t maxObjectSize);


/*!
 *  @brief      Free the internal json memory
 *
 *  @param[in]  objHandle       json handle, created by Json_createObject()
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *
 *  ret = Json_destroyObject(h);
 *  @endcode
 *
 *  @sa     Json_createObject()
 */
int16_t Json_destroyObject(Json_Handle objHandle);

/*!
 *  @brief This function converts the json text into internal representation
 *
 *  @param[in]  objHandle       json object handle
 *  @param[in]  jsonText        pointer to the json text
 *  @param[in]  jsonTextLen     json text size
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  char *jsonBuf = "{\"name\":\"John\","
 *          "\"age\":30,"
 *          "\"car models\":[\"toyota\",\"fiat\",\"volvo\"]}";
 *
 *  ret = Json_parse(h, jsonBuf, strlen(jsonBuf));
 *  @endcode
 *
 *  @sa     Json_createObject()
 */
int16_t Json_parse(Json_Handle objHandle, char *jsonText, uint16_t jsonTextLen);


/*!
 *  @brief      Retrieve the number of array elements in the provided key
 *
 *  @param[in]  objHandle       json object handle
 *  @param[in]  pKey            pointer to the key of the requested array
 *
 *  @remark     @c pKey must be NULL terminated.
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  uint16_t arrayCount;
 *  char *jsonBuf = "{"
 *          "\"name\":\"John\","
 *          "\"age\":30,"
 *          "\"car models\":[\"toyota\",\"fiat\",\"volvo\"]};
 *  char *key =  "\"car models\"";
 *
 *  ret = Json_parse(h, jsonBuf, strlen(jsonBuf));
 *
 *  arrayCount = Json_getArrayMembersCount(h, key);
 *  @endcode
 *
 *  @sa     Json_parse()
 */
int16_t Json_getArrayMembersCount(Json_Handle objHandle, const char *pKey);


/*!
 *  @brief This function retrieves value from json
 *
 *  @param[in]      objHandle       json object handle
 *  @param[in]      pKey            pointer to the key of the requested value
 *  @param[out]     pValue          [optional] pointer to the retrieved value
 *  @param[in,out]  maxValueSize    input, number of bytes @c pValue can
 *                                  hold (if @c pValue is not NULL).
 *                                  output, number of bytes required
 *                                  to hold requested value.
 *
 *  @remark     @c pKey must be NULL terminated
 *
 *  @remark     if @c pValue is @c NULL, @c maxValueSize will return the
 *              requested value size
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  char *jsonBuf = "{"
 *          "\"name\":\"John\","
 *          "\"age\":30,"
 *          "\"car models\":[\"toyota\",\"fiat\",\"volvo\"]};
 *  char *key =  "\"name\"";
 *  char   value[5];
 *  uint16_t valueSize = 5;
 *
 *  ret = Json_parse(h, jsonBuf, strlen(jsonBuf));
 *
 *  ret = Json_getValue(h, key, value, &valueSize);
 *  @endcode
 *
 *  @sa     Json_parse()
 */
int16_t Json_getValue(Json_Handle objHandle, const char *pKey, void *pValue,
        uint16_t *maxValueSize);


/*!
 *  @brief      Sets the value for the provided key
 *
 *  @param[in]  objHandle   json object handle
 *  @param[in]  pKey        pointer to the key of the value to be changed
 *  @param[in]  pValue      pointer to the value to be set
 *  @param[in]  valueSize   size of the value
 *
 *  @remark     @c pKey must be NULL terminated
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  char *jsonBuf = "{"
 *          "\"name\":\"John\","
 *          "\"age\":30,"
 *          "\"car models\":[\"toyota\",\"fiat\",\"volvo\"]};
 *  char *key =  "\"age\"";
 *  uint16_r value = 29;
 *  uint16_t valueSize = sizeof(value);
 *
 *  ret = Json_parse(h, jsonBuf, strlen(jsonBuf));
 *
 *  ret = Json_setValue(h, key, value, valueSize);
 *  @endcode
 *
 *  @remark For boolean types, valueSize should be set to 2 a.k.a. sizeof(uint16_t).
 *          For all other integral types, calling @c sizeof() on the respective
 *          type is sufficient.
 *
 *  @sa     Json_parse()
 */
int16_t Json_setValue(Json_Handle objHandle, const char *pKey, void *pValue,
        uint16_t valueSize);


/*!
 *  @brief This function builds the internal json into a text json
 *
 *  @param[in]      objHandle   json object handle
 *  @param[out]     pJsonText   pointer to buffer to output the json text
 *  @param[in,out]  maxTxtLen   input, maximum buffer size.
 *                              output, used buffer size.
 *
 *  @return     Success: #JSON_RC__OK
 *  @return     Failure: negative error code
 *
 *  @par        Example
 *  @code
 *  uint16_t ret;
 *  char *key =  "\"age\"";
 *  uint16_r value = 29;
 *  uint16_t valueSize = sizeof(value);
 *  char builtBuff[100];
 *  uint16_t builtBuffSize = 100;
 *  ret = Json_setValue(h, key, value, valueSize);
 *
 *  ret = Json_build(h, builtBuf, &builtBuffSize);
 *  @endcode
 *
 *  @sa     Json_parse()
 *  @sa     Json_setValue()
 */
int16_t Json_build(Json_Handle objHandle, char *pJsonText, uint16_t *maxTxtLen);

/*! @} */
#ifdef __cplusplus
}
#endif

#endif
