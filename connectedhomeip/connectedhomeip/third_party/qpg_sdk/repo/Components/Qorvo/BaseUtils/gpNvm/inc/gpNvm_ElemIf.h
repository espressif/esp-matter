/*
 * Copyright (c) 2012-2013, 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * This file contains the internal defines, typedefs,... of the NVM component element interface implementation
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPNVM_ELEMIF_H_
#define _GPNVM_ELEMIF_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpNvm.h"

#ifndef GP_NVM_DIVERSITY_ELEMENT_IF
#error Element interface header only to be used for NVM element interface
#endif //GP_NVM_DIVERSITY_ELEMENT_IF

#ifdef GP_NVM_DIVERSITY_ELEMENT_IF

/**
 * @file gpNvm_ElemIf.h
 *
 * @defgroup ELMIF_NVM Element interface NVM functions
 * @brief This file contains element specific interface to the gpNvm component
 *
 *  This header is implicitly included via the main interface header if the element interface
 *  diversity is selected.
 *  The element interface version of the gpNvm component treats various NVM attributes
 *  in a different way than the standard gpNvm version. In the standard version the components
 *  register arrays of tags for the NVM attributes which form sections belonging to its component.
 *  Tag IDs are derived implicitly from the index of a tag within the section array and if application
 *  was to configure a component in such way that attributes are added or dropped, the implicit
 *  tag IDs could change. In addition to that, the attributes are placed in the NVM one after another
 *  without embedding information to help disambiguate them when the firmware changes.
 *
 *  The element interface offers solution to this problem having in mind also the possibility to reuse
 *  inherited standard NVM by the element interface compatible application. The following image illustrates
 *  the reference situation (NVM usage of last non-up/downgradeable firmware) and relationship of it to the
 *  unique IDs of the attributes:
 *
 *  @image latex NVM_Reference_NVM_and_mapping_to_UIDs.png "Reference NVM and mapping to UIDs" width=12cm
 *
 *  To resolve the issue of lacking the information about the attributes in the inherited standard NVM,
 *  the application is required to provide this information in a form of a program memory look-up table
 *  as illustrated in the following image:
 *
 *  @image latex NVM_ElemIf_based_application_and_inherited_NVM.png "Element interface based application and inherited NVM" width=12cm
 *
 *  Element interface based application is allowed to add/remove attributes and change attribute sizes
 *  (with limitation that the common denominator always stays alligned to the beginning of the attribute).
 *  The information about the added content into the NVM is embedded in the NVM as linked list of look-up tables.
 *  Initial program memory based look-up table is used as starting point for parsing of this linked list.
 *  Each time some component registeres attribute not known already in the NVM, NVM look-up table gets extended
 *  with this information and new attribute gets its space reserved in the NVM.
 *  Look-up tables are NVM elements themselves and as such are limited in size so the link list chaining
 *  is provided as mechanism for further extension.
 *
 *  The following images illustrate several typical situations of firmware upgrades and downgrades causing
 *  redefinition of the attributes:
 *
 *  @image latex NVM_Upgrading_to_increased_attributes.png "Upgrading to attribute increased in size" width=12cm
 *
 *  @image latex NVM_Downgrading_to_smaller_attributes.png "Downgrading to attribute smaller in size" width=12cm
 *
 *  @image latex NVM_Upgrading_to_decreased_attriutes.png "Upgrading to attribute decreased in size" width=12cm
 *
 *  @image latex NVM_Downgrading_to_larger_attributes.png "Downgrading to attribute larger in size" width=12cm
 *
 *  @image latex NVM_Upgrading_to_added_attributes.png "Upgrading by adding attributes" width=12cm
 *
 *  @image latex NVM_Downgrading_to_less_attributes.png "Downgrading to smaller number of attributes" width=12cm
 *
 *  @image latex NVM_Upgrading_to_less_attributes.png "Upgrading to smaller number of attributes" width=12cm
 *
 *  Elements themselves are defined in the NVM in two ways. The inherited NVM has implicit definition of the
 *  elements as inherited attributes and their sizes. The subsequent elements are added in the NVM in a platform
 *  specific way. Simple way to implement platform specific element access is to have a table of fixed size
 *  located right after the inherited NVM. This table can contain array of values (8 or 16 bit) specifying element
 *  sizes. It should be initialized to all 1 values (all 0 and all 1 values indicating void element). Elements
 *  would be added one after another and the size array would be updated accordingly. When locating the element
 *  within the NVM, first query would be made to program memory look-up table. If element is not found there,
 *  the NVM element size array located right after the last known inherited attribute could be used to locate
 *  noninherited elements.
 *
 *  The explained manipulations with NVM based look-up tables are illustrated in the following images:
 *
 *  @image latex NVM_Extending_NVM_LUT.png "Extending the existing NVM LUT with additional element" width=12cm
 *
 *  @image latex NVM_Adding_NVM_LUT.png "Adding NVM LUT with refering to newly added element" width=12cm
 *
 *  @image latex NVM_Calculation_of_the_address_of_the_element.png "Calculation of the address of the element in the NVM" width=12cm
 *
 *  All of the premises for the driver stated so far imply that the way the attributes are registered to the
 *  NVM driver had to change. Information about attribute identity has to be provided explicitly rather than
 *  implicitly so new registration function is introduced. Elements may be registered individualy or in group(s) where
 *  order (IDs within componen or componets them selves) is not relevant. Any mix is possible, match will be found.
 *  Since the element tags had to change in structure to accomodate this explicit information, opportunity is taken
 *  to also change the way the default value initialization is done. Instead of providing the pointer to default data (if any)
 *  a default initialization function reference is provided making large sparse default information compressed beter in the
 *  program memory.
 *  Another addition to the tag is element (and not section) specific consistency checker function reference that can be
 *  used to asses integrity per attribute.
 *
 *  The special attention is given to make the best effort approach in reusing the attribute values when traversing among
 *  the firmware versions. The standard applications have the first attribute contain the CRC signing the section-attribute
 *  structure of the NVM. The element interface based applications will reconstruct this information from the program memory
 *  look-up table and will be able to asses if the inherited NVM is valid. This information is not dependant on the actual
 *  attributes used by the element interface based application.
 *  All of the reused attributes are updated in the inherited memory and if some attributes are extended, only the delta of the
 *  extended attribute will be allocated in the separate NVM element.. Initial value of
 *  extended attribute will be composite of already present value in inherited attribute and the rest will be default initialized.
 *  In unlikely event of reducing the inherited attribute size, the part of the inherited arribute known and used by the new
 *  application will be kept up to date.
 *  IMPORTANT NOTE: although in most of cases this approach provides valid attribute data when up or downgrading, care must be taken
 *  that the attribute values created by default extension of previous shorter attributes or by truncating previous longer attributes
 *  actually make sense. It is recommended to use consistency checker functions if the consistency can be broken by up/downgrading.
 *
 *  Compatibility between the applications in regard to NVM is defined by two requirements:
 *  - All mutually compatible versions have to use same base program memory LUT content
 *  - All mutually compatible versions have to use same unique IDs for same attributes
 *
 *  One special situation occurs with applications offering remote NVM service over the serial interface. The RW_NVM storage is allocated
 *  as array of elements that total in size required remote NVM. These elements are attributes being registered with common tag ID
 *  but internally their ID is incremented on every registration of such special element to stop them from being treated as one
 *  single attribute.
 */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#if !defined(GP_NVM_DIVERSITY_ELEMIF_KEYMAP)
