# OPS
The Opportunistic Protocol Simulator (OPS, pronounced as oops!!!) is a set of 
simulation models in OMNeT++ to simulate opportunistic networks. It has a 
modular architecture where different protocols relevant to opportunistic networks 
can be developed and plugged in. The details of current models available 
are given in the following sections. The immediately following section is a
`tl;dr` for anyone who wishes to get it up and running without going through 
all the explanations.    

tl;dr
-----

For Linux or MacOS:

- Install OMNeT++
- Clone this repository: `git clone https://github.com/ComNets-Bremen/OPS`
- Checkout and build dependencies: `./bootstrap.sh`
- Create makefiles: `./ops-makefile-setup.sh`
- Build simulation: `make`
- Create the `simulations/out` folder
- Run simulation: `./ops-simu-run.sh -m tkenv`
- Enjoy


Introduction
------------

The number of computing devices of the Internet of Things (IoT) are expected 
to grow exponentially. To address the communication needs of the IoT, research 
is being done to develop new networking architectures and to extend existing 
architectures. 

Opportunistic Networking is an architecture that is currently being considered for
communications in the IoT. The Sustainable Communication Networks group (ComNets) 
at the University of Bremen has developed a simulator called the Opportunistic 
Protocol Simulator (OPS) to evaluate protocols and mechanisms for opportunistic 
networks in the context of the IoT.

This repository contains the OMNeT++ based models of OPS. OPS consist of nodes, 
each of which implements a protocol stack with the functionality required to 
perform opportunistic networking. This release of the models consist of
models relevant to each protocol layer.


Prerequisites
-------------

OPS requires the following software to be installed and operational
in the systems on which it is run.

### OMNeT++ 5.0 

OMNeT++ 5.0 version must be installed and the PATH variable must be
set to run all the binaries of OMNeT++.

e.g.,

`export PATH=/Users/adu/Development/omnet/omnetpp-5.0/bin:$PATH (on a Mac OS X)`


### INET 3.4.0

