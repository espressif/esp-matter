// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STDBOOL_H
#define STDBOOL_H

#define __bool_true_false_are_defined	1

#define HAS_STDBOOL

#ifndef __cplusplus

typedef unsigned char bool;
typedef bool _Bool;

#define false	0
#define true	1

#endif /* __cplusplus */

#endif /* STDBOOL_H */
