#!/bin/bash
#
# Bash script to run a developed ops model either as
# only command line (cmdenv) or with GUI (tkenv).
#
# @author: Asanga Udugama (adu@comnets.uni-bremen.de)
# @author: Jens Dede (jd@comnets.uni-bremen.de)
# @date: 09-January-2018
#
source ./tools/shell-functions.sh

loadSettings

if [[ "$OSTYPE" == "darwin"* ]]; then
    export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$KEETCHI_API_LIB:$INET_LIB
elif [[ "$OSTYPE" == "linux"* ]]; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$KEETCHI_API_LIB:$INET_LIB
fi

# Check if the simulation executable exists
if [[ -x ./$OPS_MODEL_NAME ]]; then
    echo "Found simulation \"./$OPS_MODEL_NAME\""
else
    echo "Cannot find the simulation executable \"./$OPS_MODEL_NAME\". Have you run \"make\"?"
    exit 1
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

# Is a default output directory defined in the settings? Use this one.
# Otherwise create a generic one
if [ -z ${SIM_OUTPUT_DIR+x} ]; then
    SIM_OUTPUT_DIR=$(basename $OMNET_INI_FILE)
    SIM_OUTPUT_DIR="$OMNET_OUTPUT_DIR$(date +"%Y-%m-%d_%H-%M-%S")_${SIM_OUTPUT_DIR%.*}"
fi

echo "Storing Simulation in directory \"simulations/$SIM_OUTPUT_DIR\""

case "$modeval" in
    cmdenv)
        SIMTYPE="Cmdenv"
        ;;

    tkenv)
        SIMTYPE="Tkenv"
        ;;

    *)
        echo "Mode not specified" >&2
        echo "Usage:"
        echo " $0 -m cmdenv|tkenv" >&2
        exit 1
        ;;
esac

mkdir -p simulations/$SIM_OUTPUT_DIR

# Keep the ini file in the output directory
cp $OMNET_INI_FILE simulations/$SIM_OUTPUT_DIR

echo "./$OPS_MODEL_NAME -u $SIMTYPE -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l keetchi -l INET --result-dir=$SIM_OUTPUT_DIR" | tee simulations/$SIM_OUTPUT_DIR/sim_command.txt
./$OPS_MODEL_NAME -u $SIMTYPE -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l keetchi -l INET --result-dir=$SIM_OUTPUT_DIR


