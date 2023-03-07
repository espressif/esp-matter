## Contributing

## License details

Please use an [SPDX license identifier](http://spdx.org/licenses/) in every
source file following the
[recommendations](https://spdx.dev/spdx-specification-21-web-version/#h.twlc0ztnng3b)
to make it easier for users to understand and review licenses.

The full original [Apache 2.0 license
text](http://www.apache.org/licenses/LICENSE-2.0) is in
[LICENSE-apache-2.0.txt](LICENSE-apache-2.0.txt). Each source file should start
with your copyright line followed by the SPDX identifier as shown here:

```
/* Copyright (c) 2013-2021, Arm  Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
```

## Developer’s Certificate of Origin

Each commit must have at least one `Signed-off-by:` line from the committer to
certify that the contribution is made under the terms of the [Developer
Certificate of Origin](./DCO.txt).

If you set your `user.name` and `user.email` as part of your git configuration,
you can add a signoff to your commit automatically with `git commit -s`. You
have to use your real name (i.e., pseudonyms or anonymous contributions cannot
be made). This is because the DCO is a legally binding document.

## Maintainers and code owners

All contributions are ultimately merged by the maintainers listed below.
Technical ownership of most parts of the codebase falls on the code owners
listed in [CODEOWNERS](CODEOWNERS) file. An acknowledgement from these code
owners is required before the maintainers merge a contribution.

## Pre-commit

After you clone this project locally, we recommend installing [`pre-commit`](https://pre-commit.com/) and running

```
$ pre-commit install
```

This adds a pre-commit hook `.git/hooks/pre-commit` which gets run every time you create a git commit. It makes changes to your commit as needed such as formatting code and license headers.

Maintainers
===========

Martin Kojtal
    :email: `Martin Kojtal <Martin.Kojtal@arm.com>`
    :gitlab: `https://gitlab.arm.com/Martin.Kojtal`

Jaeden Amero
    :email: `Jaeden Amero <Jaeden.Amero@arm.com>`
    :gitlab: `https://gitlab.arm.com/Jaeden.Amero`