OPS requires the use of [INET Framework](https://inet.omnetpp.org) of OMNeT++ to simulate mobility and checks out an own version of INET where running `./bootstrap.sh`.
As downloading and compiling INET takes a while, you might prefer using your
own checkout of INET. To do so, please change the following values in the file
`settings.default`:

- `INET_BUILD=false`
- `INET_PATH=<your path to INET>`

When building the `INET Framework`, make sure to build and use the `release` version (not the `debug` version).


Caveats
-------

Though OMNeT++ models are possible to be executed in Microsoft Windows, the scripts
provided here are only meant for Unix based systems (Mac OS X, Linux, etc.).


Adapt the default Settings
--------------------------

The OPS repository should work out of the box. The configuration
parameters are read from certain files:

- `settings.default` is located in the root directory of the simulation and
  contains the default parameters for building and running the simulation

- `~/.opsSettings` is the user defined version of the settings

We recommend to copy `settings.default` to your home directory and rename it to
`.opsSettings`. So your settings won't get lost in case of updates. If you
are later on facing problems (especially after an update), please check your
local settings. Maybe we have added additional parameters which you now should
add to your settings.

At the moment, the simulation uses the following parameters:

- `OPS_MODEL_NAME` - Name of the model executable generated my OMNeT++

- `INET_BUILD` - Boolean value to determine if a local INET checkout should be
  build. You can set this to false if you would like to use your own INET
  checkout

- `INET_PATH` - Path to where INET is located. Change this if you prefer using
  an own INET checkout

- `INET_LIB` - Path to where the dynamic library of INET is located (.so or .dylib).

- `INET_NED` - Path to where all the INET source files (i.e., NED files) are located.

- `OMNET_INI_FILE` - Path to your OMNeT++ simulation setup file. Change this
  according to the simulation you would like to run.

- `OMNET_OUTPUT_DIR` - Path where the binaries will be located


Building the Model
------------------

1. Run `bootstrap.sh` to build the INET if you are not using your own version.

2. Run the `ops-makefile-setup.sh` script. This will result in the creation of
the Makefiles in all the relevant folders, pulling the dependents linked in the 
`./modules/` folder and building these dependents. 

3. Run make from the root folder of OPS.

When successfully compiled and linked, an executable by the name given
in `OPS_MODEL_NAME` must be present in the root folder of OPS.



Running the Model
-----------------

1. Create the node model, the network and an appropriate OMNeT++ parameter file to
run the required simulations. Currently there are many samples of these 3 components.

  - Node Model - Currently implemented node models are `KNode.ned` and `KHeraldNode.ned`
  in the `src/` folder. IMPORTANT: If a new node model is created, remember to include
  the name of this model in the `expectedNodeTypes` parameter of the `KWirelessInterface`
  module
  - Network - Currently created simulation networks are `OpsNetA.ned` and
  `OpsNetB.ned` in the `simulations/` folder
  - Parameter File - Currently created parameter files are `omnetpp.ini` and
  `omnetpp-herald.ini` in the `simulations/` folder


3. Modify the settings to point to the created OMNeT++ parameters in the
previous step.

4. Create the results output folder `simulations/out`

5. Run the `ops-simu-run` script file to run simulations. Simulation can be run using the GUI
(Tkenv) mode or the command line mode. To enable the required mode, the  `-m` switch must be used.

  - `ops-simu-run -m tkenv` - The `tkenv` mode brings up the GUI version of the simulation
  where the buttons have to be pressed to start simulating and further, this mode also shows
  the animations. __WARNING:__ this mode results in longer simulation times (VERY LONG)

  - `ops-simu-run -m cmdenv` - The `cmdenv` mode directly starts the simulation and the
  command line is blocked until the simulation is completed.


The results files and the log files are created in the `out/` folders.


Creating Your Own Scenarios
---------------------------

The network file (i.e., scenario) created to simulate nodes deployed with the different
protocol layers are located in the `simulations/` folder. As indicated in the Running
the Model section, check the given example networks (e.g., `OpsNetA.ned` file) and the
parameter file (e.g., `omnetpp.ini` file) to make your own networks.


Node Architecture
-----------------

The architecture of a node uses a number of protocol layers which can be configured
based on the scenario considered. Generally, every node has the following layers.


                           +------------------------+
                           |  +------------------+  |
                           |  |Application Layer |  |
                           |  |                  |  |
                           |  +--------+---------+  |
                           |           |            |
                           |  +--------+---------+  |
                           |  |   Opportunistic  |  |
                           |  | Networking Layer |  |
                           |  +--------+---------+  |
                           |           |            |
                           |  +--------+---------+  |
                           |  | Link Adaptation  |  |
                           |  |     Layer        |  |
                           |  +--------+---------+  |
                           |           |            |
                           |  +--------+---------+  |
                           |  |    Link Layer    |  |
                           |  | +--------------+ |  |
                           |  | |   Mobility   | |  |
                           |  | +--------------+ |  |
                           |  +--------+---------+  |
                           +-----------|------------+
                                       |

Each of the above layers can be configured to use different implementations relevant
to the specific layer as listed below.

1. Application Layer - Applications generate data and feedback. Current applications
   are,

   - `KBruitApp` - Generates data and feedback randomly
   - `KHeraldApp` - Generates data from a given list and generates feedback for this
     data randomly
   - `KPromoteApp` - Generates data as constant, uniformly distributed or exponentially distributed traffic

2. Opportunistic Networking Layer - Performs the forwarding of data and feedback according
   to the forwarding strategy employed. Current implementations are,

   - `KRRSLayer` - Implements a simple forwarding strategy based on the Randomised
     Rumor Spreading (RRS) algorithm which randomly selects a data item to broadcast
     to a node's neighbourhood
   - `KEpidemicRoutingLayer` - Implements the epidemic routing algorithm as described 
     in the publication `Epidemic Routing for Partially-Connected Ad Hoc Networks` by
     A. Vahdat and D. Becker
 
3. Link Adaptation Layer - Tasked with converting packets sent by the Opportunistic
   Networking Layer to the specific link technology used (at Link Layer). Currently
   implemented has a simple pass-through layer.

   - `KLinkAdaptLayer` - Pass-through layer

4. Link Layer - Implements the operations of a link technology used. Currently has
   the following implementation.

   - `KWirelessInterface` - A simple wireless interface implementation (without the `INET
     framework`)

5. Mobility - Implements the mobility modelling for the node. Currently uses the interfaces
   provided by the `INET framework` and therefore, is able to use any of the mobility models
   supported by the `INET framework`.



Simulation Parameters
---------------------

Before running simulations, the OMNeT++ parameter file (.ini file) must be configured properly
to setup the node models and the network. Following are some of the important parameters to
consider.

- `numNodes` - The number of nodes in the network
- `wirelessRange` - The wireless coverage range. The default is 100 meters
- `forwardingLayer` - The forwarding layer to use
- `mobilityType` - The mobility model used to move the nodes 

There are many more parameters that need to be set according to the scenario to be simulated. Most 
of these parameters have default values. Check the sample .ini files in `simulations/` folder to 
see what parameters are usually set to run a simulation.



Parsers
-------

The `parsers/` folder provides a number of Python scripts to parse the logs created
during the simulations to generate statistics of the operations.

- `s01-liked-data.py` - This script generates stats related to liked and non-liked data
(based on Goodness value). Data items that have Goodness values >= 75 are considered as
liked data and the data with Goodness values below 75 are considered as non-liked data.
The script requires the log file created in `simulations/out/` as input to compute these
stat values (i.e., using the `-i` switch). As output, this script creates text files
with the computed stats and a set of Matlab scripts to generate the graphs.
__WARNING:__ The Matlab scripts are incomplete. They have to be modified to
get the correct graphs.

- `s02-traffic-spread.py` - This script generates stats related to total packets generated
by all nodes and how well the traffic is spread across the network (i.e., CoV of the means
of all nodes). The script requires the log file created in `simulations/out/` as input to
compute these stat values (i.e., using the `-i` switch). As output, this script creates
text files with the computed stats and a set of Matlab scripts to generate the graphs.
__WARNING:__ The Matlab scripts are incomplete. They have to be modified to
get the correct graphs.

- `s03-delivery-ratio.py` - This script generates stats related to delivery ratio and the
average delivery time. The script requires the log file created in `simulations/out/` as 
input to compute these stat values (i.e., using the `-i` switch) and the optional maximum 
simulation time to consider (i.e., using the `-m` switch). As output, this script creates
text files with the computed stats.

- `s04-contact-times.py` - This script generates the stats related to contact times (number
of contacts and average contact time). The script requires the log file created in 
`simulations/out/` as input to compute these stat values (i.e., using the `-i` switch),
total simulation time (-s), simulation period (i.e., the range as -p) and the time resolution
to consider for a unit of communications (-r). The -s, -p and -r are given in seconds.

You can develop your own parsers in Python or any other language (e.g., Google's GO language).



SWIM Mobility Model
-------------------

OPS can be configured to use any INET based mobility model. The Small Worlds in Motion (SWIM) 
mobility model is one such mobility model that was developed by our research group. If you 
wish to use this mobility model, follow the following steps.

1. Download the OMNeT++ INET SWIM mobility model from following Github repository.
 
`https://github.com/ComNets-Bremen/SWIMMobility.git`

2. Place the downloaded files in the following folder of local INET copy which was created 
when the `bootstrap.sh` was run.

`./modules/inet/src/inet/mobility/single/`

3. Recompile the INET in the following manner

- `cd modules/inet`
- `make clean`
- `make makefiles`
- `make MODE=release`

4. Set parameters in `.ini` file in `simulations/` folder to use the SWIM mobility model.



BonnMotion Mobility Model
-------------------------

Another mobility model that we have used extensively is the Bonn Motion model. This model
(wich is available in INET) requires a trace to move nodes in a network. The trace files
are generated using a tool provided by the authors of the Bonn Motion model. The tool can
be requested to generate traces based on synthetic mobility models (e.g., Random Way Point
mobility model) or from an actual mobility trace (e.g., San Fransisco Taxi trace at 
www.crawdad.com).



FAQs
----

Here are some answers to very commonly asked questions.

1. Why do I get the following error when I run `./ops-simu-run.sh -m cmdenv`?

`<!> Error: Cannot open output redirection file ...log2.txt`

Was the `simulations/out` created?

2. The output shown on the terminal does not point to any errors, but why does the simulation end quickly?

Check the log created in `simulations/out`. It may give hints to where the problem is.

3. The simulation uses other models or other parameters instead of what I set. Why is that?

Was the `.opsSettings` file created in home directory? If it was created, is it pointing to
another `.ini` file? If `.opsSettings` was not created, then it uses the `.ini` file in
`settings.default`. The actual `.ini` used is shown when running `./ops-simu-run.sh` script. 

4. Must the given OMNeT and INET versions be used?

We have developed, tested and run the OPS models using only the given versions of OMNeT and INET. You are
on your own if you decide to use other versions. Having said that, we like to mention - you are encouraged 
to try out newer versions and tell us if there are problems. Even better - if you can find fixes for these
problems and inform us, we will be very glad to publish them and acknowledge you.

5. Can OPS be installed and run in Microsoft Windows environments?

NO, OPS can not be installed and run in MS Windows environments. Of course OMNeT and any model developed 
for OMNeT can be run on MS Windows, but OPS is a simulator that has been setup only to be installed and
run in `*nix` based system (e.g., Mac OSX, Linux). 

6. Can the result parsers (in the `parsers/` folder) be used as they are? 

Yes, they can be used as they are, provided that the text output in the log files made by the OPS models in 
`simulations/out` have not been changed. A general rule to keep in mind about the parsers is as follows.

The existing parsers we developed were meant to obtain results that WE wanted. There is a possibility that 
these results and hence, the parsers, may not suit other users' requirements for results. Therefore, 
we recommend that you use these parsers to only get an idea to develop your own parsers that serve your 
own requirements.

One more thing - the current parsers are developed using Python. You are free to develop them in any language 
you like.

7. Why are no packets communicated between nodes?

If you have introduced a new node model (an example of an existing node model is `KPromoteNode`), then that name
must be added to the `expectedNodeTypes` parameter in the `KWirelessInterface.ned`.

8. Why is it that destinations are not identified even though the destination oriented parameter is set in 
`KPromoteApp`?

If you have introduced a new node model (an example of an existing node model is `KPromoteNode`), then that name
must be added to the `expectedNodeTypes` parameter in the `KPromoteApp.ned`.


Questions or Comments
---------------------

If you have any questions or comments, please write to,

  - Asanga Udugama (adu@comnets.uni-bremen.de)
  - Jens Dede (jd@comnets.uni-bremen.de)
  - Anna Foerster (anna.foerster@comnets.uni-bremen.de)
  - Anas bin Muslim (anas1@uni-bremen.de)





