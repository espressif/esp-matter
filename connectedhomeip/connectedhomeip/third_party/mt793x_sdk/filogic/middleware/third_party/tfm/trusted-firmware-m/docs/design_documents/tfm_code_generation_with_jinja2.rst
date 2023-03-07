###########################
Code Generation With Jinja2
###########################

:Author: Mate Toth-Pal
:Organization: Arm Limited
:Contact: Mate Toth-Pal <mate.toth-pal@arm.com>
:Status: Accepted

***************************************
Generating files from templates in TF-M
***************************************

Some of the files in TF-M are generated from template files. The files to be
generated are listed in ``tools/tfm_generated_file_list.yaml``. For each
generated file ``<path_to_file>/<filename>`` the template file is
``<path_to_file>/<filename>.template``. The templates are populated with
partition information from the partition manifests. The manifests to be used for
the generation are listed in ``tools/tfm_manifest_list.yaml``.

****************************************
Custom generator script - Current method
****************************************

``tools/tfm_parse_manifest_list.py`` Python script is used to generate files
from the templates. This script calls the ``tools/generate_from_template.py`` to
parse the template files, and uses ``tools/keyword_substitution.py`` to
substitute the keychains with actual values from the manifest files.

*************************************
Use Jinja2 template engine - proposal
*************************************

The proposal is to eliminate the template parser and substituter scripts, and
call the Jinja2 template engine library from
``tools/tfm_parse_manifest_list.py`` to do the substitution.

More information on jinja2: http://jinja.pocoo.org/

Changes needed:
===============

- ``tools/tfm_parse_manifest_list.py`` have to be modified to call the Jinja2
  library instead of the custom scripts. The data structure required by the
  library is very similar to the one required by the current scripts.
- template files needs to be rewritten to the Jinja syntax: The control
  characters to be replaced (like ``@!`` -> ``{%``) and ``for`` statements to be
  added to iterate over the substitutions.

Improvements over the current solution
======================================

- Compatible with Python 2.7 and Python 3, while current solution only supports
  Python 2.7
- More advanced functionality: direct control over the list of items for a
  keychain, meta information on that list (like length)
- Well documented (see website)
- Jinja2 is free and open-source software, BSD licensed, just like TF-M. It is a
  well established software in contrast with the current proprietary solution.

*******
Example
*******

Below code snippet enumerates the secure function IDs:

.. code-block::

    {% for manifest in manifests %}
        {% if manifest.attr.conditional %}
    #ifdef {{manifest.attr.conditional}}
        {% endif %}
        /******** {{manifest.manifest.tfm_partition_name}} ********/
        {% for sec_func in manifest.manifest.secure_functions %}
        {{'{'}}{{sec_func.tfm_symbol}}, {{sec_func.sfid}}{{'}'}},
        {% endfor %}
        {% if manifest.attr.conditional %}
    #endif /* {{manifest.attr.conditional}} */
        {% endif %}

    {% endfor %}

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*
