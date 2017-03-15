# OPS
Opportunistic Protocol Simulator


A simulation model in OMNeT++ to simulate networks and nodes that employ the
Organic Data Dissemination (ODD) model to perform opportunistic communications.

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

The number of computing devices of the IoT are expected to grow exponentially.
To address the communication needs of the IoT, research is being done to
develop new networking architectures and to extend existing architectures. An
area that lacks attention in these efforts is the emphasis on utilisation of
omnipresent local data. There are a number of issues (e.g., underutilisation of
local resources and dependence on cloud based data) that need to be addressed
to exploit the benefits of utilising local data.

The Sustainable Communication Networks group (ComNets) at the University of
Bremen has identified a novel data dissemination model, called the
Organic Data Dissemination (ODD) model to utilise the omni-present data around
us, where devices deployed with the ODD model are able to operate even without
the existence of networking infrastructure. The realisation of the ODD model
requires innovations in many different area including the areas of
opportunistic communications, naming of information, direct peer-to-peer
communications and reinforcement learning.

This software, the OPS is an OMNeT++ based simulator to simulate nodes
(networks) deployed with the ODD model.

The detailed description of the ODD model is presented in the paper
[A Novel Data Dissemination Model for Organic Data Flows](http://link.springer.com/chapter/10.1007%2F978-3-319-26925-2_18),
published at the 2015 MONAMI conference.


Prerequisites
-------------

OPS requires the following software to be installed and operational
in the systems on which it is run.

### OMNeT++ 5.0 or above

OMNeT++ 5.0 or any new version must be installed and the PATH variable must be
set to run all the binaries of OMNeT++.

e.g.,

`export PATH=/Users/adu/Development/omnet/omnetpp-5.0/bin:$PATH (on a Mac OS X)`


### INET 3.2.4 or above

OPS requires the use of [INET Framework](https://inet.omnetpp.org) of OMNeT++ to simulate mobility and checks out an own version of INET where running `./bootstrap.sh`.
As downloading and compiling INET takes a while, you might prefer using your
own checkout of INET. To do so, please change the following values in the file
`settings.default`:

- `INET_BUILD=false`
- `INET_PATH=<your path to INET>`

When building the `INET Framework`, make sure to build and use the `release` version (not the `debug` version).


Caveats
-------

Though OMNeT++ models are possible to be executed in MS Windows, the scripts
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

1. Run `bootstrap.sh` to build the Inet if you are not using your own version.

2. Run the `ops-makefile-setup.sh` script. This will result in the creation of
the Makefiles in all the relevant folders.

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
   - `KPromoteApp` - Generates data (only) in a given interval or using an exponential
     distribution

2. Opportunistic Networking Layer - Performs the forwarding of data and feedback according
   to the forwarding strategy employed. Current implementations are,

   - `KRRSLayer` - Implements a simple forwarding strategy based on the Randomised
     Rumor Spreading (RRS) algorithm

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

Check the sample .ini files in `simulations/` folder to see all the parameters.


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


Support
-------

If you have any questions or comments, please write to,

  - Asanga Udugama (adu@comnets.uni-bremen.de),
  - Jens Dede (jd@comnets.uni-bremen.de) or
  - Anna Foerster (anna.foerster@comnets.uni-bremen.de)




