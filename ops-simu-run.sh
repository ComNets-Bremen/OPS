#!/bin/bash
#
# Bash script to run a developed ops model either as
# only command line (cmdenv) or with GUI (tkenv).
#
# @author: Asanga Udugama (adu@comnets.uni-bremen.de)
# @author: Jens Dede (jd@comnets.uni-bremen.de)
# @date: 15-January-2018
#
source ./res/tools/shell-functions.sh

loadSettings

# Show help message
showUsage(){
    echo "Usage:"
    echo " $0 [PARAMS]"
    echo ""
    echo "  -m cmdenv|qtenv : Mandatory, set the simulation type (command line vs. GUI)"
    echo "  -c <ini file>   : Optional, set the simulation ini file"
    echo "  -o <output dir> : Optional, set the simulation output directory"
    echo "  -b <backup dir> : Optional, backup results to this directory (beta)"
}

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

while [ "$#" -gt 0 ]; do
    case "$1" in
        -m)
            if [ -z ${2+x} ]; then
                echo "Missing parameter for \"$1\""
                showUsage
                exit 1
            fi

            modeval="$2"
            shift 2
            ;;
        -h)
            showUsage
            exit 0
            ;;

        -c)
            if [ -z ${2+x} ]; then
                echo "Missing parameter for \"$1\""
                showUsage
                exit 1
            fi
            OMNET_INI_FILE="$2"

            shift 2
            ;;

        -o)
            if [ -z ${2+x} ]; then
                echo "Missing parameter for \"$1\""
                showUsage
                exit 1
            fi
            SIM_OUTPUT_DIR="$2"

            shift 2
            ;;

        -b)
            if [ -z ${2+x} ]; then
                echo "Missing parameter for \"$1\""
                showUsage
                exit 1
            fi
            BACKUP_DIR="$2"

            shift 2
            ;;

        *)
            echo "Unknown option: $1" >&2
            showUsage
            exit 1
            ;;
    esac
done

# Check if the ini file exists and is accessible

if [ ! -f $OMNET_INI_FILE ]; then
    echo "Simulation configuration file \"$OMNETPP_INI_FILE\" not found. Aborting"
    exit 1
fi

case "$modeval" in
    cmdenv)
        SIMTYPE="Cmdenv"
        ;;

    qtenv)
        SIMTYPE="Qtenv"
        ;;

    *)
        echo "Mode not specified" >&2
        showUsage
        exit 1
        ;;
esac

# Is a default output directory defined in the settings? Use this one.
# Otherwise create a generic one
GENERIC_SIM_OUTPUT_DIR=$(basename $OMNET_INI_FILE)
GENERIC_SIM_OUTPUT_DIR="$OMNET_OUTPUT_DIR$(date +"%Y-%m-%d_%H-%M-%S")_${GENERIC_SIM_OUTPUT_DIR%.*}"

if [ -z ${SIM_OUTPUT_DIR+x} ]; then
    SIM_OUTPUT_DIR=$GENERIC_SIM_OUTPUT_DIR
fi

mkdir -p simulations/$SIM_OUTPUT_DIR

# Keep the ini file in the output directory
cp $OMNET_INI_FILE simulations/$SIM_OUTPUT_DIR

echo ""
echo "############################# Simulation Settings #############################"
echo "#"
echo "# \$OMNET_INI_FILE : $OMNET_INI_FILE"
echo "# \$SIM_OUTPUT_DIR : simulations/$SIM_OUTPUT_DIR"
echo "#"
echo "# Simulation mode : $SIMTYPE"
echo "#"
echo "# Post processing : $PARSERS_FILE"
echo "#"
echo "# Simulation command:"
echo "# ./$OPS_MODEL_NAME -u $SIMTYPE -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l keetchi -l INET --result-dir=$SIM_OUTPUT_DIR" | tee simulations/$SIM_OUTPUT_DIR/sim_command.txt
echo "#"
echo "###############################################################################"
echo ""

SIM_START_DAY=$(date +%Y%m%d) # For later logfile localization
./$OPS_MODEL_NAME -u $SIMTYPE -f $OMNET_INI_FILE -n simulations/:src/:$INET_NED/ -l keetchi -l INET --result-dir=$SIM_OUTPUT_DIR
ret=$?

# Did the simulation return something else than "0"? -> Error
if [ $ret -ne 0 ]; then
    echo "Error during simulation run. Simulation returned code $ret"
    exit $ret
fi

# Simulation done. Perform post-processing steps
echo "Simulation ended successfully"

if [ -z  ${BACKUP_DIR+x} ]; then
    echo "Not backing up the data!"
else
    if [ -d $BACKUP_DIR ]; then
        INPUTDIR="simulations/$SIM_OUTPUT_DIR"
        BACKUPFILE="$BACKUP_DIR/$HOSTNAME-$(basename $GENERIC_SIM_OUTPUT_DIR).tar.gz"
        mkdir -p outdir
        tar -cvzf "$BACKUPFILE" "$INPUTDIR"

    else
        echo "##########"
        echo "\"$BACKUP_DIR\" is not a valid directory. Canceling backup..."
        echo "##########"
    fi
fi


