"""
This file contains utility/convenience functions to:
    * Read out PEM files created with openssl
    * sign and verify payloads
    * Print a C code buffer
    * writing signatures to HEX files
"""

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.serialization import load_pem_private_key
from ecdsa import SigningKey, VerifyingKey, NIST256p, BadSignatureError
import binascii
import hashlib
import logging
import codecs
import six


def long_to_bytes(s):
    h = '%x' % s
    return codecs.decode('0' * (len(h) % 2) + h, 'hex')


def bytes_to_long(s):
    if six.PY2:
        return long(codecs.encode(s, 'hex'), 16)
    return int(codecs.encode(s, 'hex'), 16)


def getPrivatePublicKeysFromPEMFile(pemfilePath: str, password: bytes) -> (str, bytes, bytes):
    with open(pemfilePath, "rb") as file_handle:
        pem_file_data = file_handle.read(-1)

    private_key_object = serialization.load_pem_private_key(pem_file_data, password, default_backend())

    # Get Private key
    privKey = private_key_object.private_numbers().private_value
    privKey = binascii.unhexlify(f"{privKey:x}")

    # Or another way which works for both p192 and p256
    pubKey = long_to_bytes(private_key_object.public_key().public_numbers().x) + long_to_bytes(
        private_key_object.public_key().public_numbers().y)

    # Return correctly formatted private/public key
    return (private_key_object.curve.name, privKey, pubKey)


def hashMessage(message, hashfunc=hashlib.sha256):
    """Calculate hash of @p message"""
    m = hashfunc()
    m.update(message)
    return m.hexdigest()


def signMessage(message, privateKey, curve=NIST256p, hashfunc=hashlib.sha256) -> bytes:
    """ Sign a arbitrary message with privateKey"""
    privKeyInstance = SigningKey.from_string(privateKey, curve=curve)

    return privKeyInstance.sign(message, hashfunc=hashfunc)


def verifyMessage(message, publicKey, signature, curve=NIST256p, hashfunc=hashlib.sha256):
    """ Verify that signature of message is created with private key associated with publicKey"""
    pubKeyInstance = VerifyingKey.from_string(publicKey, curve=curve)

    try:
        if pubKeyInstance.verify(signature, message, hashfunc=hashfunc):
            return True
    except BadSignatureError:
        return False

    return False


def getCCodeBuffer(payload, name):
    """Given a payload string and name, output a C buffer"""
    # if not isinstance(payload, str):
    #     payload = "".join((chr(x) for x in payload))

    cCodeString = "UInt8 %s[%d] = {0x" % (name, len(payload))
    payload = codecs.decode(codecs.encode(payload, 'hex'), 'ascii')
    t = iter(payload)
    cCodeString += ',0x'.join(a + b for a, b in zip(t, t))
    cCodeString += "};"
    return cCodeString


def printCCodeBuffer(payload, name):
    logging.info(getCCodeBuffer(payload, name))


def addSignatureToIntelHexObject(ihObject, signature, address):
    """ Write generated signature into hex object @ signatureAddress
    """
    payload = codecs.decode(codecs.encode(signature, 'hex'), 'ascii')
    t = iter(payload)
    for idx, byteValue in enumerate(zip(t, t)):
        byteValue = ''.join(byteValue)
        byteValue = int(byteValue, 16)
        ihObject[address + idx] = byteValue
        # logging.info("ih[0x%08x] = 0x%2x" % (signatureAddress+USER_LICENSE_SIGNATURE_OFFSET+idx, byteValue))

    logging.info("%d-byte signature written in user license @ 0x%08x" % (len(signature), address))


def convertKey(input, input_type, output_type):
    """
    given an input and input type, convert key to output_type and return data

    types are:
    0 : byte-string: '\xfc\x10|\xad\xb2\xd8\x19\x1cc\x00m*\xd86eF\xb2 S\xb1\xd7,x\\\xd8\xc6\x02\x061Jh\xea'
    1 : long integer: 0xfc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68e
    2 : hex string without 0x: "fc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea"
    3 : hex string with 0x: "0xfc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea"
    4 : array of integers:  [0xfc,0x10,0x7c,0xad,0xb2,0xd8,0x19,0x1c,0x63,0x00,0x6d,0x2a,0xd8,0x36,0x65,0x46,0xb2,0x20,0x53,0xb1,0xd7,0x2c,0x78,0x5c,0xd8,0xc6,0x02,0x06,0x31,0x4a,0x68,0xea]
    """
    if input_type == 0:
        interMediateRepresentation = input
    elif input_type == 1:
        # Hexadecimal long integer
        # e.g. : 0xfc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea
        # or :
        interMediateRepresentation = long_to_bytes(input)
    elif input_type == 2:
        # hexstring
        # or : "fc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea"
        interMediateRepresentation = input.decode('hex')
    elif input_type == 3:
        interMediateRepresentation = input[2:].decode('hex')
    elif input_type == 4:
        # array of bytes
        # e.g. : [255, 255, 255, 255, ...]
        # or : [0xFF, 0xAB,0xFF, 0xCD, ...]
        import array
        interMediateRepresentation = array.array('B', input).tostring()
    else:
        assert False

    if output_type == 0:
        return interMediateRepresentation

    if output_type == 1:
        return bytes_to_long(interMediateRepresentation)

    if output_type == 2:
        return codecs.encode(interMediateRepresentation, 'hex')

    if output_type == 3:
        return "0x" + codecs.encode(interMediateRepresentation, 'hex')

    if output_type == 4:
        b = bytearray(interMediateRepresentation)
        return list(b)

    assert False


logging.basicConfig(level=logging.INFO)


def test_convert_key():
    # Test Vectors with same values in different formats
    listOfTestVectors = [b'\xfc\x10|\xad\xb2\xd8\x19\x1cc\x00m*\xd86eF\xb2 S\xb1\xd7,x\\\xd8\xc6\x02\x061Jh\xea',
                         0xfc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea,
                         "fc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea",
                         "0xfc107cadb2d8191c63006d2ad8366546b22053b1d72c785cd8c60206314a68ea",
                         [0xfc, 0x10, 0x7c, 0xad, 0xb2, 0xd8, 0x19, 0x1c, 0x63, 0x00, 0x6d, 0x2a, 0xd8, 0x36, 0x65,
                          0x46,
                          0xb2, 0x20, 0x53, 0xb1, 0xd7, 0x2c, 0x78, 0x5c, 0xd8, 0xc6, 0x02, 0x06, 0x31, 0x4a, 0x68,
                          0xea],
                         ]

    # Check all same values against eachother
    for idx, inputdata in enumerate(listOfTestVectors):
        for idx2, outputdata in enumerate(listOfTestVectors):
            assert convertKey(inputdata, idx, idx2) == outputdata


if __name__ == '__main__':
    test_convert_key()
