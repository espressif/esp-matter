![ZCL Advanced Platform](src-electron/icons/zap_128x128.png)

# ZCL Advanced Platform

![Generation and back-end tests](https://github.com/project-chip/zap/workflows/Generation%20and%20back-end%20tests/badge.svg)
![Cypress UI tests](https://github.com/project-chip/zap/workflows/Cypress%20UI%20tests/badge.svg)
![SonarCloud scan](https://github.com/project-chip/zap/workflows/SonarCloud/badge.svg)
![Build and release](https://github.com/project-chip/zap/workflows/Build%20&%20Release/badge.svg)

## What is ZAP?

ZAP is a generic generation engine and user interface for applications and libraries based on [Zigbee Cluster Library](https://zigbeealliance.org/developer_resources/zigbee-cluster-library/), the specification developed by the [Zigbee Alliance](https://zigbeealliance.org/).

ZAP allows you to perform the following:

- perform SDK-specific customized generation of all global artifacts (constants, types, IDs, etc) based on the ZCL specification
- provide UI for the end-user to select specific application configuration (clusters, attributes, commands, etc.)
- perform SDK-specific customized generation of all user selected configuration artifacts (application configuration, endpoint configuration, etc) based on ZCL specification and customer-provided application configuration.

ZAP is a generic templating engine. Examples are provided for how to generate artifacts for the C language environment, but one could easily add new templates for other language environments, such as C++, java, node.js, python or any other.

## Quick instructions

This is a node.js application. In order to run it, you need to have [npm](https://www.npmjs.com/) installed. The best way is to simply download latest install of [node](https://nodejs.org/en/download/) and you will get npm. If you have an older version of node installed on your workstation, it may give you trouble, particularly if it's very old. So make sure you have decently recent (v12.x or v14.x should work as of 2021) version of node available. Run `node --version` to check what version is picked up.

Once you have a desired version of node, you can run:

```
npm ci
```

which will download install all the project dependencies. It is not uncommon to run into native library compilation problems at this point.
There are various `src-script/install-*` scripts for different platforms. Please refer to [FAQ](docs/faq.md) for additional details of which script to run on different platforms and then rerun `npm ci`.

Then run:

```
npm run zap
```

Refer to [more detailed instructions](docs/instructions.md) for more details.

## License

This software is licensed under [Apache 2.0 license](LICENSE.txt).

## Detailed Documentation

- [Design](docs/design.md)
- [Template tutorial](docs/template-tutorial.md)
- [SDK integration guideline](docs/sdk-integration.md)
- [Access control features](docs/access.md)
- [Custom ZCL entities design](docs/custom-zcl.md)
- [Instructions](docs/instructions.md)
- [Coding standard](docs/coding-standard.md)
- [API](docs/api.md)
- [Release notes](docs/releasenotes.md)
- [FAQ](docs/faq.md)
