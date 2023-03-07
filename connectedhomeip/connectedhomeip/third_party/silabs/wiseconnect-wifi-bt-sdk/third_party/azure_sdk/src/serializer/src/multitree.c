// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"

#include "multitree.h"
#include <string.h>
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/const_defines.h"

/*assume a name cannot be longer than 100 characters*/
#define INNER_NODE_NAME_SIZE 128

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(MULTITREE_RESULT, MULTITREE_RESULT_VALUES);

typedef struct MULTITREE_HANDLE_DATA_TAG
{
    char* name;
    void* value;
    MULTITREE_CLONE_FUNCTION cloneFunction;
    MULTITREE_FREE_FUNCTION freeFunction;
    size_t nChildren;
    struct MULTITREE_HANDLE_DATA_TAG** children; /*an array of nChildren count of MULTITREE_HANDLE_DATA*   */
}MULTITREE_HANDLE_DATA;


MULTITREE_HANDLE MultiTree_Create(MULTITREE_CLONE_FUNCTION cloneFunction, MULTITREE_FREE_FUNCTION freeFunction)
{
    MULTITREE_HANDLE_DATA* result;

    /* Codes_SRS_MULTITREE_99_052:[If any of the arguments passed to MultiTree_Create is NULL, the call shall return NULL.]*/
    if ((cloneFunction == NULL) ||
        (freeFunction == NULL))
    {
        LogError("CloneFunction or FreeFunction is Null.");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_MULTITREE_99_005:[ MultiTree_Create creates a new tree.]*/
        /*Codes_SRS_MULTITREE_99_006:[MultiTree_Create returns a non - NULL pointer if the tree has been successfully created.]*/
        /*Codes_SRS_MULTITREE_99_007:[MultiTree_Create returns NULL if the tree has not been successfully created.]*/
        result = (MULTITREE_HANDLE_DATA*)calloc(1, sizeof(MULTITREE_HANDLE_DATA));
        if (result != NULL)
        {
            result->name = NULL;
            result->value = NULL;
            result->cloneFunction = cloneFunction;
            result->freeFunction = freeFunction;
            result->nChildren = 0;
            result->children = NULL;
        }
        else
        {
            LogError("MultiTree_Create failed because malloc failed");
        }
    }

    return (MULTITREE_HANDLE)result;
}


/*return NULL if a child with the name "name" doesn't exists*/
/*returns a pointer to the existing child (if any)*/
static MULTITREE_HANDLE_DATA* getChildByName(MULTITREE_HANDLE_DATA* node, const char* name)
{
    MULTITREE_HANDLE_DATA* result = NULL;
    size_t i;
    for (i = 0; i < node->nChildren; i++)
    {
        if (strcmp(node->children[i]->name, name) == 0)
        {
            result = node->children[i];
            break;
        }
    }
    return result;
}

/*helper function to create a child immediately under this node*/
/*return 0 if it created it, any other number is error*/

typedef enum CREATELEAF_RESULT_TAG
{
    CREATELEAF_OK,
    CREATELEAF_ALREADY_EXISTS,
    CREATELEAF_EMPTY_NAME,
    CREATELEAF_ERROR,
    CREATELEAF_RESULT_COUNT // Used to track the number of elements in the enum
                            // Do not remove, or add new enum values below this one
}CREATELEAF_RESULT;

static STATIC_VAR_UNUSED const char* CreateLeaf_ResultAsString[CREATELEAF_RESULT_COUNT] =
{
    MU_TOSTRING(CREATELEAF_OK),
    MU_TOSTRING(CREATELEAF_ALREADY_EXISTS),
    MU_TOSTRING(CREATELEAF_EMPTY_NAME),
    MU_TOSTRING(CREATELEAF_ERROR)
};

