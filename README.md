# OPS
The Opportunistic Protocol Simulator (OPS, pronounced as oops!!!) is a set of
simulation models for OMNeT++ to simulate opportunistic networks. It has a
modular architecture where different protocols relevant to opportunistic networks
can be developed and plugged in. The details of prerequisites, installation, configuration
and simulating are given in the following sections.


## Prerequisites

The models of OPS require OMNeT++, the INET framework, the KeetchiLib and some mobility models
that are not part of the INEt framework. Each of the sections below describe how these are
installed and configured.

### OMNeT++ (Version 5.4.1)

OPS requires OMNeT++. Install OMNeT++ version 5.4.1 using the following resources.

- [OMNeT++ Installation Guide](https://omnetpp.org/documentation/)
- [OMNeT++ Installation Binaries](https://omnetpp.org/download/)

Once OMNeT++ is installed, run the OMNeT++ IDE. The IDE provides options to install all the 
additional components (OMNeT++ frameworks, libraries, etc.) required by OPS.


### INET Framework (Version 4.1.0)

OPS requires the use of [INET Framework](https://inet.omnetpp.org), version 4.1.0, of OMNeT++ to simulate
mobility. There are a number of ways of installing the INET Framework in the IDE. OPS require the exact 
version 4.1.0. So, do not install the default version prompted by the IDE when run for the first time.

There are many ways of installing INET 4.1.0. We recommend the following way.

- Run the OMNeT++ IDE
- Select `File->Import->Git->Projects from Git->Clone URI->` and provide the following parameters and 
install INET
- Parameters,
  - URI `https://github.com/inet-framework/inet.git`
  - Branch `v4.1.x`
- Select the installed project in `Project Explorer` pane and build INET using `Project->Build Project`

Note: The build project should result in an error free build.


### KeetchiLib

An external library called `KeetchiLib` is used by the `KKeetchiLayer` model to handle all the functionality
related to the [Organic Data Dissemination](https://www.mdpi.com/1999-5903/11/2/29/htm)
forwarding model. The code for this library is available at Github. 

There are many ways of installing KeetchiLib. We recommend the following way.

- Run the OMNeT++ IDE
- Select `File->Import->Git->Projects from Git->Clone URI->` and provide the following parameters 
and install KeetchiLib
- Parameters,
  - URI `https://github.com/ComNets-Bremen/KeetchiLib.git`
  - Branch `master`
- Select the installed project in `Project Explorer` pane and build KeetchiLib using `Project->Build Project`

Note: The build project should result in an error free build.


###  SWIM Mobility Model

[Small Worlds in Motion](https://arxiv.org/pdf/0809.2730.pdf) (SWIM) is a mobility model
developed by A. Mei and J. Stefa. The INET code for the SWIM mobility model was developed 
by our research group. SWIM is used by many of the OPS scenarios for mobility. 

There are many ways of installing SWIM. We recommend the following way.

- Run the OMNeT++ IDE
- Select `File->Import->Git->Projects from Git->Clone URI->` and provide the following parameters 
and install SWIMMobility
- Parameters,
  - URI `https://github.com/ComNets-Bremen/SWIMMobility.git`
  - Branch `master`
- Copy the following 3 files into the INET project folder `src/inet/mobility/single`
  - SWIMMobility.ned
  - SWIMMobility.h
  - SWIMMobility.cc
- Select the INET project in `Project Explorer` pane and rebuild INET using `Project->Build Project`


## Install and build OPS

Once the prerequisits are installed and built, to install and build OPS, follow the procedure below.

- Run the OMNeT++ IDE
- Select `File->Import->Git->Projects from Git->Clone URI->` and provide the following parameters 
and install OPS
- Parameters,
  - URI `https://github.com/ComNets-Bremen/OPS.git`
  - Branch `master`
- Copy the following 2 files, located in `res/inet-models/ExtendedSWIMMobility` into the INET 
project folder `src/inet/mobility/contract`
  - IReactiveMobility.ned
  - IReactiveMobility.h
- Copy the following 2 files, located in `res/inet-models/ExtendedSWIMMobility` into the INET 
project folder `src/inet/mobility/single`
  - ExtendedSWIMMobility.ned
  - ExtendedSWIMMobility.h
- Select the INET project in `Project Explorer` pane and rebuild INET using `Project->Build Project`
- Select the OPS project in `Project Explorer` pane and select `File->Properties->Project References`
- Tick the installed INET and KeetchiLib, and click `Apply and Close`.
- Select the OPS project again in `Project Explorer` pane and build using `Project->Build Project`

Note: The build project should result in an error free build.

## Running Simulations

## Viewing Results

## Creating New Scenarios

## Important Model Parameters

## Help

## Known Problems

## Questions or Comments

Firstly, if you have any questions, please check the Known Problems section and if you cannot find answers there, then
write to us. Secondly, if you have any comments or suggestions, we are very glad to hear them. In both cases, please
write to us using any of the e-mail adresses below.

  - Asanga Udugama (adu@comnets.uni-bremen.de)
  - Jens Dede (jd@comnets.uni-bremen.de)
  - Anna Förster (anna.foerster@comnets.uni-bremen.de)
  - Vishnupriya Parimalam (vp@fb1.uni-bremen.de)



## Model Contributors

There are many individuals who have used and extended OPS, either by extending existing models or by introducing new 
models. This section lists the individuals who have done such contributions.

  - Anas bin Muslim
  - Jibin P. John
  - Karima Khandaker 
  - Kirishanth Chethuraja
  
  