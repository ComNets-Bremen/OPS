#!/bin/bash

source ./tools/shell-functions.sh
loadSettings


if [ ! -d "modules" ]; then
    mkdir modules
fi
cd modules

echo "Getting latest version of the KeetchiLib"
git submodule init
git submodule update
git submodule add https://github.com/ComNets-Bremen/KeetchiLib.git
echo "Building the KeetchiLib"
cd KeetchiLib
./bootstrap.sh
./configure
make
cd ..

if [ "$INET_BUILD" = true ]; then
    echo "Getting the latest version of OMNeT++ INET Framework..."

    git submodule add https://github.com/inet-framework/inet.git
    cd inet
    git submodule init
    git checkout tags/v3.4.0
    git submodule update
    echo "Building the OMNeT++ INET Framework..."
    if [ ! -f “Makefile” ]; then
        make clean
    fi
    make makefiles
    make MODE=release
else
    echo "Skipping OMNeT++ INET Framework"
fi