/*name cannot be empty, value can be empty or NULL*/
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
#endif
#ifdef _MSC_VER
#pragma warning(disable: 4701) /* potentially uninitialized local variable 'result' used */ /* the scanner cannot track linked "newNode" and "result" therefore the warning*/
#endif
static CREATELEAF_RESULT createLeaf(MULTITREE_HANDLE_DATA* node, const char*name, const char*value, MULTITREE_HANDLE_DATA** childNode)
{
    CREATELEAF_RESULT result;
    /*can only create it if it doesn't exist*/
    if (strlen(name) == 0)
    {
        /*Codes_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
        result = CREATELEAF_EMPTY_NAME;
        LogError("(result = %s)", CreateLeaf_ResultAsString[result]);
    }
    else if (getChildByName(node, name) != NULL)
    {
        result = CREATELEAF_ALREADY_EXISTS;
        LogError("(result = %s)", CreateLeaf_ResultAsString[result]);
    }
    else
    {
        MULTITREE_HANDLE_DATA* newNode = (MULTITREE_HANDLE_DATA*)calloc(1, sizeof(MULTITREE_HANDLE_DATA));
        if (newNode == NULL)
        {
            result = CREATELEAF_ERROR;
            LogError("(result = %s)", CreateLeaf_ResultAsString[result]);
        }
        else
        {
            newNode->nChildren = 0;
            newNode->children = NULL;
            if (mallocAndStrcpy_s(&(newNode->name), name) != 0)
            {
                /*not nice*/
                free(newNode);
                newNode = NULL;
                result = CREATELEAF_ERROR;
                LogError("(result = %s)", CreateLeaf_ResultAsString[result]);
            }
            else
            {
                newNode->cloneFunction = node->cloneFunction;
                newNode->freeFunction = node->freeFunction;

                if (value == NULL)
                {
                    newNode->value = NULL;
                }
                else if (node->cloneFunction(&(newNode->value), value) != 0)
                {
                    free(newNode->name);
                    newNode->name = NULL;
                    free(newNode);
                    newNode = NULL;
                    result = CREATELEAF_ERROR;
                    LogError("(result = %s)", CreateLeaf_ResultAsString[result]);
                }
                else
                {
                    /*all is fine until now*/
                }
            }


            if (newNode!=NULL)
            {
                /*allocate space in the father node*/
                MULTITREE_HANDLE_DATA** newChildren = (MULTITREE_HANDLE_DATA**)realloc(node->children, (node->nChildren + 1)*sizeof(MULTITREE_HANDLE_DATA*));
                if (newChildren == NULL)
                {
                    /*no space for the new node*/
                    newNode->value = NULL;
                    free(newNode->name);
                    newNode->name = NULL;
                    free(newNode);
                    newNode = NULL;
                    result = CREATELEAF_ERROR;
                    LogError("(result = %s)", CreateLeaf_ResultAsString[result]);
                }
                else
                {
                    node->children = newChildren;
                    node->children[node->nChildren] = newNode;
                    node->nChildren++;
                    if (childNode != NULL)
                    {
                        *childNode = newNode;
                    }
                    result = CREATELEAF_OK;
                }
            }
        }
    }

    return result;
#ifdef _MSC_VER
#pragma warning(default: 4701) /* potentially uninitialized local variable 'result' used */ /* the scanner cannot track linked "newNode" and "result" therefore the warning*/
#endif
#ifdef __APPLE__
#pragma clang diagnostic pop
#endif
}

