# Frequently Asked Questions and Known Problems

When installing, building or simulating with OPS, there may be problems that are either known or new. We have listed here some of the known problems and solutions to them. There could be many others that we are not aware of and we encourage you to ask us if you have one, or better still, write to us with a solution using the email address shown at `Questions or Comments` section in the README.


## Building OPS fails with IMobility headers not found

- Description: When building OPS, the compilation fails, complaining about the inability to find INET mobility headers.

- Solution: Since OPS uses the mobility headers of INET, the referenced projects section (`Properties -> Project References`) has to point to the exact INET version installed in your workspace. Sometimes, INET is installed as `inet4` or `inet`. So, make sure that the right INET entry is ticked. Untick all other unwanted projects.  

## Undefined symbols when building OPS on Microsoft Windows

- Description: With OMNeT++ 5.4.1 IDE on Windows, the building fails with undefined symbols pointing to the IMobility interface used. We found this to be due to OMNeT++ being setup by default to use the `clang` compiler. 

- Solution: Change the configure.user file to use gcc by setting "PREFER_CLANG=no" and rebuilding OMNeT++ and all the models installed (including INET). Check [Install Guide](https://www.omnetpp.org/doc/omnetpp/InstallGuide.pdf) for further information.


## Must the given OMNeT++ and INET versions be used?

- Description: There are many OMNeT++ and INET versions available to download. The versions that are mentioned are not the latest or you require an older version of INET because of a certain feature available there.

- Solution: We have developed, tested and run the OPS models using only the given versions of OMNeT++ and INET. You are on your own if you decide to use other versions. Having said that, we like to mention - you are encouraged to try out newer versions and tell us if there are problems. Even better - if you can find fixes for these problems and inform us, we will be very glad to publish them and acknowledge you.


## Results files are large, how to reduce them?

- Description: After every simulation, the results files are created in the `simulations/results` folder. Depending on the simulation, they can be, sometimes, in Gigabytes. 

- Solutions: If you are only interested in the scalar results, then specify `**.vector-recording = false` in the `.ini` file used to run the simulation. This will stop all vector results from being written. If you only want the vector results of a certain protocol layer, then enable recording for that layer, e.g., `**.app.vector-recording = true` and disable other layers globally with `**.vector-recording = false`.


## Can simulations be run on the command-line without the graphical environment?

- Description: When running simulation campaigns with multiple `.ini` files, it is easier to automate the runs with scripts that initiate simulations on the command-line as background processes.

- Solutions: Yes, this can be done in MacOS or Linux environments in the following manner. 
  - Open console/terminal
  - Run the terminal multiplexer `screen` to create virtual terminals that execute processes in the background even after the console/terminal is closed
  - Run simulations in the virtual terminal
    ```bash
    myhome$ cd OPS/simulations
    myhome$ ../src/OPS -r 0 -m -u Cmdenv \
    -n .:../src:../../inet/src:../../inet/examples:../../inet/tutorials:../../inet/showcases:../../KeetchiLib \
    --image-path=../../inet/images -l ../../inet/src/INET -l ../../KeetchiLib/KeetchiLib \
    omnetpp-herald-epidemic.ini
    ```


