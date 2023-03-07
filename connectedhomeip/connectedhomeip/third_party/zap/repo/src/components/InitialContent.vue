<!--
Copyright (c) 2008,2020 Silicon Labs.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->

<template>
  <div>
<h3 id="zcl-advanced-platform">ZCL Advanced Platform</h3>
<p><img src="https://github.com/project-chip/zap/workflows/Generation%20and%20back-end%20tests/badge.svg" alt="Generation and back-end tests">
<img src="https://github.com/project-chip/zap/workflows/Cypress%20UI%20tests/badge.svg" alt="Cypress UI tests">
<img src="https://github.com/project-chip/zap/workflows/SonarCloud/badge.svg" alt="SonarCloud scan">
<img src="https://github.com/project-chip/zap/workflows/Build%20&amp;%20Release/badge.svg" alt="Build and release"></p>
<h4 id="what-is-zap-">What is ZAP?</h4>
<p>ZAP is a generic generation engine and user interface for applications and libraries based on <a href="https://zigbeealliance.org/developer_resources/zigbee-cluster-library/">Zigbee Cluster Library</a>, the specification developed by the <a href="https://zigbeealliance.org/">Zigbee Alliance</a>.</p>
<p>ZAP allows you to perform the following:</p>
<ul>
<li>perform SDK-specific customized generation of all global artifacts (constants, types, IDs, etc) based on the ZCL specification</li>
<li>provide UI for the end-user to select specific application configuration (clusters, attributes, commands, etc.)</li>
<li>perform SDK-specific customized generation of all user selected configuration artifacts (application configuration, endpoint configuration, etc) based on ZCL specification and customer-provided application configuration.</li>
</ul>
<p>ZAP is a generic templating engine. Examples are provided for how to generate artifacts for the C language environment, but one could easily add new templates for other language environments, such as C++, java, node.js, python or any other.</p>
<h2 id="quick-instructions">Quick instructions</h2>
<p>This is a node.js application. In order to run it, you need to have <a href="https://www.npmjs.com/">npm</a> installed. The best way is to simply download latest install of <a href="https://nodejs.org/en/download/">node</a> and you will get npm. If you have an older version of node installed on your workstation, it may give you trouble, particularly if it&#39;s very old. So make sure you have decently recent (v12.x or v14.x should work as of 2021) version of node available. Run <code>node --version</code> to check what version is picked up.</p>
<p>Once you have a desired version of node, you can run:</p>
<pre><code><span class="hljs-built_in">npm</span> ci
</code></pre><p>which will download install all the project dependencies. It is not uncommon to run into native library compilation problems at this point.
There are various <code>src-script/install-*</code> scripts for different platforms. Please refer to <a href="docs/faq.md">FAQ</a> for additional details of which script to run on different platforms and then rerun <code>npm ci</code>.</p>
<p>Then run:</p>
<pre><code>npm <span class="hljs-keyword">run</span><span class="bash"> zap</span>
</code></pre><p>Refer to <a href="docs/instructions.md">more detailed instructions</a> for more details.</p>
<h2 id="license">License</h2>
<p>This software is licensed under <a href="LICENSE.txt">Apache 2.0 license</a>.</p>
<h2 id="detailed-documentation">Detailed Documentation</h2>
<ul>
<li><a href="docs/design.md">Design</a></li>
<li><a href="docs/template-tutorial.md">Template tutorial</a></li>
<li><a href="docs/sdk-integration.md">SDK integration guideline</a></li>
<li><a href="docs/access.md">Access control features</a></li>
<li><a href="docs/custom-zcl.md">Custom ZCL entities design</a></li>
<li><a href="docs/instructions.md">Instructions</a></li>
<li><a href="docs/coding-standard.md">Coding standard</a></li>
<li><a href="docs/api.md">API</a></li>
<li><a href="docs/releasenotes.md">Release notes</a></li>
<li><a href="docs/faq.md">FAQ</a></li>
</ul>
</div>
</template>
<script>
//imports

export default {
  name: 'InitialContent',

  mounted() {},
  watch: {},
  computed: {},
  methods: {},
  components: {},
}
</script>

<!-- Notice lang="scss" -->
<style lang="scss">
.bar {
  padding: 15px 15px 15px 15px;
}
</style>
