#!/bin/bash
#
# Bootstrap script for OPS -- The Opportunistic Protocol Simulator
#
# Setup everything for the OPS simulations. Refer to README.md for detailed
# information and how to use this file.
#
# @author Jens Dede (jd@comnets.uni-bremen.de)
# @date 28-February-2018
#

# Load the settings and convenience functions
source ./tools/shell-functions.sh
loadSettings

# Ensure $INET_PATH is absolute
if [[ "$INET_PATH" != /* ]]; then
    echo "\$INET_PATH is relative. Change it to an absolute one"
    INET_PATH=$(pwd)/$INET_PATH
fi

echo "Using INET: $INET_PATH"

if [ ! -d "modules" ]; then
    mkdir modules
fi

echo "Initialize the submodules"
git submodule update --init --recursive

# Now, all directories should be there and we can finally check the $INET_PATH
if [ ! -d $INET_PATH ]; then
    echo "Invalid INET path: \"$INET_PATH\". Please check your configuration. Aborting..."
    exit 1
fi

cd modules
if [ "$INET_BUILD" = true ]; then
    echo "Building version $INET_VERSION of the OMNeT++ INET Framework..."
    cd inet

    # The inet version is taken from the settings
    git checkout $INET_VERSION
    git submodule update --init --recursive

    # build SWIM and ExtendedSWIM modules with INET
    if [ "$INET_PATCH" = true ]; then
        if [ -d "../SWIMMobility" ]; then
            echo "Checking for SWIM in INET located in $INET_PATH and patch it if necessary"
            for f in $(ls ../SWIMMobility/*.{cc,h,ned}); do
                basefile=$(basename $f)
                echo "Checking for $basefile..."
                if [ ! -f "$INET_PATH/inet/mobility/single/$basefile" ]; then
                    echo "\"$basefile\" does not exist. Copying..."
                    cp $f $INET_PATH/inet/mobility/single/
                else
                    echo "Found \"$basefile\" in the INET directory \"$INET_PATH/inet/mobility/single/"
                    echo "The existing version is not replaced. Ensure that you update it if required!"
                fi
            done
        else
            echo "SWIMMobility not found! Skipping"
        fi

        if [ -d "../ExtendedSWIMMobility" ]; then
            echo "Checking for ExtendedSWIM in INET located in $INET_PATH and patch it if necessary"
            for f in $(ls ../ExtendedSWIMMobility/IReactive*.{h,ned}); do
                basefile=$(basename $f)
                echo "Checking for $basefile..."
                if [ ! -f "$INET_PATH/inet/mobility/contract/$basefile" ]; then
                    echo "\"$basefile\" does not exist. Copying..."
                    cp $f $INET_PATH/inet/mobility/contract/
                else
                    echo "Found \"$basefile\" in the INET directory \"$INET_PATH/inet/mobility/contract/"
                    echo "The existing version is not replaced. Ensure that you update it if required!"
                fi
            done
            for f in $(ls ../ExtendedSWIMMobility/ExtendedSWIM*.{cc,h,ned}); do
                basefile=$(basename $f)
                echo "Checking for $basefile..."
                if [ ! -f "$INET_PATH/inet/mobility/single/$basefile" ]; then
                    echo "\"$basefile\" does not exist. Copying..."
                    cp $f $INET_PATH/inet/mobility/single/
                else
                    echo "Found \"$basefile\" in the INET directory \"$INET_PATH/inet/mobility/single/"
                    echo "The existing version is not replaced. Ensure that you update it if required!"
                fi
            done
        else
            echo "ExtendedSWIMMobility not found. Skipping"
        fi
    else
        echo "INET patching disabled"
    fi

    # Is there an old version? -> clean up inet
    if [ ! -f “Makefile” ]; then
        make clean
    fi

    make makefiles

    if [ $? -ne 0 ]; then
        echo "Command \"make makefiles\" for INET failed. Aborting"
        exit 1
    fi

    make MODE=release

    if [ $? -ne 0 ]; then
        echo "Command \"make MODE=release\" for INET failed. Aborting"
        exit 1
    fi


    cd ..
else
    echo "Skipping building OMNeT++ INET Framework"
fi


echo "Building the KeetchiLib"
cd KeetchiLib
./bootstrap.sh
./configure
make
cd ..

echo "Done. Next step: run \"make\" to build the simulation"
