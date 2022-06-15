#!/bin/bash

# running

../src/OPS -m -u Cmdenv -c "Herald-RSSI-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-NeighborOrderedForwarder-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-RSSI-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini

../src/OPS -m -u Cmdenv -c "Herald-NeighborOrderedForwarder-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini


../src/OPS -m -u Cmdenv -c "Herald-Epidemic-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-Optimum-Delay-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-Epidemic-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini

../src/OPS -m -u Cmdenv -c "Herald-Optimum-Delay-SFO" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-30m.ini



../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Opp-and-Same" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Same-and-Stat" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Stat-and-Same" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Opp-and-Stat" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Stat-and-Opp" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Same-Opp-and-Stat" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Same-Stat-and-Opp" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Opp-Stat-and-Same" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

../src/OPS -m -u Cmdenv -c "Herald-MobilitySameDirection-SFO-Opp-Same-and-Stat" -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib omnetpp-ops-sfo-10m.ini

