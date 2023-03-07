#!/bin/sh
# Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0


echo Test missing license
if jsonschema -i schema_test/INVALID_missing_license.json license.schema ; then
    echo " - FAILED - schema did not throw error" && exit 1
else
    echo " - PASSED - error thrown as expected"
fi

echo Test non permissive
if jsonschema -i schema_test/INVALID_non_permissive.json license.schema ; then
    echo " - FAILED - schema did not throw error" && exit 1
else
    echo " - PASSED - error thrown as expected"
fi

echo Test missing spdx license key
if jsonschema -i schema_test/INVALID_missing_spdx_license_key.json license.schema ; then
    echo " - FAILED - schema did not throw error" && exit 1
else
    echo " - PASSED - error thrown as expected"
fi

echo Test json file without license
if jsonschema -i schema_test/VALID_json_without_license.json license.schema ; then
    echo " - PASSED - no errors as expected"
else
    echo " - FAILED - schema throw error" && exit 1
fi

echo Test binary file without license
if jsonschema -i schema_test/VALID_binary_without_license.json license.schema ; then
    echo " - PASSED - no errors as expected"
else
    echo " - FAILED - schema throw error" && exit 1
fi

echo Test folder without license
if jsonschema -i schema_test/VALID_folder_without_license.json license.schema ; then
    echo " - PASSED - no errors as expected"
else
    echo " - FAILED - schema throw error" && exit 1
fi
