# Coding Standards

---

**Q: How do I deal with `undefined` or `null`?**

**A:** If you deal with a return value that might be null or undefined,
you should be checking it as:

```
   if ( value == null ) {
       ...
   }
```

This will evaluate to true if value is either `undefined` or `null`, and will give you a correct result in most cases.

If you are returning a value that is undefined, you should use `undefined`, not `null`.

---

**Q: How do I deal with asynchronicity?**

**A:** Use promises wherever you can. Any function that can, should return a promise, and NOT do anything sychronous. Think twice before you create a synchronous function, and then don't do it afterwards.
Asynchronicity is a defining feature of the node.js platform, and the one that contributes the most to it's high performance. So please make the best use of it.

---

**Q: How do I isolate the DB schema from the rest of the code?**

**A:** All the database schema should be isolated to `src-electron/db` directory. There should be NOTHING that depends on the schema outside of it. That includes:

- Any explicit queries in form of a string.
- Any dependency on specific names of the columns
- Any dependency on specific names of the tables
- Any dependency on number of columns or type of columns
- etc.

This means that when you are doing `SELECT` queries, you should not just pass the returned `rows` to the upper layer, but should map them.

---

**Q: How do I name things?**

**A:** You name things by observing the following rules:

- Files and directories follow kebab-case: meaning lower case, separate with a `-`, as in:

```
   src-electron/some-directory/this-is-a-file.js
```

- Functions follow camel-case: meaning you start with a lower case letter and then uppercase each word, as in:

```
   thisIsAFunction()
   anotherFunction()
```

- Acronyms are NOT uppercased, but treated as a regular words. For example, you will NOT use `convertZCLData()`, instead, you will use `convertZclData()`. Even though ZCL is an acronym, we treat it as a normal word in the code.

---

**Q: Whitespace?**

**A:** VSCode default JS formatter is generally doing a good job, so follow common sense.

To indent your code and for whitespace separation, you are very strongly advised to use spaces.

Using tabs is your choice as a free person, but for each tab found in the code under your name, you will be shipped to a small village 200 miles west of Norilsk, for a month-long reeducation and a stint of hard labor. Siberia is nice this time of year...

---

**Q: Imports?**
**A:** In back-end node.js code, when importing a module, one should import them using the pattern:

```
const lowerCaseCamelName = require('relative/path/file.js')
```

When using functions from these imported modules, one should use the pattern

```
lowerCaseCamelName.importedFunction()
```

If you are importing just a single symbol from a module, you can also do:

```
const { symbol } = require('relative/path/file.js')
```

but it's preferable to use the previous syntax, with loading entire module into its own namespace.

You can not use ES6 import/export syntax on the node.js backend.

In front-end java code, using ES6 is ok, as quasar transpiles all that code to a browser standard JS anyway.
