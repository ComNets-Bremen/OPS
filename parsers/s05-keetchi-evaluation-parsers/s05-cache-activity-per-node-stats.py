#!/usr/bin/python
#
# Script to extract and process cache activity per node
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 2018-feb-05
#

import argparse
import csv
import pickle
import os.path, sys

# Include the upper directory to access the helper module
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir))

from helper import NodeHelpers, FileHelpers

# Keys and default values required by this parser
REQUIRED_KEYS = {

        "data_items_loved"                  : "",
        "data_items_non_loved"              : "",

        "cache_additions_count_total"       : 0,
        "cache_additions_count_loved"       : 0,
        "cache_additions_count_non_loved"   : 0,
        "cache_additions_bytes_total"       : 0,
        "cache_additions_bytes_loved"       : 0,
        "cache_additions_bytes_non_loved"   : 0,

        "cache_updates_count_total"         : 0,
        "cache_updates_count_loved"         : 0,
        "cache_updates_count_non_loved"     : 0,
        "cache_updates_bytes_total"         : 0,
        "cache_updates_bytes_loved"         : 0,
        "cache_updates_bytes_non_loved"     : 0,

        "cache_removals_count_total"        : 0,
        "cache_removals_count_loved"        : 0,
        "cache_removals_count_non_loved"    : 0,
        "cache_removals_bytes_total"        : 0,
        "cache_removals_bytes_loved"        : 0,
        "cache_removals_bytes_non_loved"    : 0,
        }

# Names of nodes to be ignored
IGNORED_NODES = ["abc", "xyz"]

