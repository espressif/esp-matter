# Copyright 2022 The Pigweed Authors
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
"""Console HTTP Log Server functions."""

import logging
from pathlib import Path
import mimetypes
import http.server
from typing import Dict, Callable

_LOG = logging.getLogger(__package__)


def _start_serving(port: int, handler: Callable) -> bool:
    try:
        with http.server.HTTPServer(('', port), handler) as httpd:
            _LOG.debug('Serving on port %i', port)
            httpd.serve_forever()
        return True
    except OSError:
        _LOG.debug('Port %i failed.', port)
        return False


def pw_console_http_server(starting_port: int, handler: Callable) -> None:
    for i in range(100):
        if _start_serving(starting_port + i, handler):
            break


class ConsoleLogHTTPRequestHandler(http.server.BaseHTTPRequestHandler):
    """Request handler that serves files from pw_console.html package data."""

    def __init__(self, html_files: Dict[str, str], *args, **kwargs):
        self.html_files = html_files
        super().__init__(*args, **kwargs)

    def do_GET(self):  # pylint: disable=invalid-name
        _LOG.debug(
            '%s: %s',
            self.client_address[0],
            self.raw_requestline.decode('utf-8').strip(),
        )

        path = self.path
        if path == '/':
            path = '/index.html'

        if path not in self.html_files:
            self.send_error(http.server.HTTPStatus.NOT_FOUND, 'File not found')
            return

        content: str = self.html_files[path].encode('utf-8')
        content_type = 'application/octet-stream'
        mime_guess, _ = mimetypes.guess_type(Path(path).name)
        if mime_guess:
            content_type = mime_guess

        self.send_response(http.server.HTTPStatus.OK)
        self.send_header('Content-type', content_type)
        self.send_header('Content-Length', str(len(content)))
        self.send_header('Last-Modified', self.date_time_string())
        self.end_headers()
        self.wfile.write(content)

    def log_message(self, *args, **kwargs):
        pass
