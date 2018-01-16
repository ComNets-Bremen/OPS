#!/usr/bin/python
#
# Script to extract and process sent packets per node
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 17-sep-2017
#
# Author: Jens Dede (jd@comnets.uni-bremen.de)
# Date: 15-jan-2018

import argparse
import csv
import pickle
import os.path, sys

# Include the upper directory to access the helper module
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir))

from helper import NodeHelpers, FileHelpers

# Keys and default values required by this parser
REQUIRED_KEYS = {
        "data_sent"         : 0,
        "data_sent_bytes"   : 0,
        "feedback_sent"     : 0,
        "feedback_sent_bytes": 0,
        "svm_sent"          : 0,
        "svm_sent_bytes"    : 0,
        "drm_sent"          : 0,
        "drm_sent_bytes"    : 0,
        }

# Names of nodes to be ignored
IGNORED_NODES = []

# Parse the lines and store the data in the corresponding node object
def parseLine(line, nodeObjects):
    if "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line) and ">!<LO>!<DM>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["data_sent"] += 1
        node["data_sent_bytes"] += int(words[10].strip())

    elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LO>!<DM>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["data_sent"] += 1
        node["data_sent_bytes"] += int(words[8].strip())

    elif "INFO" in line and "KKeetchiLayer" in line and ">!<LO>!<FM>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["feedback_sent"] += 1
        node["feedback_sent_bytes"] += int(words[11].strip())

    elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LO>!<SVM>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["svm_sent"] += 1
        node["svm_sent_bytes"] += int(words[10].strip())

    elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LO>!<DRM>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["drm_sent"] += 1
        node["drm_sent_bytes"] += int(words[8].strip())


# Main function
def main():
    parser = argparse.ArgumentParser(description='Create statistics: Number of packets per node')
    parser.add_argument('logfiles', type=str, nargs="+", help="The logfiles")
    args = parser.parse_args()

    for inputfile in args.logfiles:
        print "Processing", inputfile
        fileHelper = FileHelpers.FileHelper(inputfile)

        # Initialize the helper to handle the node information
        nodes = NodeHelpers.NodeInformationHandler()
        for key in REQUIRED_KEYS:
            nodes.addKey(key, REQUIRED_KEYS[key])

        # Get all data from the logfile and store it to the node object
        with open(fileHelper.getInputFile()) as iFile:
            for line in iFile:
                parseLine(line, nodes)

        if nodes.getLength() < 1:
            print "Got no information from this input file. Continuing with next one"
            continue

        # Store the per node statistics
        with open(fileHelper.getGenericOutput(suffix="_ps"), "wb") as nodeOutput:
            print "Writing file", nodeOutput.name

            # Heading
            nodeOutput.write(" >!< ".join([
                    "# seq",
                    "node name",
                    "total sent count",
                    "total sent bytes",
                    "data sent count",
                    "data sent bytes",
                    "feedback sent count",
                    "feedback sent bytes",
                    "svm sent count",
                    "svm sent bytes",
                    "drm sent count",
                    "drm sent bytes"
                ])+"\n")

            # Data
            for number, key in enumerate(nodes.getKnownNodes()):
                if key in IGNORED_NODES:
                    continue
                node = nodes.getNode(key)
                nodeOutput.write(" >!< ".join([
                    str(number+1),
                    str(node["name"]),
                    str(node["data_sent"] + node["feedback_sent"] + node["svm_sent"] + node["drm_sent"]),
                    str(node["data_sent_bytes"] + node["feedback_sent_bytes"] + node["svm_sent_bytes"] + node["drm_sent_bytes"]),
                    str(node["data_sent"]),
                    str(node["data_sent_bytes"]),
                    str(node["feedback_sent"]),
                    str(node["feedback_sent_bytes"]),
                    str(node["svm_sent"]),
                    str(node["svm_sent_bytes"]),
                    str(node["drm_sent"]),
                    str(node["drm_sent_bytes"])
                    ])+"\n")

        # Total statistics
        with open(fileHelper.getGenericOutput(suffix="_pst"), "wb") as totalOutput:
            print "Writing file", totalOutput.name

            # Heading
            totalOutput.write(" >!< ".join([
                    "# totals",
                    "total sent count",
                    "total sent bytes",
                    "data sent count",
                    "data sent bytes",
                    "feedback sent count",
                    "feedback sent bytes",
                    "svm send count",
                    "svm sent bytes",
                    "drm sent count",
                    "drm sent bytes"
                ])+"\n")

            allData = nodes.getTotalData(IGNORED_NODES)
            totalOutput.write(" >!< ".join([
                    "totals",
                    str(allData["data_sent"] + allData["feedback_sent"] + allData["svm_sent"] + allData["drm_sent"]),
                    str(allData["data_sent_bytes"] + allData["feedback_sent_bytes"] + allData["svm_sent_bytes"] + allData["drm_sent_bytes"]),
                    str(allData["data_sent"]),
                    str(allData["data_sent_bytes"]),
                    str(allData["feedback_sent"]),
                    str(allData["feedback_sent_bytes"]),
                    str(allData["svm_sent"]),
                    str(allData["svm_sent_bytes"]),
                    str(allData["drm_sent"]),
                    str(allData["drm_sent_bytes"])
                    ])+"\n")

        # Everything important to a pickle file
        with open(fileHelper.getPickleFilename(), "wb") as pickleOutput:
            print "Writing file", pickleOutput.name
            pickle.dump(nodes.exportData(), pickleOutput)

if __name__ == "__main__":
    main()

