############################
Initial Attestation Verifier
############################
This is a parser and verifier script for an Initial Attestation Token,
the structure of which is described here:

https://tools.ietf.org/html/draft-tschofenig-rats-psa-token-01


************
Installation
************
You can install the script using pip:

.. code:: bash

   # Inside the directory containg this README
   pip install .

This should automatically install all the required dependencies. Please
see ``setup.py`` for the list of said dependencies.

*****
Usage
*****
After installing, you should have check_iat script in your PATH. The
script expects a single parameter – a path to the signed IAT in COSE
format.

You can find an example in the “sample” directory.

The script will extract the COSE payload and make sure that it is a
valid IAT (i.e. all mandatory fields are present, and all known
fields have correct size/type):

.. code:: bash

   $ check_iat sample/iat.cbor
   Token format OK

If you want the script to verify the signature, you need to specify the
file containing the signing key in PEM format using -k option. The key
used to sign sample/iat.cbor is inside sample/key.pem.

::

   $ check_iat -k sample/key.pem  sample/iat.cbor
   Signature OK
   Token format OK

You can add a -p flag to the invocation in order to have the script
print out the decoded IAT in JSON format. It should look something like
this:

.. code:: json

   {
       "INSTANCE_ID": "\u0001\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
       "IMPLEMENTATION_ID": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
       "CHALLEGE": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
       "CLIENT_ID": 2,
       "SECURITY_LIFECYCLE": 2,
       "VERSION": 1,
           "BOOT_SEED": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018"
       "SUBMOD": [
       {
           "SUBMOD_NAME": "BL",
           "SIGNER_ID": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
           "SUBMOD_VERSION": "3.4.2",
           "MEASUREMENT": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018"
       },
       {
           "SUBMOD_NAME": "M1",
           "SIGNER_ID": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
           "SUBMOD_VERSION": "3.4.2",
           "MEASUREMENT": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018"
       },
       {
           "SUBMOD_NAME": "M2",
           "SIGNER_ID": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
           "SUBMOD_VERSION": "3.4.2",
           "MEASUREMENT": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018"
       },
       {
           "SUBMOD_NAME": "M3",
           "SIGNER_ID": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018",
           "SUBMOD_VERSION": "3.4.2",
           "MEASUREMENT": "\u0007\u0006\u0005\u0004\u0003\u0002\u0001\u0000\u000f\u000e\r\f\u000b\n\t\b\u0017\u0016\u0015\u0014\u0013\u0012\u0011\u0010\u001f\u001e\u001d\u001c\u001b\u001a\u0019\u0018"
       }
       ]
   }


*******
Testing
*******
Tests can be run using ``nose2``:

.. code:: bash

   pip install nose2

Then run by executing ``nose2`` in the root directory.


*******************
Development Scripts
*******************
The following utility scripts are contained within ``dev_scripts``
subdirectory and were utilized in development of this tool. They are not
need to use the iat-verifier script, and can generally be ignored.

.. code:: bash

   ./dev_scripts/generate-key.py OUTFILE

Generate an ECDSA (NIST256p curve) signing key and write it in PEM
format to the specified file.

.. code:: bash

   ./dev_scripts/generate-sample-iat.py KEYFILE OUTFILE

Generate a sample token, signing it with the specified key, and writing
the output to the specified file.

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
