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
"""Unit tests for pw_software_update.remote_sign module."""

import os
from pathlib import Path
import tempfile
import textwrap
import unittest
from unittest import mock

from google.cloud.storage import Blob  # type: ignore
from google.cloud.storage.bucket import Bucket  # type: ignore

from pw_software_update import remote_sign

# inclusive-language: disable
FAKE_BUILDER_KEY = textwrap.dedent(
    """\
    -----BEGIN RSA PRIVATE KEY-----
    MIIEpAIBAAKCAQEA4qEQSHM0QpWEhTvhWMBahS7wbTIihaiRpUQC8+hEkmHhoJQy
    zaNR3CKdYWnJ1bAjdBT1HTHznbYSBasFAUKPiB16K/akuKSPnwHG9OM6+8Psw7lt
    GLP2jP65HE4a8n9lGas399xIK4hxZJkV2BXocociXVEVB3nzzNk1AQZdJxik/ToL
    MYC2EKTu1kdt+OLl56/O1Mq9p8V7u2G1l8fqHtJi4Z34LzUzIoyFf7+bSmZBcHG1
    F/QdjbHb4temShDzptOM1VfXZchYTDVnbNsmR7TP2B857agog4rhqtVlPvHqFial
    WEU1WmAQz+oYqtRikUVWHq10SACxo6MFoM7LqQIDAQABAoIBAQCjrdoZyYLUGDLn
    G1FtDTgTesxQwWXnjNDsQMu1J2rnImSX2pE6rhtAV4u9QG9ys01X2I8Tr/EYVdh8
    WYE64LzTfR6ww+lCJjBIkjsEwVznWyEUV0bxEYEfYhWF2O9jdxkoyd2ZWXKSZnAn
    TN1W/LOui+UI6re6d5zatYGvpM4AnlMTmwcO5aPQqTZMBOqJQZgEgyyHH2DZpIRI
    L7dG/k9Y/ML4T+hSVvi84+NS7GyTajPtaNRoVnlwr9+QVKplIgT8ZSqAF7unBsmF
    +s/U+TCFKq0pOhamOVz8eVd/uusy0d7a2oomtKoIzcPd74J5KZMub8izmEOwp5TZ
    17rsBDuVAoGBAPJMv737tYf5T5ihwEJ84OWxq3qLwWSoNwezOF3iWi4r42+QoqsC
    F0dLlgTmsafNTwVP1ztoeGvvezXSUfKfMTjjaZDRB226gwW7+eZ0MbrVcEnI9wm5
    K9MOWut40KsoAHHGs5sLyAtIENwnPAkPQPwPxmEcUxJJZwI/Rq78zR1bAoGBAO9x
    fAi9M8VdbV3r/l1SnExRlTu2gp9Rv02Zy78HVOWEWdEn6mhG3to5mXsHNIQwLulQ
    jm/hBSme5g4xbSCL/qCqRkc1rRautq/W50G8h7S3+KFGdnzXdEYnEh4oh5t5PJtA
    LHWc2WhTe5bsBNwOR1xQ8bEm+V/lf4Fbq+jOcjZLAoGBALzKgDwPfApOf2512c/0
    bWeLYAlEC5PaXcZqJmlAjPOczsGG+Lg2EN1ET8fR2Gre1ctVwmZPqESxfFcbYS6i
    S0AAMajcteURhjVZmgWuU3E4DR3wsEurNDJm5QDEShKSQIZmRFtyepQPutNO3sBQ
    WloMEI5p+3AsMU7W7sQ5xbgxAoGABWwAbwI5xeJTs6jAXcSdHW1Lf8qmMo1bU5qD
    7pNv7LKOhhntSOcx7KcZPpvvKH8e0NGuKAJkZ4jdlLyxx+bjoSe556rjfHwATwMC
    wY5PVFxGGQDLdhA65cvEsUIhr/eS08EkQJWIpsAdMFGv2nvISeLbVjOXugAsXvWA
    cwkZtPkCgYBQJPRCGBz73lBFB16oYJR6AnC32nf0WbrnMLFwMejUrsTLN6M72axw
    bWFpW7rWV7ldSgxKJ6ucKWl78uUMUnkM+CGPt5WJiisZXM2X1Q8+V7fmdAtK/AFj
    /tbEFpftkCyIM1nGZwZ/ziPF4n5hzGGF6w/ZMWZkwFZlqxlejK9IfQ==
    -----END RSA PRIVATE KEY-----
"""
)

FAKE_BUILDER_PUBLIC_KEY = textwrap.dedent(
    """\
    -----BEGIN PUBLIC KEY-----
    MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4qEQSHM0QpWEhTvhWMBa
    hS7wbTIihaiRpUQC8+hEkmHhoJQyzaNR3CKdYWnJ1bAjdBT1HTHznbYSBasFAUKP
    iB16K/akuKSPnwHG9OM6+8Psw7ltGLP2jP65HE4a8n9lGas399xIK4hxZJkV2BXo
    cociXVEVB3nzzNk1AQZdJxik/ToLMYC2EKTu1kdt+OLl56/O1Mq9p8V7u2G1l8fq
    HtJi4Z34LzUzIoyFf7+bSmZBcHG1F/QdjbHb4temShDzptOM1VfXZchYTDVnbNsm
    R7TP2B857agog4rhqtVlPvHqFialWEU1WmAQz+oYqtRikUVWHq10SACxo6MFoM7L
    qQIDAQAB
    -----END PUBLIC KEY-----
"""
)

# inclusive-language: enable


# TODO(b/235240430): Improve unit test coverage.
class PathSigningTest(unittest.TestCase):
    """Tests the signing of bundles by path."""

    @classmethod
    def setUpClass(cls):
        cls.tempdir = tempfile.mkdtemp()

        cls.builder_key = Path(cls.tempdir) / 'fake_builder_key'
        cls.builder_key.write_text(FAKE_BUILDER_KEY)

        cls.builder_pub_key = Path(cls.tempdir) / 'fake_builder_pub_key.pem'
        cls.builder_pub_key.write_text(FAKE_BUILDER_PUBLIC_KEY)

        cls.bundle = Path(cls.tempdir) / 'fake_bundle'
        cls.bundle.write_bytes(b'FAKE BUNDLE CONTENTS\n')

    def test_bundle_blob_uploads(self):
        """Signing should upload the bundle, pub key, and signing request."""
        mock_blob = mock.create_autospec(Blob, instance=True)
        mock_blob.exists = mock.MagicMock(return_value=False)
        mock_in_bucket = mock.create_autospec(Bucket, instance=True)
        mock_in_bucket.blob = mock.MagicMock(return_value=mock_blob)
        mock_out_bucket = mock.create_autospec(Bucket, instance=True)
        mock_out_bucket.name = 'fake_out_bucket'
        client = remote_sign.RemoteSignClient(
            input_bucket=mock_in_bucket, output_bucket=mock_out_bucket
        )

        client.sign(
            self.bundle,
            signing_key_name='fake_key',
            builder_key=self.builder_key,
            builder_public_key=self.builder_pub_key,
            request_blob_name='signing_request.json',
            timeout_s=0.1,
        )

        mock_blob.upload_from_filename.assert_has_calls(
            [
                mock.call(os.path.join(self.tempdir, 'fake_bundle')),
                mock.call(
                    os.path.join(self.tempdir, 'fake_builder_pub_key.pem')
                ),
            ],
            any_order=True,
        )
        mock_blob.upload_from_string.assert_called_once()


if __name__ == '__main__':
    unittest.main()