# Parse the lines and store the data in the corresponding node object
def parseLine(line, nodeObjects):

    if "INFO" in line and "KBasicUBM" in line and ">!<NE>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())

        if words[8].strip() == "L":
            node["data_items_loved"] += (words[4].strip() + " ")
        else:
            node["data_items_non_loved"] += (words[4].strip() + " ")
            

    elif "INFO" in line and ("KeetchiLib" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and ">!<CA>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["cache_additions_count_total"] += 1
        node["cache_additions_bytes_total"] += int(words[6].strip())
        if node["data_items_loved"].find(words[5].strip()) >= 0:
            node["cache_additions_count_loved"] += 1
            node["cache_additions_bytes_loved"] += int(words[6].strip())
        elif node["data_items_non_loved"].find(words[5].strip()) >= 0:
            node["cache_additions_count_non_loved"] += 1
            node["cache_additions_bytes_non_loved"] += int(words[6].strip())
        else:
            print("Event not present for", words[2].strip(), "-", words[5].strip())
            sys.exit(2)

    elif "INFO" in line and ("KeetchiLib" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and ">!<CU>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["cache_updates_count_total"] += 1
        node["cache_updates_bytes_total"] += int(words[6].strip())
        if node["data_items_loved"].find(words[5].strip()) >= 0:
            node["cache_updates_count_loved"] += 1
            node["cache_updates_bytes_loved"] += int(words[6].strip())
        elif node["data_items_non_loved"].find(words[5].strip()) >= 0:
            node["cache_updates_count_non_loved"] += 1
            node["cache_updates_bytes_non_loved"] += int(words[6].strip())
        else:
            print("Event not present for", words[2].strip(), "-", words[5].strip())
            sys.exit(2)

    elif "INFO" in line and ("KeetchiLib" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and ">!<CR>!<" in line:
        words = line.split(">!<")
        node = nodeObjects.getNode(words[2].strip())
        node["cache_removals_count_total"] += 1
        node["cache_removals_bytes_total"] += int(words[6].strip())
        if node["data_items_loved"].find(words[5].strip()) >= 0:
            node["cache_removals_count_loved"] += 1
            node["cache_removals_bytes_loved"] += int(words[6].strip())
        elif node["data_items_non_loved"].find(words[5].strip()) >= 0:
            node["cache_removals_count_non_loved"] += 1
            node["cache_removals_bytes_non_loved"] += int(words[6].strip())
        else:
            print("Event not present for", words[2].strip(), "-", words[5].strip())
            sys.exit(2)


# Main function
def main():
    parser = argparse.ArgumentParser(description='Create statistics: Cache activities')
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
        with open(fileHelper.getGenericOutput(suffix="_ca"), "wb") as nodeOutput:
            print "Writing file", nodeOutput.name

            # Heading
            nodeOutput.write(" >!< ".join([
                    "# seq",
                    "node name",
                    "cache additions count total",
                    "cache additions count loved",
                    "cache additions count non-loved",

                    "cache additions bytes total",
                    "cache additions bytes loved",
                    "cache additions bytes non-loved",

                    "cache updates count total",
                    "cache updates count loved",
                    "cache updates count non-loved",

                    "cache updates bytes total",
                    "cache updates bytes loved",
                    "cache updates bytes non-loved",

                    "cache removals count total",
                    "cache removals count loved",
                    "cache removals count non-loved",

                    "cache removals bytes total",
                    "cache removals bytes loved",
                    "cache removals bytes non-loved"
                ])+"\n")

            # Data
            for number, key in enumerate(nodes.getKnownNodes()):
                if key in IGNORED_NODES:
                    continue
                node = nodes.getNode(key)
                nodeOutput.write(" >!< ".join([
                    str(number+1),
                    str(node["name"]),

                    str(node["cache_additions_count_total"]),
                    str(node["cache_additions_count_loved"]),
                    str(node["cache_additions_count_non_loved"]),

                    str(node["cache_additions_bytes_total"]),
                    str(node["cache_additions_bytes_loved"]),
                    str(node["cache_additions_bytes_non_loved"]),

                    str(node["cache_updates_count_total"]),
                    str(node["cache_updates_count_loved"]),
                    str(node["cache_updates_count_non_loved"]),

                    str(node["cache_updates_bytes_total"]),
                    str(node["cache_updates_bytes_loved"]),
                    str(node["cache_updates_bytes_non_loved"]),

                    str(node["cache_removals_count_total"]),
                    str(node["cache_removals_count_loved"]),
                    str(node["cache_removals_count_non_loved"]),

                    str(node["cache_removals_bytes_total"]),
                    str(node["cache_removals_bytes_loved"]),
                    str(node["cache_removals_bytes_non_loved"])

                    ])+"\n")

        # Total statistics
        with open(fileHelper.getGenericOutput(suffix="_cat"), "wb") as totalOutput:
            print "Writing file", totalOutput.name

            # Heading
            totalOutput.write(" >!< ".join([
                    "# totals",
                    "cache additions count total",
                    "cache additions count loved",
                    "cache additions count non-loved",

                    "cache additions bytes total",
                    "cache additions bytes loved",
                    "cache additions bytes non-loved",

                    "cache updates count total",
                    "cache updates count loved",
                    "cache updates count non-loved",

                    "cache updates bytes total",
                    "cache updates bytes loved",
                    "cache updates bytes non-loved",

                    "cache removals count total",
                    "cache removals count loved",
                    "cache removals count non-loved",

                    "cache removals bytes total",
                    "cache removals bytes loved",
                    "cache removals bytes non-loved"
                ])+"\n")

            allData = nodes.getTotalData(IGNORED_NODES)
            totalOutput.write(" >!< ".join([
                    "totals",

                    str(allData["cache_additions_count_total"]),
                    str(allData["cache_additions_count_loved"]),
                    str(allData["cache_additions_count_non_loved"]),

                    str(allData["cache_additions_bytes_total"]),
                    str(allData["cache_additions_bytes_loved"]),
                    str(allData["cache_additions_bytes_non_loved"]),

                    str(allData["cache_updates_count_total"]),
                    str(allData["cache_updates_count_loved"]),
                    str(allData["cache_updates_count_non_loved"]),

                    str(allData["cache_updates_bytes_total"]),
                    str(allData["cache_updates_bytes_loved"]),
                    str(allData["cache_updates_bytes_non_loved"]),

                    str(allData["cache_removals_count_total"]),
                    str(allData["cache_removals_count_loved"]),
                    str(allData["cache_removals_count_non_loved"]),

                    str(allData["cache_removals_bytes_total"]),
                    str(allData["cache_removals_bytes_loved"]),
                    str(allData["cache_removals_bytes_non_loved"])

                    ])+"\n")

        # Everything important to a pickle file
        with open(fileHelper.getPickleFilename(), "wb") as pickleOutput:
            print "Writing file", pickleOutput.name
            pickle.dump(nodes.exportData(), pickleOutput)

if __name__ == "__main__":
    main()

