from unittest import TestCase, main
from subprocess import Popen, PIPE
from re import sub
from pathlib import Path


try:
    import cddl_gen
except ImportError:
    print("""
The cddl_gen package must be installed to run these tests.
During development, install with `python3 setup.py develop` to install in a way
that picks up changes in the files without having to reinstall.
""")
    import sys
    sys.exit(1)


p_root = Path(__file__).absolute().parents[2]
p_tests = Path(p_root, 'tests')
p_manifest = Path(p_tests, 'cases/manifest12.cddl')
p_test_vectors = tuple(Path(p_tests, f'cases/manifest12_example{i}.cborhex') for i in range(6))


class Testn(TestCase):
    def decode(self, ccdl_path, data_path):
        with open(ccdl_path, 'r') as f:
            my_types = cddl_gen.DataTranslator.from_cddl(f.read(), 16)
        cddl = my_types["SUIT_Envelope_Tagged"]
        with open(data_path, 'r') as f:
            data = bytes.fromhex(f.read().replace("\n", ""))
        self.decoded = cddl.decode_str(data)
        return


class Test0(Testn):
    def __init__(self, *args, **kwargs):
        super(Test0, self).__init__(*args, **kwargs)
        self.decode(p_manifest, p_test_vectors[0])

    def test_manifest_digest(self):
        self.assertEqual(
            bytes.fromhex("5c097ef64bf3bb9b494e71e1f2418eef8d466cc902f639a855ec9af3e9eddb99"),
            self.decoded.suit_authentication_wrapper.SUIT_Digest.suit_digest_bytes)

    def test_signature(self):
        self.assertEqual(
            1,
            self.decoded.suit_authentication_wrapper.SUIT_Authentication_Block[0].COSE_Sign1_Tagged.protected.uintint[0].uintint_key)
        self.assertEqual(
            -7,
            self.decoded.suit_authentication_wrapper.SUIT_Authentication_Block[0].COSE_Sign1_Tagged.protected.uintint[0].uintint)
        self.assertEqual(
            bytes.fromhex("a19fd1f23b17beed321cece7423dfb48c457b8f1f6ac83577a3c10c6773f6f3a7902376b59540920b6c5f57bac5fc8543d8f5d3d974faa2e6d03daa534b443a7"),
            self.decoded.suit_authentication_wrapper.SUIT_Authentication_Block[0].COSE_Sign1_Tagged.signature)

    def test_validate_run(self):
        self.assertEqual(
            "suit_condition_image_match",
            self.decoded.suit_manifest.SUIT_Unseverable_Members.suit_validate[0][0].SUIT_Condition.union_choice)
        self.assertEqual(
            "suit_directive_run",
            self.decoded.suit_manifest.SUIT_Unseverable_Members.suit_run[0][0].SUIT_Directive.union_choice)

    def test_image_size(self):
        self.assertEqual(34768, self.decoded.suit_manifest.suit_common.suit_common_sequence[0][0].SUIT_Common_Commands.suit_directive_override_parameters.map[3].suit_parameter_image_size)


class Test1(Testn):
    def __init__(self, *args, **kwargs):
        super(Test1, self).__init__(*args, **kwargs)
        self.decode(p_manifest, p_test_vectors[1])

    def test_components(self):
        self.assertEqual(
            [b'\x00'],
            self.decoded.suit_manifest.suit_common.suit_components[0][0].bstr)

    def test_uri(self):
        self.assertEqual(
            "http://example.com/file.bin",
            self.decoded.suit_manifest.SUIT_Severable_Manifest_Members.suit_install[0][0].SUIT_Directive.suit_directive_set_parameters.map[0].suit_parameter_uri)


class Test2(Testn):
    def __init__(self, *args, **kwargs):
        super(Test2, self).__init__(*args, **kwargs)
        self.decode(p_manifest, p_test_vectors[2])

    def test_severed_uri(self):
        self.assertEqual(
            "http://example.com/very/long/path/to/file/file.bin",
            self.decoded.SUIT_Severable_Manifest_Members.suit_install[0][0].SUIT_Directive.suit_directive_set_parameters.map[0].suit_parameter_uri)

    def test_severed_text(self):
        self.assertIn(
            "Example 2",
            self.decoded.SUIT_Severable_Manifest_Members.suit_text[0].SUIT_Text_Keys.suit_text_manifest_description[0])
        self.assertEqual(
            [b'\x00'],
            self.decoded.SUIT_Severable_Manifest_Members.suit_text[0].SUIT_Component_Identifier[0].SUIT_Component_Identifier_key.bstr)
        self.assertEqual(
            "arm.com",
            self.decoded.SUIT_Severable_Manifest_Members.suit_text[0].SUIT_Component_Identifier[0].SUIT_Component_Identifier.SUIT_Text_Component_Keys.suit_text_vendor_domain[0])
        self.assertEqual(
            "This component is a demonstration. The digest is a sample pattern, not a real one.",
            self.decoded.SUIT_Severable_Manifest_Members.suit_text[0].SUIT_Component_Identifier[0].SUIT_Component_Identifier.SUIT_Text_Component_Keys.suit_text_component_description[0])