/** @brief GP_NVM_MAX_NUM_BASE_LUT_ELEMENTS is a macro defining the limit for the number of attributes inherited from the standard application */
#define GP_NVM_MAX_NUM_BASE_LUT_ELEMENTS                                255
/** @brief GP_NVM_MAX_NUM_LUT_ELEMENTS is a macro defining the limit for the number of attributes defined in NVM based LookUp Table */
#define GP_NVM_MAX_NUM_LUT_ELEMENTS                                     15
/** @brief GP_NVM_NO_LUT_INDEX is a macro defining the value used in the list link field of the last LUT in the list */
#define GP_NVM_NO_LUT_INDEX                                             0xFFFF
#endif /* GP_NVM_DIVERSITY_ELEMIF_KEYMAP */

/** @brief NVM_MAX_TAG_SIZE is a macro defining the maximal size in bytes of object located in the NVM element */
#define NVM_MAX_TAG_SIZE                                                239
/** @brief GP_NVM_VERSION_CRC_UNIQUE_TAG_ID is a macro defining the tag ID of the inherited NVM CRC attribute. Must allways be 0 */
#define GP_NVM_VERSION_CRC_UNIQUE_TAG_ID                                0
/** @macro gpNvm_VersionKey*/
/** @brief Wildcard used to access Nvm type. */
#define GP_NVM_TYPE_UNIQUE_TAG_ID                                       0xEF
/** @brief GP_NVM_REMOTE_RW_TAG_ID_BASE is a macro defining the tag ID base used for elements storing remote NVM. This is the value that should be used for all of the elements registered for remote NVM */
#define GP_NVM_REMOTE_RW_TAG_ID_BASE                                    128

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
/** @typedef gpNvm_ElementDefine_t
 *  @brief The gpNvm_ElementDefine_t type provides unique identification of nvm element containing nvm attribute */
typedef struct
{
/** @brief Unique tag identifier composed from the unique component ID located in MSB and unique tag ID within the component located in LSB */
    UInt16  uniqueTagId;
/** @brief The number of bytes of the attribute in the element */
    UInt16  elementSize;
} gpNvm_ElementDefine_t;

/** @struct gpNvm_LookUpTableHeader
 *  @brief The gpNvm_LookUpTableHeader structure specifies the fields that describe header of the element LookUp Table */
/** @typedef gpNvm_LookUpTableHeader_t
 *  @brief The gpNvm_LookUpTableHeader_t type describes header of the element LookUp Table */
typedef struct gpNvm_LookUpTableHeader {
/** @brief The number of tags for attribute elements contained in the LookUp Table */
    UInt16 nbrTags;
/** @brief The index of the element containing next LookUp Table. LUTs are forming single direction linked list. */
    UInt16 indexNextLUT;
/** @brief The CRC calculated over the rest of the header and element defines belonging to the LookUp Table */
    gpNvm_VersionCrc_t crcLUT;
} gpNvm_LookUpTableHeader_t;


/** @struct gpNvm_IdentifiableTag
 *  @brief The gpNvm_IdentifiableTag structure specifies the fields that describe an uniquely identifiable nvm-tag */
/** @typedef gpNvm_IdentifiableTag_t
 *  @brief The gpNvm_IdentifiableTag_t type describes an uniquely identifiable nvm-tag */
typedef struct gpNvm_IdentifiableTag {
/** @brief Unique tag identifier composed from the unique component ID located in MSB and unique tag ID within the component located in LSB */
    UInt16                      uniqueTagId;
/** @brief A pointer to the corresponding RAM memory of the attribute to be backuped in nvm */
    UInt8*                      pRamLocation;
/** @brief The number of bytes of the nvm-tag */
    UInt16                      size;
/** @brief An indication of the update frequency */
    gpNvm_UpdateFrequency_t     updateFrequency;
/** @brief A pointer to function that initializes attribute default values. 0xFF will be used as default values if pointer is specified as NULL */
    Bool                        (*DefaultValueInitializerCB)(const ROM void * pTag, UInt8* pBuffer);
/** @brief A pointer to function that performs consistency check of the attribute value. */
    Bool                        (*ConsistencyCheckerCB)(const ROM void * pTag);
} gpNvm_IdentifiableTag_t;

/** @brief Get the Nvm Type describing the Nvm building blocks used.
*
*   @param pNvmType                  Pointer to UInt8 to store NvmType value.
*   @return result                   gpNvm_Result_DataAvailable gpNvm_Result_NoDataAvailable
*/
gpNvm_Result_t gpNvm_GetNvmType(UInt8* pNvmType);

/** @typedef gpNvm_cbDefaultValueInitializer_t
 *  @brief The gpNvm_cbDefaultValueInitializer_t type describes reference to a function that initializes attribute with default values
 *  @param pTag points to the description of the element to be initialized. If pTag indicates non NULL pRamLocation default values will be stored there.
 *  @param pBuffer points to buffer that will be used (if not NULL) to store default values.
 *  @return true if the initialization was successful and false otherwise
 */
typedef Bool (*gpNvm_cbDefaultValueInitializer_t)(const ROM void* pTag, UInt8* pBuffer);

/** @typedef gpNvm_cbConsistencyChecker_t
 *  @brief The gpNvm_cbConsistencyChecker_t type describes reference to a function that provides additional element specific check-consistency routine
 *  @param pTag points to the description of the element to be checked
 *  @return true if the check passed and false otherwise
 */
typedef Bool (*gpNvm_cbConsistencyChecker_t)(const ROM gpNvm_IdentifiableTag_t* pTag);

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/
#if !defined(GP_NVM_DIVERSITY_ELEMIF_KEYMAP)

/*****************************************************************************
 *                    Public Function Declarations
 *****************************************************************************/
/**@ingroup ELMIF_NVM
 * @brief Configures NVM for base tables other than gpNvm_elementBaseLUTHeader and gpNvm_elementBaseLUTElements
 *
 *  This function call may be used if there can be different possibilities for the NVM structure inherited. This function can't
 *  be used for detection of the type of the NVM used since the failure to validate NVM content against the registered table causes
 *  NVM defaulting.
 *  @param baseTableHeader            A reference to the header of the base LUT
 *  @param baseTableElements          A reference to the array of element descriptors of the base LUT
 */

void gpNvm_Config(const ROM gpNvm_LookUpTableHeader_t* baseTableHeader, const ROM gpNvm_ElementDefine_t* baseTableElements, const ROM gpNvm_VersionCrc_t* inheritedNvmVersionCrc);

#endif /* GP_NVM_DIVERSITY_ELEMIF_KEYMAP */

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpNvm_CodeJumpTableFlash_Defs_ElemIf.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

#ifdef GP_NVM_DIVERSITY_ELEMENT_IF
/**@ingroup ELMIF_NVM
 * @brief Registers an element to the NVM software block
 *
 *  Registers an element to the NVM software block. The function should be called from the _init
 *  function of the component for each element being registered.
 *
 *  @param pTag                 A pointer to a tag of the element allocated in flash contining the information about the attribute.
 */
void gpNvm_RegisterElement(const ROM gpNvm_IdentifiableTag_t* pTag);

/**@ingroup ELMIF_NVM
 * @brief Registers multiple elements to the NVM software block
 *
 *  Registers multiple of elements reported as array of tags to the NVM software block. The function should be called from the _init
 *  function of the component for each element being registered.
 *
 *  @param pTag                 A pointer to array of tags of the elements allocated in flash contining the information about the attributes.
 */
void gpNvm_RegisterElements(const ROM gpNvm_IdentifiableTag_t* pTag, UInt8 nbrOfTags);
#endif /* GP_NVM_DIVERSITY_ELEMENT_IF */

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#endif /* GP_NVM_DIVERSITY_ELEMENT_IF */

#endif //_GPNVM_ELEMIF_H_
