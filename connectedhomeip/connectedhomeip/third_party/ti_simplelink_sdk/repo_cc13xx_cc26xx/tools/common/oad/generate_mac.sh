#!/bin/bash
# Assuming Pyinstaller is installed via pip
# See more here: http://www.pyinstaller.org/

@echo off

echo "Cleaning up build folders"
rm -rf bin/macosx
rm -rf pyinstaller_build

echo "Generate temp build folders"
mkdir pyinstaller_build

echo "Generating binary"
cd pyinstaller_build
pyinstaller --onefile ../oad_image_tool.py

mkdir ../bin/macosx
mv dist/oad_image_tool ../bin/macosx/oad_image_tool

cd ../
rm -rf  pyinstaller_build
