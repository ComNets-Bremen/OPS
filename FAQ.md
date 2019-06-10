# Frequently Asked Questions and Known Problems

There have been many questions asked by users when setting up and using OPS. We have made
a list of the most widely asked questions below and the answers to them.

## Building OPS fails with IMobility headers not found

- Description: When building OPSLite, the compilation fails, complaining about the inability to find INET mobility headers.

- Solution: Since OPSLite uses the mobility headers of INET, the referenced projects section (`Properties -> Project References`) has to point to the exact INET version installed in your workspace. Sometimes, INET is installed as `inet4` or `inet`. So, make sure that the right INET entry is ticked. Untick all other unwanted projects.  

## Undefined symbols when building OPSLite on Microsoft Windows

- Description: With OMNeT++ 5.4.1 IDE on Windows, the building fails with undefined symbols pointing to the IMobility interface used. We found this to be due to OMNeT++ being setup by default to use the `clang` compiler. 

- Solution: Change the configure.user file to use gcc by setting "PREFER_CLANG=no" and rebuilding OMNeT++ and all the models installed (including INET). Check [Install Guide](https://www.omnetpp.org/doc/omnetpp/InstallGuide.pdf) for further information.


## Must the given OMNeT and INET versions be used?

We have developed, tested and run the OPS models using only the given versions of OMNeT and INET. You are
on your own if you decide to use other versions. Having said that, we like to mention - you are encouraged 
to try out newer versions and tell us if there are problems. Even better - if you can find fixes for these
problems and inform us, we will be very glad to publish them and acknowledge you.


## Why are no packets communicated between nodes?

If you have introduced a new node model (an example of an existing node model is `KMessengerNode`), then that name
must be added to the `expectedNodeTypes` parameter in the `KWirelessInterface.ned`.


## Where can I find the `50_traces.gpx.movements` file used with the BonnMotion Mobility Model?

Send us an email if you want a copy of this file.

