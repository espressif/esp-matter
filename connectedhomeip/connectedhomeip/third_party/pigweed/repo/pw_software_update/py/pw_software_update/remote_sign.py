# Copyright 2021 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Upload update bundles to a GCS bucket for signing.

This module does not implement any actual signing logic; that is left up to the
remote signing service. This module takes care of uploading bundles to GCS for
signing, waiting the the signed version to appear, and downloading the signed
bundle from the output bucket. It can be used either as an entry point by
invoking it as a runnable module and providing all the necessary arguments (run
the tool with --help for details), or as a library by instantiating
RemoteSignClient and calling its sign() method.

The expected API for the remote signing service consists of the following:

    - A pair of GCS buckets. One bucket to serve as a queue of update bundles
      to be signed, and the other bucket to serve as the output area where
      signed bundles are deposited.

    - Three artifacts should be placed into the input queue bucket:
          1. The update bundle to be signed
          2. A signing request file whose name ends with "signing_request.json"
          3. A public builder key named "<signing_request_name>.public_key.pem"

Builder keys are used to generate intermediate signatures for the signing
request. Specifically, a private builder key is used to generate an
intermediate signature for both the update bundle to be signed, and the signing
request file. These signatures are then added to the GCS blob metadata for
their respective blobs. The corresponding public builder key is uploaded
alongside the signing request.

The signing service should be set up to trigger whenever a new signing request
file is added anywhere inside the input queue bucket. The signing request file
is a JSON file with the following fields:

    remote_signing_key_name: A string that should correspond to the name of a
        signing key known to the remote signing service.

    bundle_path: The path (relative to GCS input bucket root) to an update
        bundle to be signed by the remote signing service.

    bundle_public_key_path: The path (relative to GCS input bucket root) to a
        public builder key .pem file corresponding to the private builder key
        that was used to sign the update bundle and signing request file.

    output_bucket: Name of the output GCS bucket into which the remote signing
        service should place signed artifacts.

    output_path: The path (relative to the GCS output bucket root) at which the
        signed update bundle should be deposited by the remote signing service.

On the remote side, the signing service is expected to check the public builder
key against its list of allowed builder keys. Provided the key is found in the
allow list, the signing service should use it to verify the intermediate
signatures of both the update bundle to be signed and the signing request file.
If the key is not found in the allow list, or if the signature on the update
bundle or signing request file do not match the public builder key, the signing
service should reject the signing request.

If the builder public key is found in the allow list and the intermediate
signatures are verified, the signing service should produce a signed version of
the update bundle, and place it the GCS output bucket at the specified path.

In order to authenticate to GCS, the "Application Default Credentials" will be
used. This assumes remote_sign.py is running in an environment that provides
such credentials. See the Google cloud platform documentation for details:

    https://cloud.google.com/docs/authentication/production

For development purposes, it is possible to provide an alternate method of
authenticating to GCS. The alternate authentication method should be a Python
module that is importable in the running Python environment. The module should
define a 'get_credentials()' function that takes no arguments and returns an
instance of google.auth.credentials.Credentials.
"""
import argparse
import base64
import importlib
import json
from pathlib import Path
import time
from typing import Dict, Optional, Union

from cryptography.hazmat import backends
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ed25519, padding, rsa
from google.auth.credentials import Credentials  # type: ignore
from google.cloud import storage  # type: ignore
from google.cloud.storage.bucket import Bucket  # type: ignore

DEFAULT_TIMEOUT_S = 600

PathOrBytes = Union[Path, bytes]


def _parse_args():
    """Parse CLI aguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        '--project', help='GCP project that owns storage buckets.'
    )
    parser.add_argument(
        '--input-bucket', help='GCS bucket used as a signing queue'
    )
    parser.add_argument(
        '--output-bucket', help='GCS bucket to watch for signed bundles'
    )
    parser.add_argument(
        '--bundle', type=Path, help='Update bundle to upload for signing'
    )
    parser.add_argument(
        '--out', type=Path, help='Path to which to download signed bundle'
    )
    parser.add_argument(
        '--signing-key-name',
        help='Name of signing key remote signing service should use',
    )
    parser.add_argument(
        '--builder-key',
        type=Path,
        help='Path to builder private key for intermediate signatures',
    )
    parser.add_argument(
        '--builder-public-key', type=Path, help='Path to builder public key'
    )
    parser.add_argument(
        '--bundle-blob-name',
        default=None,
        help='Path in the input bucket at which to upload bundle',
    )
    parser.add_argument(
        '--request-blob-name',
        default=None,
        help='Path in the input bucket at which to put signing request',
    )
    parser.add_argument(
        '--signed-bundle-blob-name',
        default=None,
        help='Path in the output bucket for the signed bundle',
    )
    parser.add_argument(
        '--dev-gcs-auth-module-override',
        default=None,
        help='Developer use only; custom auth module to use with GCS.',
    )
    parser.add_argument(
        '--timeout',
        type=int,
        default=DEFAULT_TIMEOUT_S,
        help='Seconds to wait for signed bundle to appeaer before giving up.',
    )

    return parser.parse_args()


