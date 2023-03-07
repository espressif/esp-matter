/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>
#include "manifest-moran3_decode.h"
#include "manifest-moran4_decode.h"
#include "cbor_debug.h" // Enables use of print functions when debugging tests.


uint8_t test_vector2[] = {
	0xa2, 0x01, 0xf6, 0x02, 0x58, 0x3a, 0xa3, 0x01, 0x01, 0x02,
	0x02, 0x05, 0x81, 0xa3, 0x01, 0x81, 0x41, 0x30,
	0x02, 0x18, 0x25, 0x03, 0x84, 0x44, 0xa1, 0x01,
	0x18, 0x29, 0xa0, 0xf6, 0x58, 0x20, 0x8c, 0xaf,
	0x92, 0x83, 0xb1, 0x36, 0x66, 0xca, 0x4e, 0x50,
	0xf7, 0xa1, 0xee, 0xe8, 0x6b, 0xa4, 0x0b, 0x5e,
	0x6a, 0x1d, 0x2c, 0xa3, 0x9f, 0x74, 0x98, 0xb6,
	0xa6, 0xa7, 0xbe, 0x8d, 0x8d, 0x67
};

uint8_t test_vector3[] = {
	0xa3, 0x01, 0x84, 0x44, 0xa1, 0x03, 0x18, 0x2a,
	0xa0, 0xf6, 0x81, 0x83, 0x43, 0xa1, 0x01, 0x26, 0xa1, 0x04,
	0x58, 0x20, 0x53, 0x7a, 0xc9, 0x3a, 0xc9, 0x09, 0xe7, 0x99,
	0x90, 0x91, 0x4c, 0xaa, 0x00, 0xfe, 0x87, 0xee, 0xea, 0x63,
	0x7e, 0xf8, 0x9b, 0x55, 0x12, 0xe5, 0xcb, 0x6e, 0x55, 0x8a,
	0x13, 0x6f, 0xf9, 0x8d, 0x58, 0x47, 0x30, 0x45, 0x02, 0x21,
	0x00, 0x83, 0x0c, 0xf1, 0x42, 0xcc, 0x4a, 0xdf, 0x56, 0x33,
	0x92, 0xdc, 0x7e, 0x04, 0x30, 0x00, 0x01, 0x58, 0xbf, 0x37,
	0x20, 0xb2, 0x8b, 0x7c, 0xea, 0x38, 0x8b, 0x0f, 0x1a, 0x5f,
	0x89, 0x18, 0xa8, 0x02, 0x20, 0x1d, 0xef, 0x2d, 0xf3, 0x4d,
	0x6a, 0xbd, 0x3b, 0x17, 0xc3, 0x42, 0x55, 0x73, 0xff, 0x2b,
	0x7c, 0xac, 0xae, 0x3d, 0xd0, 0x85, 0xe1, 0x1d, 0xfc, 0x23,
	0xbf, 0x0c, 0x60, 0xbe, 0x51, 0xb7, 0xda, 0x02, 0x58, 0xb9,
	0xa6, 0x01, 0x01,
	0x02, 0x02, 0x03, 0xa1, 0x01, 0x82, 0x82, 0x01, 0x50, 0xfa,
	0x6b, 0x4a, 0x53, 0xd5, 0xad, 0x5f, 0xdf, 0xbe, 0x9d, 0xe6,
	0x63, 0xe4, 0xd4, 0x1f, 0xfe, 0x82, 0x02, 0x50, 0x6e, 0x04,
	0xd3, 0xc2, 0x48, 0x87, 0x59, 0xe4, 0xa5, 0x97, 0xb5, 0xe7,
	0xcd, 0x49, 0x76, 0x53, 0x05, 0x81, 0xa3, 0x01, 0x81, 0x41,
	0x30, 0x02, 0x18, 0x25, 0x03, 0x84, 0x44, 0xa1, 0x01, 0x18,
	0x29, 0xa0, 0xf6, 0x58, 0x20, 0x8c, 0xaf, 0x92, 0x83, 0xb1,
	0x36, 0x66, 0xca, 0x4e, 0x50, 0xf7, 0xa1, 0xee, 0xe8, 0x6b,
	0xa4, 0x0b, 0x5e, 0x6a, 0x1d, 0x2c, 0xa3, 0x9f, 0x74, 0x98,
	0xb6, 0xa6, 0xa7, 0xbe, 0x8d, 0x8d, 0x67, 0x06, 0xa1, 0x01,
	0x81, 0xa2, 0x01, 0x81, 0x41, 0x30, 0x02, 0x81, 0xa2, 0x01,
	0x82, 0x01, 0x01, 0x03, 0x82, 0x00, 0x76, 0x68, 0x74, 0x74,
	0x70, 0x3a, 0x2f, 0x2f, 0x66, 0x6f, 0x6f, 0x2e, 0x62, 0x61,
	0x72, 0x2f, 0x62, 0x61, 0x7a, 0x2e, 0x62, 0x69, 0x6e, 0x08,
	0x84, 0x44, 0xa1, 0x01, 0x18, 0x29, 0xa0, 0xf6, 0x58, 0x20,
	0x4e, 0x27, 0x14, 0x59, 0x84, 0x79, 0xd8, 0xb6, 0x63, 0x48,
	0x05, 0xdf, 0x50, 0x19, 0xef, 0x34, 0x20, 0xed, 0xff, 0x03,
	0x29, 0x89, 0x4a, 0xcc, 0x91, 0xde, 0x8c, 0x8d, 0xe1, 0x6f,
	0xb0, 0xcf, 0x06, 0x58, 0xcc,
	0xa1, 0x01, 0x78, 0xc8, 0x4c, 0x6f, 0x72, 0x65, 0x6d, 0x20,
	0x69, 0x70, 0x73, 0x75, 0x6d, 0x20, 0x64, 0x6f, 0x6c, 0x6f,
	0x72, 0x20, 0x73, 0x69, 0x74, 0x20, 0x61, 0x6d, 0x65, 0x74,
	0x2c, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x63, 0x74, 0x65,
	0x74, 0x75, 0x72, 0x20, 0x61, 0x64, 0x69, 0x70, 0x69, 0x73,
	0x63, 0x69, 0x6e, 0x67, 0x20, 0x65, 0x6c, 0x69, 0x74, 0x2e,
	0x20, 0x4e, 0x75, 0x6e, 0x63, 0x20, 0x73, 0x65, 0x64, 0x20,
	0x74, 0x69, 0x6e, 0x63, 0x69, 0x64, 0x75, 0x6e, 0x74, 0x20,
	0x61, 0x6e, 0x74, 0x65, 0x2c, 0x20, 0x61, 0x20, 0x73, 0x6f,
	0x64, 0x61, 0x6c, 0x65, 0x73, 0x20, 0x6c, 0x69, 0x67, 0x75,
	0x6c, 0x61, 0x2e, 0x20, 0x50, 0x68, 0x61, 0x73, 0x65, 0x6c,
	0x6c, 0x75, 0x73, 0x20, 0x75, 0x6c, 0x6c, 0x61, 0x6d, 0x63,
	0x6f, 0x72, 0x70, 0x65, 0x72, 0x20, 0x6f, 0x64, 0x69, 0x6f,
	0x20, 0x63, 0x6f, 0x6d, 0x6d, 0x6f, 0x64, 0x6f, 0x20, 0x69,
	0x70, 0x73, 0x75, 0x6d, 0x20, 0x65, 0x67, 0x65, 0x73, 0x74,
	0x61, 0x73, 0x2c, 0x20, 0x76, 0x69, 0x74, 0x61, 0x65, 0x20,
	0x6c, 0x61, 0x63, 0x69, 0x6e, 0x69, 0x61, 0x20, 0x6c, 0x65,
	0x6f, 0x20, 0x6f, 0x72, 0x6e, 0x61, 0x72, 0x65, 0x2e, 0x20,
	0x53, 0x75, 0x73, 0x70, 0x65, 0x6e, 0x64, 0x69, 0x73, 0x73,
	0x65, 0x20, 0x70, 0x6f, 0x73, 0x75, 0x65, 0x72, 0x65, 0x20,
	0x73, 0x65, 0x64, 0x2e,
};

