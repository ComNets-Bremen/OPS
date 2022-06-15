#!/bin/bash

# running

../src/OPS-angles -m -u Cmdenv -c "Herald-RSSI-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS-angles -m -u Cmdenv -c "Herald-NeighborOrderedForwarder-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS-angles -m -u Cmdenv -c "Herald-RSSI-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini

../src/OPS-angles -m -u Cmdenv -c "Herald-NeighborOrderedForwarder-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini


../src/OPS-angles -m -u Cmdenv -c "Herald-Epidemic-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS-angles -m -u Cmdenv -c "Herald-Optimum-Delay-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS-angles -m -u Cmdenv -c "Herald-Epidemic-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini

../src/OPS-angles -m -u Cmdenv -c "Herald-Optimum-Delay-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini




