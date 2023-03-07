/**
 * @file
 * @brief Header file containing various 'sizeof' macros
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef _SIZEOF_H_
#define _SIZEOF_H_

/**
* Macro returns the number of ELEMENTS an array can hold.
* Purpose of macro is increased readability in source code.
*/
#define sizeof_array(ARRAY) ((sizeof ARRAY) / (sizeof *ARRAY))

/**
* Macro returns the size of a struct member from type (no actual instantiated
* struct or pointer to struct needed). Use stadnard sizeof if you have an
* instantiation or pointer to the struct.
* Usage: as offsetoff, sizeof_structmember(MyStructType, MyStructMember)
*/
#define sizeof_structmember(TYPE, MEMBER) (sizeof(((TYPE *)0)->MEMBER))

#endif	// _SIZEOF_H_