uint8_t test_vector4[] = {
	0xa2, 0x01, 0xf6, 0x02, 0x58, 0x6d, 0xa4, 0x01,
	0x01, 0x02, 0x02, 0x04, 0x81, 0xa3, 0x01, 0x82,
	0x45, 0x46, 0x6c, 0x61, 0x73, 0x68, 0x43, 0x00,
	0x34, 0x01, 0x02, 0x19, 0x87, 0xd0, 0x03, 0x82,
	0x01, 0x58, 0x20, 0x00, 0x11, 0x22, 0x33, 0x44,
	0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc,
	0xdd, 0xee, 0xff, 0x01, 0x23, 0x45, 0x67, 0x89,
	0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76,
	0x54, 0x32, 0x10, 0x09, 0x58, 0x2d, 0x83, 0xa1,
	0x0b, 0x00, 0xa1, 0x10, 0xa1, 0x06, 0x58, 0x20,
	0x81, 0x82, 0x00, 0x78, 0x1b, 0x68, 0x74, 0x74,
	0x70, 0x3a, 0x2f, 0x2f, 0x65, 0x78, 0x61, 0x6d,
	0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x2f,
	0x66, 0x69, 0x6c, 0x65, 0x2e, 0x62, 0x69, 0x6e,
	0xa1, 0x14, 0xf6
};

