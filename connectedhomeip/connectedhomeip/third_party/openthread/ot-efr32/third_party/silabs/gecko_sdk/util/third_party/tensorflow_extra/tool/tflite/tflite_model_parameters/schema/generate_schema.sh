#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if command -v flatc --version &> /dev/null
then
    FLATC=flatc
elif command -v $SCRIPT_DIR/flatc --version &> /dev/null
then
    FLATC=$SCRIPT_DIR/flatc
else
    echo "You must first install the flatbuffer compiler executable."
    echo "You can either install it into the environment PATH"
    echo "OR you may download the executable from:"
    echo "https://github.com/google/flatbuffers/releases/tag/v2.0.0"
    echo "and extract to the directory: $SCRIPT_DIR"
    exit 
fi 

$FLATC --python -o $SCRIPT_DIR $SCRIPT_DIR/dictionary.fbs