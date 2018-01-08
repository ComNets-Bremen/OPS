#!/bin/bash
#
# Bash script to run a developed ops model either as
# only command line (cmdenv) or with GUI (tkenv).
#
# @author: Asanga Udugama (adu@comnets.uni-bremen.de)
# @date: 18-may-2016
#
source ./tools/shell-functions.sh

loadSettings

if [[ "$OSTYPE" == "darwin"* ]]; then
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$KEETCHI_API_LIB:$INET_LIB
elif [[ "$OSTYPE" == "linux"* ]]; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$KEETCHI_API_LIB:$INET_LIB
fi


modeval=""

while [ "$#" -gt 0 ]; do
    case "$1" in
        -m)
            modeval="$2"
            shift 2
            ;;
        -h)
            echo "Usage:"
            echo " $0 -m cmdenv|tkenv" >&2
            exit 0
            ;;

        --mode=*)
            modeval="${1#*=}"
            shift 1
            ;;

        --mode)
            echo "$1 requires an argument" >&2
            echo "Usage:"
            echo " $0 -m cmdenv|tkenv" >&2
            exit 1
            ;;

        *)
            echo "Unknown option: $1" >&2
            echo "Usage:"
            echo " $0 -m cmdenv|tkenv" >&2
            exit 1
            ;;
    esac
done

mkdir -p simulations/$OMNET_OUTPUT_DIR

case "$modeval" in
    cmdenv)
        echo "./$OPS_MODEL_NAME -u Cmdenv -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l INET --result-dir=$OMNET_OUTPUT_DIR"
        ./$OPS_MODEL_NAME -u Cmdenv -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l keetchi -l INET --result-dir=$OMNET_OUTPUT_DIR
        ;;

    tkenv)
        echo "./$OPS_MODEL_NAME -u Tkenv -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l INET --result-dir=$OMNET_OUTPUT_DIR"
        ./$OPS_MODEL_NAME -u Tkenv -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l keetchi -l INET --result-dir=$OMNET_OUTPUT_DIR
        ;;

    *)
        echo "Mode not specified" >&2
        echo "Usage:"
        echo " $0 -m cmdenv|tkenv" >&2
        exit 1
        ;;
esac


