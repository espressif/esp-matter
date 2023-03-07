# Instructions

This section lists instructions for various things you might need to do in this repo.

**Install the dependencies:**

```bash
npm install
```

**Start the application:**

```bash
npm run zap
```

**Start the front-end in development mode:**

(Supports hot-code reloading, error reporting, etc.)

```bash
quasar dev -m electron
```

or

```
npm run electron-dev
```

**Environment variables:**

Following is the list of environment variables that zap tool honors:

- `ZAP_LOGLEVEL`: pino log level to start with. Default is 'warn'. You can use 'debug' or 'info', for example.

**Format the files:**

Format staged files in Git to follow [prettier.io](https://prettier.io/) code format.

[For development, IDE integration can be found here.](https://prettier.io/docs/en/editors.html)

This command is called via pre-commit Git hook as well.

```bash
npm run format-code
```

**Lint the files:**

```bash
npm run lint
```

**Build the app for production:**

```bash
quasar build -m electron
```

or

```
npm run electron-build
```

**Run the unit tests:**

```bash
npm run test
```

**Regenerate the API documentation:**

```bash
npm run apidoc
```

**Customize the configuration:**

See [Configuring quasar.conf.js](https://quasar.dev/quasar-cli/quasar-conf-js).
