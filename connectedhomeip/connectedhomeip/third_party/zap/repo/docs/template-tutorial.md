# Template tutorial

This document introduces the templating language and helps you develop your own templates.

## Introduction

Zap is using [handlebars](https://handlebarsjs.com/) package for templating, so any documentation about it applies to the zap templates.

Apart from stock handlebars functionality, zap provides a series of [handlebar helpers](https://handlebarsjs.com/guide/block-helpers.html) which make creation of ZCL specific templates easier.

Templates are typically named `*.zapt`, but that's a convention only, the files can be named anything. They should be listed in a `gen-template.json` file, as described in [SDK integration document](sdk-integration.md).

This document does not deal with the `gen-template.json` file itself, but only with the structure and format of individual `*.zapt` templates.

## Simple example

```
{{zap_header}}

#ifndef __ZAP_GEN_ID__
{{#zcl_clusters}}


// Definitions for cluster: {{label}}
#define ZCL_{{asMacro label}}_CLUSTER_ID ({{asHex code}})

// Client attributes for cluster: {{label}}
{{#zcl_attributes_client}}
#define ZCL_{{asMacro label}}_ATTRIBUTE_ID ({{asHex code}})
{{/zcl_attributes_client}}

// Server attributes for cluster: {{label}}
{{#zcl_attributes_server}}
#define ZCL_{{asMacro label}}_ATTRIBUTE_ID ({{asHex code}})
{{/zcl_attributes_server}}

// Commands for cluster: {{label}}
{{#zcl_commands}}
#define ZCL_{{asMacro label}}_COMMAND_ID ({{asHex code}})
{{/zcl_commands}}

// End of cluster: {{label}}
{{/zcl_clusters}}

#endif // __ZAP_GEN_ID__

```

Looking at this example, we see two types of content:

- templating tags, which are all the tags inside `{{ ... }}` symbols. This content is the handlebars template content and has a special meaning.
- other content, which is text outside of the `{{ ... }}` tags, which is just content that gets passed through as-is.

There are 2 types of templating tags:

- simple: `{{id}}` or `{{id arg}}` tags. These expand into the value of `id` within the context of the template. If `id` is a helper function, then it will executed with subsequent arguments passed to it, and the result will be substituted into the template.
- block helpers: `{{#tag}} ... {{/tag}}`. These are so called block helpers, which allows for nesting of templates into a different context. They are in case of zap commonly used for iterating over the zcl content, such as `{{#zcl_clusters}} ... {{/zcl_clusters}}` for example. Use of these tags in the above example, iterates over all the defined clusters in the database, and the content between the opening and closing tag is replaced once for each cluster. The content inside the tag is executed with a different context each time, and in case of `zcl_clusters`, the context is the cluster itself.

## Iterators

All of the iterators in ZAP (for example `{{#zcl_clusters}} ... {{/zcl_clusters}}` and similar) support the following:

- inner `{{#first}} ... {{/first}}` block: the content inside this block is only output during the FIRST iteration of the block.
- inner `{{#last}} ... {{/last}}` block: the content inside this block is only output during the LAST iteration of the block.
- inner `{{#middle}} ... {{/middle}}` block: the content inside this block is only output if this is neither the first nor the last element.
- use of `{{else}}` which makes the following block appear only if the iterator is empty.

For example:

```
{{#zcl_clusters}}
{{#first}}
This content is output only once, at the beginning, for the first cluster.
{{/first}}
This content is output for EVERY cluster.
{{#middle}}
This content is output for all clusters but the first and the last.
{{/middle}}
{{#last}}
This content is output only once, at the end, for the last cluster.
{{/last}}
{{else}}
This content is output ONLY if there are no clusters at all.
{{/zcl_clusters}}

```

## Global context

Whichever helpers get executed they all contain `global` inside their context that will always contain:

- `db`: connection to the database being used
- `sessionId`: session id for the user data being used
- `genTemplatePackageId`: toplevel package id for gen-template.json file, used for querying options and other
- `zclPackageId`: populated after you call `templateUtil.ensureZclPackageIds()` the first time, used for querying the ZCL static data.

Whenever a context switches, the switch will preserve this `global` object inside the context for helpers to access.

## Helper API

Helper API is listed in the generated JSDoc documentation under "Template API modules". There are many helpers that can be used for all kind of different cases of ZCL generation.
