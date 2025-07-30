# Documentation Source Folder

This folder contains source files for the documentation of Espressif's SDK for Matter.

The sources do not render well in GitHub and some information is not visible at all.

Use actual documentation generated within about 20 minutes on each commit:

# Hosted Documentation

* English: https://docs.espressif.com/projects/esp-matter/en/latest/

The above URL is for the main branch latest version. Click the drop-down in the bottom left to choose a particular version or to download a PDF.

# Building Documentation

This project uses [esp-docs](https://docs.espressif.com/projects/esp-docs/en/latest/index.html) to build the documentation.

- Install dependencies
```
pip install -r requirements.txt
```

- Build documentation
```
build-docs -bs html latex -t esp32 -l en
```
NOTE: Above command only builds for the `esp32` target, you can add more targets to `-t` argument.

- For more available options run:
```
build-docs --help
```