static struct OuterWrapper outerwrapper = {0};
static struct SUIT_Outer_Wrapper outerwrapper4 = {0};

// Example 9.1 from draft-moran-suit-manifest-03
void test_1(void)
{
	uint32_t decode_len;
	uint8_t expected_tag[] = {
		0x8c, 0xaf, 0x92, 0x83, 0xb1, 0x36, 0x66, 0xca,
		0x4e, 0x50, 0xf7, 0xa1, 0xee, 0xe8, 0x6b, 0xa4,
		0x0b, 0x5e, 0x6a, 0x1d, 0x2c, 0xa3, 0x9f, 0x74,
		0x98, 0xb6, 0xa6, 0xa7, 0xbe, 0x8d, 0x8d, 0x67,
	};
	zassert_true(cbor_decode_OuterWrapper(test_vector2,
				sizeof(test_vector2),
				&outerwrapper, &decode_len),
			"test_vector2 failed");
	zassert_equal(sizeof(test_vector2), decode_len, NULL);
	zassert_equal(2, outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_sequence,
			"Sequence number incorrect (%d)", outerwrapper._OuterWrapper_manifest_cbor._Manifest_sequence);
	zassert_true(outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads_present,
			"Expected payloads entry");
	zassert_equal(1, outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads_present,
			"Expected payloads present");
	zassert_equal(1, outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads
			._Manifest_payloads__PayloadInfo_count,
			"Expected single payloads entry, was %d",
			outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads
			._Manifest_payloads__PayloadInfo_count);
	zassert_mem_equal(expected_tag,
			outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads
			._Manifest_payloads__PayloadInfo[0]
			._PayloadInfo_payloadDigest
			._COSE_Mac0_tag
			.value,
			sizeof(expected_tag),
			"Expected a certain payloads tag");
}

// Example 9.3 from draft-moran-suit-manifest-03
void test_2(void)
{
	uint32_t decode_len;
	memset(&outerwrapper, 0, sizeof(struct OuterWrapper));
	char expected_updateDescription[] =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nunc "
		"sed tincidunt ante, a sodales ligula. Phasellus ullamcorper "
		"odio commodo ipsum egestas, vitae lacinia leo ornare. "
		"Suspendisse posuere sed.";
	zassert_true(cbor_decode_OuterWrapper(test_vector3,
				sizeof(test_vector3),
				&outerwrapper, &decode_len),
			"test_vector3 failed");
	zassert_equal(sizeof(test_vector3), decode_len, NULL);
	zassert_equal(2, outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_sequence,
			"Sequence number incorrect (%d)", outerwrapper._OuterWrapper_manifest_cbor._Manifest_sequence);
	zassert_equal(1, outerwrapper
			._OuterWrapper_textExt_present,
			"Expected text present");
	zassert_equal(1, outerwrapper
			._OuterWrapper_textExt
			._OuterWrapper_textExt_cbor_count,
			"Expected single text entry");
	zassert_equal(1, outerwrapper
			._OuterWrapper_textExt
			._OuterWrapper_textExt_cbor_count,
			"Expected single text entry");
	zassert_equal(1, outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads
			._Manifest_payloads__PayloadInfo_count,
			"Expected single payloads entry, was %d",
			outerwrapper
			._OuterWrapper_manifest_cbor
			._Manifest_payloads
			._Manifest_payloads__PayloadInfo_count);
	zassert_mem_equal(expected_updateDescription,
			outerwrapper
			._OuterWrapper_textExt
			._OuterWrapper_textExt_cbor[0]
			._Text_inttstr[0]
			._Text_inttstr
			.value,
			sizeof(expected_updateDescription) - 1,
			"Expected lorem ipsum text.");
}

