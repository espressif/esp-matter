# CI License Checks

License Check in CI is mean to verify are files inside this repository comply with our License Policy


### What License Policy in this repository

By default, this software is licensed under the Apache v2.0 license. However, this software may reuse software already licensed under another license, provided the license is permissive in nature and compatible with Apache v2.0. Documentation shipped alongside the code shall be licensed under the same license as the code.


### What does CI License check do

CI License check runs when a Merge Request is created with proposed code changes. The CI test assumes all existing files inside the target branch have already complied with the license policy. And the tests only check licenses against newly added/modified files in each MR.


### How it works

CI License checks utilise `scancode` and `jsonschema` to check the files' licenses within this repository.

The basic CI flow is :
- When a Merge Request is created, find all added/modified files and copy them to a folder named MR-CHANGED-FILES
- `scancode` scans folder MR-CHANGED-FILES, and generates a JSON format reports
- `jsonschema` verify the output JSON from `scancode` against a pre-defined `license.schema`
- If the MR changed the pre-defined `license.schema`  as well, a CI job will run `test_schema.sh` to test the integrity of the  schema file


### scancode

`scancode` scans the files and generates JSON output for reporting what is the license contained in each source file. The output JSON file contains a `headers` section including information about the tool and a `files` section including all the license scan results on each file/folder

Here is an example of `scancode` output JSON file


```json
{
  "headers": [
    {
      "tool_name": "scancode-toolkit",
      "tool_version": "21.8.4",
      ...
    }
  ],
  "files": [
    {
      "path": "MR-CHANGED-FILES/.gitlab",
      "type": "directory",
      "licenses": []
      ...
    },
    {
      "path": "MR-CHANGED-FILES/.gitlab-ci.yml",
      "type": "file",
      "licenses": [
        {
          "key": "apache-2.0",
          "name": "Apache License 2.0",
          "category": "Permissive",
          "owner": "Apache Software Foundation",
          "spdx_license_key": "Apache-2.0",
          "spdx_url": "https://spdx.org/licenses/Apache-2.0",
          "start_line": 2,
          "end_line": 2,
          ...
        }
      ]
    },
    {
      "path": "MR-CHANGED-FILES/.gitlab/ci/license/license.schema",
      "type": "file",
      "licenses": []
      ...
    }
  ]
}
```

### jsonschema

`jsonschema` verifies whether the `scancode` output JSON file comply with the pre-defined `license.schema`.

Currently, license schema requires each file/folder object to match with at least one of below cases:
- If the object is folders, license will not be required
- If the object is binary format file objects (e.g. *.jpg *.png), license will not be required
- If the object is certain text-based file objects (e.g. *html *.json *.md), license will not be required
- If the object is certain special files (e.g. gitmodules CODEOWNERS), license will not be required
- Rest files will be required for a Permissive license

For details about how to define jsonschema please go to [json-schema.org](https://json-schema.org/)
