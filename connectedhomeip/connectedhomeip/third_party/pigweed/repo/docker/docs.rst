------
docker
------
``pw_env_setup`` reliably initializes a working environment for Pigweed, but
can take awhile to run. It intelligently caches where it can, but that caching
is designed around a particular execution environment. That environment
assumption is poor when running tests with docker. To help out teams using
docker for automated testing, the Pigweed team has a publicly-available docker
image with a cache of some of the Pigweed environment. The current tag of this
image is in ``docker/tag`` under the Pigweed checkout.

To build your own docker image, start with ``docker/Dockerfile.cache`` and
run ``docker build --file docker/Dockerfile.cache .`` from the root of your
Pigweed checkout.

To use the publicly-available docker image, run
``docker build --file docker/Dockerfile.run --build-arg from=$(cat docker/tag) .``
from the root of your Pigweed checkout. You still need to run
``. ./bootstrap.sh`` within the docker image, but it should complete much
faster than on a vanilla docker image.

We're still trying to improve this process, so if you have any ideas for
improvements please `send us a note`_.

.. _send us a note: pigweed@googlegroups.com