class Test3(Testn):
    def __init__(self, *args, **kwargs):
        super(Test3, self).__init__(*args, **kwargs)
        self.decode(p_manifest, p_test_vectors[3])

    def test_A_B_offset(self):
        self.assertEqual(
            33792,
            self.decoded.suit_manifest.suit_common.suit_common_sequence[0][1].SUIT_Common_Commands.suit_directive_try_each.SUIT_Directive_Try_Each_Argument.SUIT_Command_Sequence[0].union[0].SUIT_Directive.suit_directive_override_parameters.map[0].suit_parameter_component_offset)
        self.assertEqual(
            541696,
            self.decoded.suit_manifest.suit_common.suit_common_sequence[0][1].SUIT_Common_Commands.suit_directive_try_each.SUIT_Directive_Try_Each_Argument.SUIT_Command_Sequence[1].union[0].SUIT_Directive.suit_directive_override_parameters.map[0].suit_parameter_component_offset)


class Test4(Testn):
    def __init__(self, *args, **kwargs):
        super(Test4, self).__init__(*args, **kwargs)
        self.decode(p_manifest, p_test_vectors[4])

    def test_load_decompress(self):
        self.assertEqual(
            0,
            self.decoded.suit_manifest.SUIT_Unseverable_Members.suit_load[0][1].SUIT_Directive.suit_directive_set_parameters.map[3].suit_parameter_source_component)
        self.assertEqual(
            "SUIT_Compression_Algorithm_zlib",
            self.decoded.suit_manifest.SUIT_Unseverable_Members.suit_load[0][1].SUIT_Directive.suit_directive_set_parameters.map[2].suit_parameter_compression_info.suit_compression_algorithm)


class Test5(Testn):
    def __init__(self, *args, **kwargs):
        super(Test5, self).__init__(*args, **kwargs)
        self.decode(p_manifest, p_test_vectors[5])

    def test_two_image_match(self):
        self.assertEqual(
            "suit_condition_image_match",
            self.decoded.suit_manifest.SUIT_Severable_Manifest_Members.suit_install[0][3].SUIT_Condition.union_choice)
        self.assertEqual(
            "suit_condition_image_match",
            self.decoded.suit_manifest.SUIT_Severable_Manifest_Members.suit_install[0][7].SUIT_Condition.union_choice)


class TestCLI(TestCase):
    def get_std_args(self, input):
        return ["cddl_gen", "--cddl", p_manifest, "--default-max-qty", "16", "convert", "--input", input, "-t", "SUIT_Envelope_Tagged"]

    def do_testn(self, n):
        call0 = Popen(self.get_std_args(p_test_vectors[n]) + ["--output", "-", "--output-as", "cbor"], stdout=PIPE)
        stdout0, _ = call0.communicate()
        self.assertEqual(0, call0.returncode)

        call1 = Popen(self.get_std_args("-") + ["--input-as", "cbor", "--output", "-", "--output-as", "json"], stdin=PIPE, stdout=PIPE)
        stdout1, _ = call1.communicate(input=stdout0)
        self.assertEqual(0, call1.returncode)

        call2 = Popen(self.get_std_args("-") + ["--input-as", "json", "--output", "-", "--output-as", "yaml"], stdin=PIPE, stdout=PIPE)
        stdout2, _ = call2.communicate(input=stdout1)
        self.assertEqual(0, call2.returncode)

        call3 = Popen(self.get_std_args("-") + ["--input-as", "yaml", "--output", "-", "--output-as", "cbor"], stdin=PIPE, stdout=PIPE)
        stdout3, _ = call3.communicate(input=stdout2)
        self.assertEqual(0, call3.returncode)

        self.assertEqual(stdout0, stdout3)

        call4 = Popen(self.get_std_args("-") + ["--input-as", "cbor", "--output", "-", "--output-as", "cborhex"], stdin=PIPE, stdout=PIPE)
        stdout4, _ = call4.communicate(input=stdout3)
        self.assertEqual(0, call4.returncode)

        call5 = Popen(self.get_std_args("-") + ["--input-as", "cborhex", "--output", "-", "--output-as", "json"], stdin=PIPE, stdout=PIPE)
        stdout5, _ = call5.communicate(input=stdout4)
        self.assertEqual(0, call5.returncode)

        self.assertEqual(stdout1, stdout5)

        self.maxDiff = None

        with open(p_test_vectors[n], 'r') as f:
            self.assertEqual(sub(r"\W+", "", f.read()), sub(r"\W+", "", stdout4.decode("utf-8")))

    def test_0(self):
        self.do_testn(0)

    def test_1(self):
        self.do_testn(1)

    def test_2(self):
        self.do_testn(2)

    def test_3(self):
        self.do_testn(3)

    def test_4(self):
        self.do_testn(4)

    def test_5(self):
        self.do_testn(5)


if __name__ == "__main__":
    main()