class BlobExistsError(Exception):
    """Raised if the blob to be uploaded already exists in the input bucket."""


class RemoteSignClient:
    """GCS client for use in remote signing."""

    def __init__(self, input_bucket: Bucket, output_bucket: Bucket):
        # "Application Default Credentials" are used implicitly when None is
        # passed to Client() as credentials. See the cloud docs for details:
        #   https://cloud.google.com/docs/authentication/production
        self._input_bucket = input_bucket
        self._output_bucket = output_bucket

    @classmethod
    def from_names(
        cls,
        project_name: str,
        input_bucket_name: str,
        output_bucket_name: str,
        gcs_credentials: Optional[Credentials] = None,
    ):
        storage_client = storage.Client(
            project=project_name, credentials=gcs_credentials
        )
        return cls(
            input_bucket=storage_client.bucket(input_bucket_name),
            output_bucket=storage_client.bucket(output_bucket_name),
        )

    def sign(
        self,
        bundle: Path,
        signing_key_name: str,
        builder_key: Path,
        builder_public_key: Path,
        bundle_blob_name: Optional[str] = None,
        request_blob_name: Optional[str] = None,
        signed_bundle_blob_name: Optional[str] = None,
        request_overrides: Optional[Dict] = None,
        timeout_s: int = DEFAULT_TIMEOUT_S,
    ) -> bytes:
        """Upload file to GCS and download signed counterpart when ready.

        Args:
          bundle: Path object for an UpdateBundle to upload for signing.
          signing_key_name: Name of remote signing key to use for signing.
          builder_key: Path to builder private key for intermediate signature.
          builder_public_key: Path to corresponding builder public key.
          bundle_blob_name: GCS path at which to upload bundle to sign.
          request_blob_name: GCS path at which to upload request file.
          signed_bundle_blob_name: GCS path in output bucket to request.
          request_overrides: Dict of signing request JSON keys and values to
              add to the signing requests. If this dict contains any keys whose
              values are already in the signing request, the existing values
              will be overwritten by the ones passed in here.
          timeout_s: Maximum seconds to wait for output before failing.
        """
        if bundle_blob_name is None:
            bundle_blob_name = bundle.name

        if request_blob_name is None:
            request_blob_name = f't{time.time()}_signing_request.json'

        if not request_blob_name.endswith('signing_request.json'):
            raise ValueError(
                f'Signing request blob name {request_blob_name}'
                ' does not end with "signing_request.json".'
            )

        request_name = request_blob_name[:-5]  # strip the ".json"
        builder_public_key_blob_name = f'{request_name}.publickey.pem'

        if signed_bundle_blob_name is None:
            signed_bundle_blob_name = f'{bundle.name}.signed'

        signing_request = {
            'remote_signing_key_names': [signing_key_name],
            'bundle_path': bundle_blob_name,
            'bundle_public_key_path': builder_public_key_blob_name,
            'output_bucket': self._output_bucket.name,
            'output_path': signed_bundle_blob_name,
        }

        if request_overrides is not None:
            signing_request.update(request_overrides)

        builder_public_key_blob = self._input_bucket.blob(
            builder_public_key_blob_name
        )
        bundle_blob = self._input_bucket.blob(bundle_blob_name)
        request_blob = self._input_bucket.blob(request_blob_name)

        for blob in (builder_public_key_blob, bundle_blob, request_blob):
            if blob.exists():
                raise BlobExistsError(
                    f'A blob named "{blob}" already exists in the input bucket.'
                    ' A unique blob name is required for uploading.'
                )

        builder_public_key_blob.upload_from_filename(str(builder_public_key))

        bundle_blob.metadata = {
            'signature': self._get_builder_signature(
                bundle, builder_key
            ).decode('ascii')
        }
        bundle_blob.upload_from_filename(str(bundle))

        encoded_json = bytes(json.dumps(signing_request), 'utf-8')
        request_blob.metadata = {
            'signature': self._get_builder_signature(
                encoded_json, builder_key
            ).decode('ascii')
        }

        # Despite its name, the upload_from_string() method can take either a
        # str or bytes object; here we already pre-encoded the string in utf-8.
        request_blob.upload_from_string(encoded_json)

        return self._wait_for_blob(signed_bundle_blob_name, timeout_s=timeout_s)

    def _wait_for_blob(
        self,
        blob_name,
        interval: int = 1,
        max_tries: Optional[int] = None,
        timeout_s: int = DEFAULT_TIMEOUT_S,
    ) -> storage.Blob:
        """Wait for a specific blob to appear in the output bucket.

        Args:
          blob_name: Name of the blob to wait for.
          interval: Time (seconds) to wait between checks for blob's existence.
          max_tries: Number of times to check for the blob before failing.
          timeout_s: Maximum seconds to keep watching before failing.
        """
        blob = self._output_bucket.blob(blob_name)
        end_time = time.time() + timeout_s
        tries = 0
        while max_tries is None or tries < max_tries:
            if time.time() > end_time:
                raise FileNotFoundError(
                    'Timed out while waiting for signed blob.'
                )
            if blob.exists():
                return blob
            tries += 1
            time.sleep(interval)

        raise FileNotFoundError(
            'Too many retries while waiting for signed blob.'
        )

    @staticmethod
    def _get_builder_signature(data: PathOrBytes, key: Path) -> bytes:
        """Generate a base64-encided builder signature for file.

        In order for the remote signing system to have some level of trust in
        the artifacts it's signing, an intermediate signature is used to verify
        that the artifacts came from an approved builder.
        """
        if isinstance(data, Path):
            data = data.read_bytes()

        private_key = serialization.load_pem_private_key(
            key.read_bytes(), None, backends.default_backend()
        )

        if isinstance(private_key, ed25519.Ed25519PrivateKey):
            signature = private_key.sign(data)

        elif isinstance(private_key, rsa.RSAPrivateKey):
            signature = private_key.sign(
                data,  # type: ignore
                padding=padding.PSS(
                    mgf=padding.MGF1(hashes.SHA256()),
                    salt_length=padding.PSS.MAX_LENGTH,
                ),
                algorithm=hashes.SHA256(),
            )

        else:
            raise TypeError(
                f'Key {private_key} has unsupported type'
                f' ({type(private_key)}). Valid types are'
                ' Ed25519PrivateKey and RSAPrivateKey.'
            )

        return base64.b64encode(signature)