// Example 1 from draft-moran-suit-manifest-04
void test_3(void)
{
	uint32_t decode_len;
	char expected_uri[] = "http://example.com/file.bin";
	memset(&outerwrapper4, 0, sizeof(struct SUIT_Outer_Wrapper));
	zassert_true(cbor_decode_SUIT_Outer_Wrapper(test_vector4,
		sizeof(test_vector4), &outerwrapper4, &decode_len), "test_vector failed");
	zassert_equal(sizeof(test_vector4), decode_len, NULL);
	zassert_equal(2,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_manifest_sequence_number,
		      "Sequence number incorrect");
	zassert_equal(1,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install_present,
		      "Expected install present");
	zassert_equal(_SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_choice,
		      "Expected install present");
	zassert_equal(3,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command_count,
		      "Expected x commands.");
	zassert_equal(_SUIT_Command_union__SUIT_Directive,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[0]
		      ._SUIT_Command_union_choice,
		      "Expected directive");
	zassert_equal(_SUIT_Directive_SUIT_Directive_Set_Component_Index,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[0]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_choice,
		      "Expected Component_Index");
	zassert_equal(_SUIT_Directive_SUIT_Directive_Set_Component_Index_uint,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[0]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_SUIT_Directive_Set_Component_Index_choice,
		      NULL);
	zassert_equal(_SUIT_Command_union__SUIT_Directive,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union_choice,
		      "Expected directive");
	zassert_equal(_SUIT_Directive_SUIT_Directive_Set_Parameters,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_choice,
		      "Expected Set_Parameters");
	zassert_equal(1,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_SUIT_Directive_Set_Parameters__SUIT_Parameters_count,
		      "Expected 1 parameter");
zassert_equal(_SUIT_Parameters_SUIT_Parameter_URI_List,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_SUIT_Directive_Set_Parameters__SUIT_Parameters[0]
		      ._SUIT_Parameters_choice,
		      "Expected uri list parameter");
	zassert_equal(1,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_SUIT_Directive_Set_Parameters__SUIT_Parameters[0]
		      ._SUIT_Parameters_SUIT_Parameter_URI_List_cbor
		      ._SUIT_URI_List__SUIT_Prioritized_URI_count,
		      "Expected 1 uri");
	zassert_equal(0,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_SUIT_Directive_Set_Parameters__SUIT_Parameters[0]
		      ._SUIT_Parameters_SUIT_Parameter_URI_List_cbor
		      ._SUIT_URI_List__SUIT_Prioritized_URI[0]
		      ._SUIT_Prioritized_URI_priority,
		      "Expected priority x");
	zassert_mem_equal(expected_uri,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[1]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_SUIT_Directive_Set_Parameters__SUIT_Parameters[0]
		      ._SUIT_Parameters_SUIT_Parameter_URI_List_cbor
		      ._SUIT_URI_List__SUIT_Prioritized_URI[0]
		      ._SUIT_Prioritized_URI_uri.value,
		      sizeof(expected_uri) - 1,
		      "Expected example.com uri");
	zassert_equal(_SUIT_Command_union__SUIT_Directive,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[2]
		      ._SUIT_Command_union_choice,
		      "Expected directive");
	zassert_equal(_SUIT_Directive_SUIT_Directive_Fetch,
		      outerwrapper4
		      ._SUIT_Outer_Wrapper_suit_manifest_cbor
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Manifest_suit_install
		      ._SUIT_Severable_Command_Sequence3_SUIT_Command_Sequence_cbor
		      ._SUIT_Command_Sequence__SUIT_Command[2]
		      ._SUIT_Command_union__SUIT_Directive
		      ._SUIT_Directive_choice,
		      "Expected Directive_Fetch");
}


void test_main(void)
{
	ztest_test_suite(cbor_decode_test,
			 ztest_unit_test(test_1),
			 ztest_unit_test(test_2),
			 ztest_unit_test(test_3)
	);
	ztest_run_test_suite(cbor_decode_test);
}
