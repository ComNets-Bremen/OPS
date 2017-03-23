#!/bin/bash

source ./tools/shell-functions.sh
loadSettings

if [ "$INET_BUILD" = true ]; then
    echo "Getting the latest version of OMNeT++ INET Framework..."
    if [ ! -d "modules" ]; then
        mkdir modules
    fi    
    cd modules
    git submodule add https://github.com/inet-framework/inet.git
    cd inet
    git submodule init
    git submodule update
    echo "Building the OMNeT++ INET Framework..."
    make makefiles
    make MODE=release
else
    echo "Skipping OMNeT++ INET Framework"
fi