def _credentials_from_module(module_name: str) -> Credentials:
    """Return GCS Credential from the named auth module.

    The module name should correspond to a module that's importable in the
    running Python environment. It must define a get_credentials() function
    that takes no args and returns a Credentials instance.
    """
    auth_module = importlib.import_module(module_name)
    return auth_module.get_credentials()  # type: ignore


def main(  # pylint: disable=too-many-arguments
    project: str,
    input_bucket: str,
    output_bucket: str,
    bundle: Path,
    out: Path,
    signing_key_name: str,
    builder_key: Path,
    builder_public_key: Path,
    bundle_blob_name: str,
    request_blob_name: str,
    signed_bundle_blob_name: str,
    dev_gcs_auth_module_override: str,
    timeout: int,
) -> None:
    """Send bundle for remote signing and write signed counterpart to disk.

    Args:
        project: Project name for GCS project containing signing bucket pair.
        input_bucket: Name of GCS bucket to deposit to-be-signed artifacts in.
        output_bucket: Name of GCS bucket to watch for signed artifacts.
        bundle: Update bundle to be signed.
        out: Path to which to download signed version of bundle.
        signing_key_name: Name of key the remote signing service should use.
        bundle_blob_name: Path in input bucket to upload bundle to.
        request_blob_name: Path in input bucket to upload signing request to.
        signed_bundle_blob_name: Output bucket path for signed bundle.
        dev_gcs_auth_module_override: For developer use only; optional module
            to use to generate GCS client credentials. Must be importable in
            the running Python environment, and must define a get_credentials()
            function that takes no args and returns a Credentials instance.
        timeout: Seconds to wait for signed bundle before giving up.
    """
    credentials = None
    if dev_gcs_auth_module_override is not None:
        credentials = _credentials_from_module(dev_gcs_auth_module_override)

    remote_sign_client = RemoteSignClient.from_names(
        project_name=project,
        input_bucket_name=input_bucket,
        output_bucket_name=output_bucket,
        gcs_credentials=credentials,
    )

    signed_bundle = remote_sign_client.sign(
        bundle,
        signing_key_name,
        builder_key,
        builder_public_key,
        bundle_blob_name,
        request_blob_name,
        signed_bundle_blob_name,
        timeout_s=timeout,
    )

    out.write_bytes(signed_bundle.download_as_bytes())


if __name__ == '__main__':
    main(**vars(_parse_args()))
