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
"""A Sphinx extension to add a Google Analytics tag to generated docs"""


def add_google_analytics_tag(
    app, pagename, templatename, context, doctree
):  # pylint: disable=unused-argument
    if app.config.google_analytics_id is None:
        return

    if 'metatags' not in context:
        context['metatags'] = ''

    # pylint: disable=line-too-long
    context[
        'metatags'
    ] += f"""<script async src="https://www.googletagmanager.com/gtag/js?id={app.config.google_analytics_id}"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){{dataLayer.push(arguments);}}
  gtag('js', new Date());

  gtag('config', '{app.config.google_analytics_id}');
</script>"""


def setup(app):
    app.add_config_value('google_analytics_id', None, 'html')
    app.connect('html-page-context', add_google_analytics_tag)
    return {'parallel_read_safe': True}