MULTITREE_RESULT MultiTree_AddLeaf(MULTITREE_HANDLE treeHandle, const char* destinationPath, const void* value)
{
    /*codes_SRS_MULTITREE_99_018:[ If the treeHandle parameter is NULL, MULTITREE_INVALID_ARG shall be returned.]*/
    MULTITREE_RESULT result;
    if (treeHandle == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_019:[ If parameter destinationPath is NULL, MULTITREE_INVALID_ARG shall be returned.]*/
    else if (destinationPath == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_020:[ If parameter value is NULL, MULTITREE_INVALID_ARG shall be returned.]*/
    else if (value == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_050:[ If destinationPath a string with zero characters, MULTITREE_INVALID_ARG shall be returned.]*/
    else if (strlen(destinationPath) == 0)
    {
        result = MULTITREE_EMPTY_CHILD_NAME;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        /*break the path into components*/
        /*find the first child name*/
        MULTITREE_HANDLE_DATA * node = (MULTITREE_HANDLE_DATA *)treeHandle;
        char * whereIsDelimiter;
        /*if first character is / then skip it*/
        /*Codes_SRS_MULTITREE_99_014:[DestinationPath is a string in the following format: /child1/child12 or child1/child12] */
        if (destinationPath[0] == '/')
        {
            destinationPath++;
        }
        /*if there's just a string, it needs to be created here*/
        whereIsDelimiter = (char*)strchr(destinationPath, '/');
        if (whereIsDelimiter == NULL)
        {
            /*Codes_SRS_MULTITREE_99_017:[ Subsequent names designate hierarchical children in the tree. The last child designates the child that will receive the value.]*/
            CREATELEAF_RESULT res = createLeaf(node, destinationPath, (const char*)value, NULL);
            switch (res)
            {
                default:
                {
                    /*Codes_SRS_MULTITREE_99_025:[The function shall return MULTITREE_ERROR to indicate any other error not specified here.]*/
                    result = MULTITREE_ERROR;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                    break;
                }
                case CREATELEAF_ALREADY_EXISTS:
                {
                    /*Codes_SRS_MULTITREE_99_021:[ If the node already has a value assigned to it, MULTITREE_ALREADY_HAS_A_VALUE shall be returned and the existing value shall not be changed.]*/
                    result = MULTITREE_ALREADY_HAS_A_VALUE;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                    break;
                }
                case CREATELEAF_OK:
                {
                    /*Codes_SRS_MULTITREE_99_034:[ The function returns MULTITREE_OK when data has been stored in the tree.]*/
                    result = MULTITREE_OK;
                    break;
                }
                case CREATELEAF_EMPTY_NAME:
                {
                    /*Codes_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
                    result = MULTITREE_EMPTY_CHILD_NAME;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                    break;
                }
            }
        }
        else
        {
            /*if there's more or 1 delimiter in the path... */
            /*Codes_SRS_MULTITREE_99_017:[ Subsequent names designate hierarchical children in the tree. The last child designates the child that will receive the value.]*/
            char firstInnerNodeName[INNER_NODE_NAME_SIZE];
            if ((whereIsDelimiter - destinationPath) >= INNER_NODE_NAME_SIZE)
            {
                /*Codes_SRS_MULTITREE_99_025:[ The function shall return MULTITREE_ERROR to indicate any other error not specified here.]*/
                result = MULTITREE_ERROR;
                LogError("Destination path is too large %lu", (unsigned long)(whereIsDelimiter - destinationPath));
            }
            else if (memcpy(firstInnerNodeName, destinationPath, whereIsDelimiter - destinationPath) == NULL)
            {
                /*Codes_SRS_MULTITREE_99_025:[ The function shall return MULTITREE_ERROR to indicate any other error not specified here.]*/
                result = MULTITREE_ERROR;
                LogError("(result = MULTITREE_ERROR)");
            }
            else
            {
                firstInnerNodeName[whereIsDelimiter - destinationPath] = 0;
                MULTITREE_HANDLE_DATA *child = getChildByName(node, firstInnerNodeName);
                if (child == NULL)
                {
                    /*Codes_SRS_MULTITREE_99_022:[ If a child along the path does not exist, it shall be created.] */
                    /*Codes_SRS_MULTITREE_99_023:[ The newly created children along the path shall have a NULL value by default.]*/
                    CREATELEAF_RESULT res = createLeaf(node, firstInnerNodeName, NULL, NULL);
                    switch (res)
                    {
                        default:
                        {
                            result = MULTITREE_ERROR;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                            break;
                        }
                        case(CREATELEAF_EMPTY_NAME):
                        {
                            /*Codes_SRS_MULTITREE_99_024:[ if a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned.]*/
                            result = MULTITREE_EMPTY_CHILD_NAME;
                            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                            break;
                        }
                        case(CREATELEAF_OK):
                        {
                            MULTITREE_HANDLE_DATA *createdChild = getChildByName(node, firstInnerNodeName);
                            result = MultiTree_AddLeaf(createdChild, whereIsDelimiter, value);
                            break;
                        }
                    };
                }
                else
                {
                    result = MultiTree_AddLeaf(child, whereIsDelimiter, value);
                }
            }
        }
    }
    return result;
}

/* Codes_SRS_MULTITREE_99_053:[ MultiTree_AddChild shall add a new node with the name childName to the multi tree node identified by treeHandle] */
MULTITREE_RESULT MultiTree_AddChild(MULTITREE_HANDLE treeHandle, const char* childName, MULTITREE_HANDLE* childHandle)
{
    MULTITREE_RESULT result;
    /* Codes_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_AddChild shall return MULTITREE_INVALID_ARG.] */
    if ((treeHandle == NULL) ||
        (childName == NULL) ||
        (childHandle == NULL))
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        MULTITREE_HANDLE_DATA* childNode;

        /* Codes_SRS_MULTITREE_99_060:[ The value associated with the new node shall be NULL.] */
        CREATELEAF_RESULT res = createLeaf((MULTITREE_HANDLE_DATA*)treeHandle, childName, NULL, &childNode);
        switch (res)
        {
            default:
            {
                result = MULTITREE_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                break;
            }
            case CREATELEAF_ALREADY_EXISTS:
            {
                /* Codes_SRS_MULTITREE_99_061:[ If a child node with the same name already exists, MultiTree_AddChild shall return MULTITREE_ALREADY_HAS_A_VALUE.] */
                result = MULTITREE_ALREADY_HAS_A_VALUE;
                LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                break;
            }
            case CREATELEAF_OK:
            {
                /* Codes_SRS_MULTITREE_99_062:[ The new node handle shall be returned in the childHandle argument.] */
                *childHandle = childNode;

                /* Codes_SRS_MULTITREE_99_054:[ On success, MultiTree_AddChild shall return MULTITREE_OK.] */
                result = MULTITREE_OK;
                break;
            }
            case CREATELEAF_EMPTY_NAME:
            {
                /* Tests_SRS_MULTITREE_99_066:[ If the childName argument is an empty string, MultiTree_AddChild shall return MULTITREE_EMPTY_CHILD_NAME.] */
                result = MULTITREE_EMPTY_CHILD_NAME;
                LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                break;
            }
        }
    }

    return result;
}

MULTITREE_RESULT MultiTree_GetChildCount(MULTITREE_HANDLE treeHandle, size_t* count)
{
    MULTITREE_RESULT result;
    /*Codes_SRS_MULTITREE_99_027:[If treeHandle is NULL, the function returns MULTITREE_INVALID_ARG.]*/
    if (treeHandle == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_028:[ If parameter count is NULL, the function returns MULTITREE_INVALID_ARG.]*/
    else if (count == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        /*Codes_SRS_MULTITREE_99_029:[ This function writes in *count the number of direct children for a tree node specified by the parameter treeHandle]*/
        *count = ((MULTITREE_HANDLE_DATA*)treeHandle)->nChildren;
        /*Codes_SRS_MULTITREE_99_035:[ The function shall return MULTITREE_OK when *count contains the number of children of the node pointed to be parameter treeHandle.]*/
        result = MULTITREE_OK;
    }
    return result;
}

MULTITREE_RESULT MultiTree_GetChild(MULTITREE_HANDLE treeHandle, size_t index, MULTITREE_HANDLE *childHandle)
{
    MULTITREE_RESULT result;
    /*Codes_SRS_MULTITREE_99_031:[ If parameter treeHandle is NULL, the function returns MULTITREE_INVALID_ARG.]*/
    if (treeHandle == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_033:[ If parameter childHandle is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
    else if (childHandle == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        MULTITREE_HANDLE_DATA * node = (MULTITREE_HANDLE_DATA *)treeHandle;
        /*Codes_SRS_MULTITREE_99_032:[If parameter index is out of range, the function shall return MULTITREE_OUT_OF_RANGE_INDEX]*/
        if (node->nChildren <= index)
        {
            result = MULTITREE_INVALID_ARG;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        else
        {
            /*Codes_SRS_MULTITREE_99_030:[ This function writes in *childHandle parameter the "index"th child of the node pointed to by parameter treeHandle]*/
            /*Codes_SRS_MULTITREE_99_035:[ The function returns MULTITREE_OK when *childHandle contains a handle to the "index"th child of the tree designated by parameter treeHandle.]*/
            *childHandle = node->children[index];
            result = MULTITREE_OK;
        }
    }
    return result;
}

MULTITREE_RESULT MultiTree_GetName(MULTITREE_HANDLE treeHandle, STRING_HANDLE destination)
{
    MULTITREE_RESULT result;
    /*Codes_SRS_MULTITREE_99_037:[ If treeHandle is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
    if (treeHandle == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_038:[If destination is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
    else if (destination == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        MULTITREE_HANDLE_DATA *node = (MULTITREE_HANDLE_DATA*)treeHandle;
        /*Codes_SRS_MULTITREE_99_051:[ The function returns MULTITREE_EMPTY_CHILD_NAME when used with the root of the tree.]*/
        if (node->name == NULL)
        {
            result = MULTITREE_EMPTY_CHILD_NAME;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        /*Codes_SRS_MULTITREE_99_036:[ This function fills the buffer pointed to by parameter destination with the name of the root node of the tree designated by parameter treeHandle.]*/
        else if (STRING_concat(destination, node->name)!=0)
        {
            /*Codes_SRS_MULTITREE_99_040:[ The function returns MULTITREE_ERROR to indicate any other error.]*/
            result = MULTITREE_ERROR;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        else
        {
            /*Codes_SRS_MULTITREE_99_039:[ The function returns MULTITREE_OK when destination contains the name of the root node of the tree designated by treeHandle parameter.]*/
            result = MULTITREE_OK;
        }
    }

    return result;
}

/* Codes_SRS_MULTITREE_99_063:[ MultiTree_GetChildByName shall retrieve the handle of the child node childName from the treeNode node.] */
MULTITREE_RESULT MultiTree_GetChildByName(MULTITREE_HANDLE treeHandle, const char* childName, MULTITREE_HANDLE *childHandle)
{
    MULTITREE_RESULT result;

    /* Codes_SRS_MULTITREE_99_065:[ If any argument is NULL, MultiTree_GetChildByName shall return MULTITREE_INVALID_ARG.] */
    if ((treeHandle == NULL) ||
        (childHandle == NULL) ||
        (childName == NULL))
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        MULTITREE_HANDLE_DATA * node = (MULTITREE_HANDLE_DATA *)treeHandle;
        size_t i;

        for (i = 0; i < node->nChildren; i++)
        {
            if (strcmp(node->children[i]->name, childName) == 0)
            {
                break;
            }
        }

        if (i == node->nChildren)
        {
            /* Codes_SRS_MULTITREE_99_068:[ If the specified child is not found, MultiTree_GetChildByName shall return MULTITREE_CHILD_NOT_FOUND.] */
            result = MULTITREE_CHILD_NOT_FOUND;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        else
        {
            /* Codes_SRS_MULTITREE_99_067:[ The child node handle shall be returned in the childHandle argument.] */
            *childHandle = node->children[i];

            /* Codes_SRS_MULTITREE_99_064:[ On success, MultiTree_GetChildByName shall return MULTITREE_OK.] */
            result = MULTITREE_OK;
        }
    }
    return result;
}

MULTITREE_RESULT MultiTree_GetValue(MULTITREE_HANDLE treeHandle, const void** destination)
{
    MULTITREE_RESULT result;
    /*Codes_SRS_MULTITREE_99_042:[If treeHandle is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
    if (treeHandle == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /*Codes_SRS_MULTITREE_99_043:[ If destination is NULL, the function shall return MULTITREE_INVALID_ARG.]*/
    else if (destination == NULL)
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        MULTITREE_HANDLE_DATA * node = (MULTITREE_HANDLE_DATA*)treeHandle;
        /*Codes_SRS_MULTITREE_99_044:[ If there is no value in the node then MULTITREE_EMPTY_VALUE shall be returned.]*/
        if (node->value == NULL)
        {
            result = MULTITREE_EMPTY_VALUE;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        else
        {
            /*Codes_SRS_MULTITREE_99_041:[This function updates the *destination parameter to the internally stored value.]*/
            *destination = node->value;
            result = MULTITREE_OK;
        }
    }
    return result;
}

MULTITREE_RESULT MultiTree_SetValue(MULTITREE_HANDLE treeHandle, void* value)
{
    MULTITREE_RESULT result;

    /* Codes_SRS_MULTITREE_99_074:[ If any argument is NULL, MultiTree_SetValue shall return MULTITREE_INVALID_ARG.] */
    if ((treeHandle == NULL) ||
        (value == NULL))
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        MULTITREE_HANDLE_DATA * node = (MULTITREE_HANDLE_DATA*)treeHandle;
        if (node->value != NULL)
        {
            /* Codes_SRS_MULTITREE_99_076:[ If the node already has a value then MultiTree_SetValue shall return MULTITREE_ALREADY_HAS_A_VALUE.] */
            result = MULTITREE_ALREADY_HAS_A_VALUE;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        else
        {
            /* Codes_SRS_MULTITREE_99_072:[ MultiTree_SetValue shall set the value of the node indicated by the treeHandle argument to the value of the argument value.] */
            if (node->cloneFunction(&node->value, value) != 0)
            {
                /* Codes_SRS_MULTITREE_99_075:[ MultiTree_SetValue shall return MULTITREE_ERROR to indicate any other error.] */
                result = MULTITREE_ERROR;
                LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
            }
            else
            {
                /* Codes_SRS_MULTITREE_99_073:[ On success, MultiTree_SetValue shall return MULTITREE_OK.] */
                result = MULTITREE_OK;
            }
        }
    }
    return result;
}

void MultiTree_Destroy(MULTITREE_HANDLE treeHandle)
{
    if (treeHandle != NULL)
    {
        MULTITREE_HANDLE_DATA* node = (MULTITREE_HANDLE_DATA*)treeHandle;
        size_t i;
        for (i = 0; i < node->nChildren;i++)
        {
            /*Codes_SRS_MULTITREE_99_047:[ This function frees any system resource used by the tree designated by parameter treeHandle]*/
            MultiTree_Destroy(node->children[i]);
        }
        /*Codes_SRS_MULTITREE_99_047:[ This function frees any system resource used by the tree designated by parameter treeHandle]*/
        if (node->children != NULL)
        {
            free(node->children);
            node->children = NULL;
        }

        /*Codes_SRS_MULTITREE_99_047:[ This function frees any system resource used by the tree designated by parameter treeHandle]*/
        if (node->name != NULL)
        {
            free(node->name);
            node->name = NULL;
        }

        /*Codes_SRS_MULTITREE_99_047:[ This function frees any system resource used by the tree designated by parameter treeHandle]*/
        if (node->value != NULL)
        {
            node->freeFunction(node->value);
            node->value = NULL;
        }

        /*Codes_SRS_MULTITREE_99_047:[ This function frees any system resource used by the tree designated by parameter treeHandle]*/
        free(node);
    }
}

MULTITREE_RESULT MultiTree_GetLeafValue(MULTITREE_HANDLE treeHandle, const char* leafPath, const void** destination)
{
    MULTITREE_RESULT result;

    /* Codes_SRS_MULTITREE_99_055:[ If any argument is NULL, MultiTree_GetLeafValue shall return MULTITREE_INVALID_ARG.] */
    if ((treeHandle == NULL) ||
        (leafPath == NULL) ||
        (destination == NULL))
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    /* Codes_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value. If a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned.] */
    else if (strlen(leafPath) == 0)
    {
        result = MULTITREE_EMPTY_CHILD_NAME;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        /*break the path into components*/
        /*find the first child name*/
        MULTITREE_HANDLE_DATA* node = (MULTITREE_HANDLE_DATA *)treeHandle;
        const char* pos = leafPath;
        const char * whereIsDelimiter;

        /*if first character is / then skip it*/
        if (*pos == '/')
        {
            pos++;
        }

        if (*pos == '\0')
        {
            /* Codes_SRS_MULTITREE_99_069:[ If a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned.] */
            result = MULTITREE_EMPTY_CHILD_NAME;
            LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
        }
        else
        {
            result = MULTITREE_OK;

            /* Codes_SRS_MULTITREE_99_056:[ The leafPath argument is a string in the following format: /child1/child12 or child1/child12.] */
            /* Codes_SRS_MULTITREE_99_058:[ The last child designates the child that will receive the value.] */
            while (*pos != '\0')
            {
                size_t i;
                size_t childCount = node->nChildren;

                whereIsDelimiter = pos;

                while ((*whereIsDelimiter != '/') && (*whereIsDelimiter != '\0'))
                {
                    whereIsDelimiter++;
                }

                if (whereIsDelimiter == pos)
                {
                    /* Codes_SRS_MULTITREE_99_069:[ If a child name is empty (such as in  "/child1//child12"), MULTITREE_EMPTY_CHILD_NAME shall be returned.] */
                    result = MULTITREE_EMPTY_CHILD_NAME;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                    break;
                }
                else if (childCount == 0)
                {
                    /* Codes_SRS_MULTITREE_99_071:[ When the child node is not found, MultiTree_GetLeafValue shall return MULTITREE_CHILD_NOT_FOUND.] */
                    result = MULTITREE_CHILD_NOT_FOUND;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                    break;
                }
                else
                {
                    for (i = 0; i < childCount; i++)
                    {
                        if (strncmp(node->children[i]->name, pos, whereIsDelimiter - pos) == 0)
                        {
                            /* Codes_SRS_MULTITREE_99_057:[ Subsequent names designate hierarchical children in the tree.] */
                            node = node->children[i];
                            break;
                        }
                    }

                    if (i == childCount)
                    {
                        /* Codes_SRS_MULTITREE_99_071:[ When the child node is not found, MultiTree_GetLeafValue shall return MULTITREE_CHILD_NOT_FOUND.] */
                        result = MULTITREE_CHILD_NOT_FOUND;
                        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                        break;
                    }
                    else
                    {
                        if (*whereIsDelimiter == '/')
                        {
                            pos = whereIsDelimiter + 1;
                        }
                        else
                        {
                            /* end of path */
                            pos = whereIsDelimiter;
                            break;
                        }
                    }
                }
            }

            if (*pos == 0)
            {
                if (node->value == NULL)
                {
                    /* Codes_SRS_MULTITREE_99_070:[ If an attempt is made to get the value for a node that does not have a value set, then MultiTree_GetLeafValue shall return MULTITREE_EMPTY_VALUE.] */
                    result = MULTITREE_EMPTY_VALUE;
                    LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
                }
                /*Codes_SRS_MULTITREE_99_053:[ MultiTree_GetLeafValue shall copy into the *destination argument the value of the node identified by the leafPath argument.]*/
                else
                {
                    *destination = node->value;
                    /* Codes_SRS_MULTITREE_99_054:[ On success, MultiTree_GetLeafValue shall return MULTITREE_OK.] */
                    result = MULTITREE_OK;
                }
            }
        }
    }
    return result;
}

/* Codes_SRS_MULTITREE_99_077:[ MultiTree_DeleteChild shall remove the direct children node (no recursive search) set by childName.] */
MULTITREE_RESULT MultiTree_DeleteChild(MULTITREE_HANDLE treeHandle, const char* childName)
{
    MULTITREE_RESULT result;
    /* Codes_SRS_MULTITREE_99_077:[ If any argument is NULL, MultiTree_DeleteChild shall return MULTITREE_INVALID_ARG.] */
    if ((treeHandle == NULL) ||
        (childName == NULL))
    {
        result = MULTITREE_INVALID_ARG;
        LogError("(result = %s)", MU_ENUM_TO_STRING(MULTITREE_RESULT, result));
    }
    else
    {
        size_t i;
        size_t childToRemove = treeHandle->nChildren;
        MULTITREE_HANDLE treeToRemove = NULL;

        for (i = 0; i < treeHandle->nChildren; i++)
        {
            if (0 == strcmp(treeHandle->children[i]->name, childName))
            {
                childToRemove = i;
                treeToRemove = treeHandle->children[childToRemove];
                break;
            }
        }

        if (i == treeHandle->nChildren)
        {
            /* Codes_SRS_MULTITREE_99_079:[If childName is not found, MultiTree_DeleteChild shall return MULTITREE_CHILD_NOT_FOUND.] */
            result = MULTITREE_CHILD_NOT_FOUND;
            // Don't log error; this function is best effort only.  Caller will determine actual error state.
        }
        else
        {
            for (i = childToRemove; i < treeHandle->nChildren - 1; i++)
            {
                treeHandle->children[i] = treeHandle->children[i+1];
            }

            /* Codes_SRS_MULTITREE_99_077:[ MultiTree_DeleteChild shall remove the direct children node (no recursive search) set by childName  */
            MultiTree_Destroy(treeToRemove);

            // Even though this isn't reachable anymore after decrementing count, NULL out for cleanliness
            treeHandle->children[treeHandle->nChildren - 1] = NULL;
            treeHandle->nChildren = treeHandle->nChildren - 1;

            result = MULTITREE_OK;
        }
    }

    return result;
}

