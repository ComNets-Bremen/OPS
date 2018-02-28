# Run OMNeT++ including INET and OPS.
#
# Used OMNeT++ version: 5.2.1
#
# TODO:
# - Make everything configurable
# - Maybe add some ideas from https://docs.omnetpp.org/tutorials/cloud/page2/#docker

FROM ubuntu:16.04

RUN apt-get update -y && apt-get install -y python3-minimal python3-pip \
      build-essential bison flex libxml2-dev zlib1g-dev python-minimal wget

WORKDIR /opt/


RUN wget https://omnetpp.org/omnetpp/send/30-omnet-releases/2323-omnetpp-5-2-1-core \
      --referer=https://omnetpp.org/omnetpp -O omnetpp.tgz --progress=dot:giga

RUN tar xf omnetpp.tgz && rm omnetpp.tgz

ENV PATH /opt/omnetpp-5.2.1/bin:$PATH

WORKDIR omnetpp-5.2.1

RUN apt-get install -y tcl-dev tk-dev xvfb
RUN apt-get install -y libopenscenegraph-dev openscenegraph-plugin-osgearth libosgearth-dev qt4-default

RUN ./configure WITH_TKENV=no WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no

RUN make MODE=release -j $(nproc)
WORKDIR /opt/

RUN apt-get install -y git

RUN apt-get install -y autoconf automake libtool

RUN git clone https://github.com/ComNets-Bremen/OPS.git
WORKDIR OPS

RUN ./bootstrap.sh
RUN ./ops-makefile-setup.sh
RUN make -j $(nproc)


RUN mkdir -p simulations/out

# Run the simulation
RUN ./ops-simu-run.sh -m cmdenv
