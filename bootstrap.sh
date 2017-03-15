#!/bin/bash

source ./tools/shell-functions.sh
loadSettings

if [ "$INET_BUILD" = true ]; then
    echo "Building inet"
    cd ../inet
    make makefiles
    make MODE=release
else
    echo "Skipping INET"
fi
