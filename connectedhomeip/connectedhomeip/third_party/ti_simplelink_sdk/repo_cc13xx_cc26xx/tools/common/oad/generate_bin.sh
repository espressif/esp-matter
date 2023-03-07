#!/bin/bash
# Assuming Pyinstaller is installed via pip
# See more here: http://www.pyinstaller.org/

@echo off

echo "Cleaning up build folders"
rm -rf bin/linux
rm -rf pyinstaller_build

echo "Generate temp build folders"
mkdir pyinstaller_build

echo "Generating binary"
cd pyinstaller_build
pyinstaller --onefile ../oad_image_tool.py

mkdir ../bin/linux
mv dist/oad_image_tool ../bin/linux/oad_image_tool

cd ../

# package dependencies
echo "Packaging in dependencies using StaticX"
staticx ./bin/linux/oad_image_tool ./bin/linux/oad_image_tool

rm -rf  pyinstaller_build
