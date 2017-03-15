#/bin/bash
#
# Bash script to setup all the Makefiles required to compile
# the Keetchi OMNeT++ model.
# The model uses the Keetchi API and therefore must be included
# in the compilation and linking (KEETCHI_API_PATH).
#
# @author: Asanga Udugama (adu@comnets.uni-bremen.de)
# @date: 18-may-2016
#

source ./tools/shell-functions.sh
loadSettings

cd simulations
opp_makemake -r --deep -I$INET_PATH -L$INET_PATH -lINET --mode release -o $OPS_MODEL_NAME -f
cd ..
cd src
opp_makemake -r --deep -I$INET_PATH -L$INET_PATH -lINET --mode release -o $OPS_MODEL_NAME -f
cd ..
opp_makemake -r --deep -Xmodules -I$INET_PATH -L$INET_PATH -lINET --mode release -o $OPS_MODEL_NAME -f

echo "Run make to compile and make the $KEETCHI_MODEL_NAME executable"
