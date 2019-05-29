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
- Select `File->Import->Git->Projects from Git->Clone URI->` and provide the following parameters
- Parameters
  - Give URI `https://github.com/inet-framework/inet.git` and select branch `v4.1.x`
- Install
- Select `Project->Build Project` to build INET


### KeetchiLib

An external library called `KeetchiLib` is used by the `KKeetchiLayer` model to handle all the functionality
related to the [Organic Data Dissemination](https://www.mdpi.com/1999-5903/11/2/29/htm)
forwarding model. The code for this library is available at Github. 

There are many ways of installing KeetchiLib. We recommend the following way.

- Run the OMNeT++ IDE
- Select `File->Import->Git->Projects from Git->Clone URI->`
- Give URI `https://github.com/ComNets-Bremen/KeetchiLib.git`
- Install
- Select `Project->Build Project` to build KeetchiLib



###  SWIM Mobility Model

[Small Worlds in Motion](https://arxiv.org/pdf/0809.2730.pdf) (SWIM) is a mobility model
developed by A. Mei and J. Stefa. The INET code for the SWIM mobility model was developed by our research
group. SWIM is used by many of the OPS scenarios for mobility. 

There are many ways of installing SWIM. We recommend the following way.

- Run the OMNeT++ IDE
- Select `File->Import->Git->Projects from Git->Clone URI->`
- Give URI `https://github.com/ComNets-Bremen/SWIMMobility.git`
- Install
- Select `Project->Build Project` to build KeetchiLib





Questions or Comments
---------------------

Firstly, if you have any questions, please check the FAQ (linked above) and if you cannot find answers there, then
write to us. Secondly, if you have any comments or suggestions, we are very glad to hear them. In both cases, please
write to us using any of the e-mail adresses below.

  - Asanga Udugama (adu@comnets.uni-bremen.de)
  - Jens Dede (jd@comnets.uni-bremen.de)
  - Anna Förster (anna.foerster@comnets.uni-bremen.de)
  - Vishnupriya Parimalam (vp@fb1.uni-bremen.de)



Model Contributors
------------------

There are many individuals who have used and extended OPS, either by extending existing models or by introducing new models. This section lists the individuals who have done such contributions.

  - Anas bin Muslim
  - Jibin P. John
  - Karima Khandaker 
  - Kirishanth Chethuraja
  
  