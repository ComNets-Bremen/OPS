#!/usr/bin/python
#
# Script to extract and process per node per message
# delays and delivery ratios
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 20-sep-2017
#
# Author: Jens Dede (jd@comnets.uni-bremen.de)
# Date: 16-jan-2018
#
# TODO:
#
# - Do not create output files in case of no results (be tolerant against wrong
#       input file format)
# - Store information in a more generic way (-> NodeInformationHelpers)
# - Store all data together (not in each function call separately
# - ...

import sys, re, os.path
import argparse

# Include the upper directory to access the helper module
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), os.pardir))

from helper import NodeHelpers, FileHelpers


EXCLUDED_NODES = ["abc", "xyz"]

class Event:
    def __init__(self, event_name, event_type):
        self.event_name = event_name
        self.injected_time = -1.0
        self.delay = -1.0
        self.event_type = event_type
        self.travelled_hops = -1


class Node:
    def __init__(self, node_name):
        self.node_name = node_name
        self.events = []
        self.loved_events_received = 0
        self.nonloved_events_received = 0


def get_node(nodes, node_name):
    node_found = False
    for node in nodes:
        if node.node_name == node_name:
            node_found = True
            break

    if not node_found:
        node = Node(node_name)
        nodes.append(node)

    return(node)


def get_event(events, event_name, add_if_not, event_type):
    event_found = False
    for event in events:
        if event.event_name == event_name:
            event_found = True
            break

    if event_found:
        return event
    elif not event_found and add_if_not:
        event = Event(event_name, event_type)
        events.append(event)
        return(event)
    else:
        return None



def extract_from_log_and_compute(fileHelper, nodes):
    all_events = []

    with open(fileHelper.getInputFile(), "r") as inputfile:
        print "Input File:                                                     ", inputfile.name

        for line in inputfile:
            if "INFO" in line and "KBasicUBM" in line and ">!<NE>!<" in line:
                words = line.split(">!<")

                # print "ne node=", words[2].strip(), " data=", words[4].strip(), " type=", words[8].strip()

                global_event = get_event(all_events, words[4].strip(), True, 'X')
                node = get_node(nodes, words[2].strip())
                event = get_event(node.events, words[4].strip(), True, words[8].strip())

            elif "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) \
                and ">!<UI>!<DM>!<" in line:
                words = line.split(">!<")

                # print "ui dm data=", words[8].strip(), " time=", words[1].strip()

                global_event = get_event(all_events, words[8].strip(), False, 'X')
                if global_event.injected_time == -1.0:
                    global_event.injected_time = float(words[1].strip())

            elif "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) \
                and ">!<LI>!<DM>!<" in line:
                words = line.split(">!<")

                # print "li dm node=", words[2].strip(), " data=", words[8].strip(), " time=", words[1].strip()

                node = get_node(nodes, words[2].strip())
                event = get_event(node.events, words[8].strip(), False, "X")
                if not event is None and event.delay == -1.0:
                    global_event = get_event(all_events, words[8].strip(), False, "X")
                    event.delay = float(words[1].strip()) - global_event.injected_time
                if not event is None and event.travelled_hops == -1:
                    event.travelled_hops = int(words[11].strip())


def print_loved_event_values(fileHelper, nodes):

    with open(fileHelper.getGenericOutput(suffix="_md_l"), "w") as outputfile_md_l, \
            open(fileHelper.getGenericOutput(suffix="_mh_l"), "w") as outputfile_mh_l, \
            open(fileHelper.getGenericOutput(suffix="_madrh_l"), "w") as outputfile_madrh_l, \
            open(fileHelper.getGenericOutput(suffix="_madrh_net"), "w") as outputfile_madrh_net:

        print "Every Message Delay (Loved):                                    ", outputfile_md_l.name
        print "Every Message Hops Travelled (Loved):                           ", outputfile_mh_l.name
        print "Per Node Average Delays, Ratios and Hops Travelled (Loved):     ", outputfile_madrh_l.name
        print "Network Average Delay, Ratio and Travelled Hops (Network-wide): ", outputfile_madrh_net.name

        outputfile_md_l.write("# seq >!< node name >!< loved data name >!< delay \n")
        outputfile_mh_l.write("# seq >!< node name >!< loved data name >!< hops travelled \n")
        outputfile_madrh_l.write("# seq >!< node name >!< average delay >!< total loved >!< loved received >!< delivery ratio >!< average hops travelled \n")
        outputfile_madrh_net.write("# type >!< network average delay >!< network total >!< network received >!< network delivery ratio >!< network hops travelled \n")

        node_count = 0
        total_avg_delays = 0.0
        total_events = 0
        total_events_received = 0
        total_delivery_ratios = 0.0
        msg_delay_seq = 0
        node_delay_ratio_seq = 0
        total_avg_travelled_hops = 0.0
        travelled_hops_seq = 0

        for node in nodes:
            if any(node.node_name in ex_node_name for ex_node_name in EXCLUDED_NODES):
                continue

            event_count = 0
            delay_acc = 0.0
            avg_delay = 0.0
            delivery_ratio = 0.0
            travelled_hops_acc = 0.0
            avg_travelled_hops = 0.0
            for event in node.events:
                if event.event_type != "L":
                    continue

                event_count += 1

                if event.delay > -1.0:
                    node.loved_events_received += 1
                    delay_acc += event.delay
                    msg_delay_seq += 1
                    outputfile_md_l.write(str(msg_delay_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.delay) + "\n")
                if event.travelled_hops > -1:
                    travelled_hops_acc += event.travelled_hops
                    travelled_hops_seq += 1
                    outputfile_mh_l.write(str(travelled_hops_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.travelled_hops) + "\n")

            if node.loved_events_received > 0:
                avg_delay = float(delay_acc) / node.loved_events_received
                delivery_ratio = float(node.loved_events_received) / event_count
                avg_travelled_hops = float(travelled_hops_acc) / node.loved_events_received

            node_delay_ratio_seq += 1
            outputfile_madrh_l.write(str(node_delay_ratio_seq) + " >!< " + node.node_name + " >!< " +  str(avg_delay) + " >!< " + \
                                    str(event_count) + " >!< " + str(node.loved_events_received) + " >!< " + str(delivery_ratio) + " >!< " + str(avg_travelled_hops) + "\n")
            node_count += 1
            total_avg_delays += avg_delay
            total_events += event_count
            total_events_received += node.loved_events_received
            total_delivery_ratios += delivery_ratio
            total_avg_travelled_hops += avg_travelled_hops

        network_avg_delay = total_avg_delays / node_count
        network_delivery_ratio = total_delivery_ratios / node_count
        network_avg_travelled_hops = total_avg_travelled_hops / node_count

        outputfile_madrh_net.write(" loved >!< " + str(network_avg_delay) + " >!< " +  str(total_events) + " >!< " + \
                                    str(total_events_received) + " >!< " + str(network_delivery_ratio) + " >!< " + str(network_avg_travelled_hops) + "\n")


def print_nonloved_event_values(fileHelper, nodes):

    with open(fileHelper.getGenericOutput(suffix="_md_nl"), "w") as outputfile_md_nl, \
         open(fileHelper.getGenericOutput(suffix="_mh_nl"), "w") as outputfile_mh_nl, \
         open(fileHelper.getGenericOutput(suffix="_madrh_nl"), "w") as outputfile_madrh_nl, \
         open(fileHelper.getGenericOutput(suffix="_madrh_net"), "a") as outputfile_madrh_net:

        print "Every Message Delay (non-Loved):                                ", outputfile_md_nl.name
        print "Every Message Hops Travelled (non-Loved):                       ", outputfile_mh_nl.name
        print "Per Node Average Delays, Ratios and Hops Travelled (non-Loved): ", outputfile_madrh_nl.name
        print "Network Average Delay, Ratio and Travelled Hops (Network-wide): ", outputfile_madrh_net.name


        outputfile_md_nl.write("# seq >!< node name >!< non-loved data name >!< delay \n")
        outputfile_mh_nl.write("# seq >!< node name >!< non-loved data name >!< hops travelled \n")
        outputfile_madrh_nl.write("# seq >!< node name >!< average delay >!< total non-loved >!< non-loved received >!< delivery ratio >!< average hops travelled \n")

        node_count = 0
        total_avg_delays = 0.0
        total_events = 0
        total_events_received = 0
        total_delivery_ratios = 0.0
        msg_delay_seq = 0
        node_delay_ratio_seq = 0
        total_avg_travelled_hops = 0.0
        travelled_hops_seq = 0

        for node in nodes:
            if any(node.node_name in ex_node_name for ex_node_name in EXCLUDED_NODES):
                continue

            event_count = 0
            delay_acc = 0.0
            avg_delay = 0.0
            delivery_ratio = 0.0
            travelled_hops_acc = 0.0
            avg_travelled_hops = 0.0
            for event in node.events:
                if event.event_type == "L":
                    continue

                event_count += 1

                if event.delay > -1.0:
                    node.nonloved_events_received += 1
                    delay_acc += event.delay
                    msg_delay_seq += 1
                    outputfile_md_nl.write(str(msg_delay_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.delay) + "\n")
                if event.travelled_hops > -1:
                    travelled_hops_acc += event.travelled_hops
                    travelled_hops_seq += 1
                    outputfile_mh_nl.write(str(travelled_hops_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.travelled_hops) + "\n")

            if node.nonloved_events_received > 0:
                avg_delay = float(delay_acc) / node.nonloved_events_received
                delivery_ratio = float(node.nonloved_events_received) / event_count
                avg_travelled_hops = float(travelled_hops_acc) / node.nonloved_events_received

            node_delay_ratio_seq += 1
            outputfile_madrh_nl.write(str(node_delay_ratio_seq) + " >!< " + node.node_name + " >!< " +  str(avg_delay) + " >!< " + \
                                    str(event_count) + " >!< " + str(node.nonloved_events_received) + " >!< " + str(delivery_ratio) + " >!< " + str(avg_travelled_hops) + "\n")
            node_count += 1
            total_avg_delays += avg_delay
            total_events += event_count
            total_events_received += node.nonloved_events_received
            total_delivery_ratios += delivery_ratio
            total_avg_travelled_hops += avg_travelled_hops

        network_avg_delay = total_avg_delays / node_count
        network_delivery_ratio = total_delivery_ratios / node_count
        network_avg_travelled_hops = total_avg_travelled_hops / node_count

        outputfile_madrh_net.write(" non-loved >!< " + str(network_avg_delay) + " >!< " +  str(total_events) + " >!< " + \
                                    str(total_events_received) + " >!< " + str(network_delivery_ratio) + " >!< " + str(network_avg_travelled_hops) + "\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Script to extract and process per node per message delays and delivery ratios')
    parser.add_argument('logfiles', type=str, nargs="+", help="The logfiles")
    args = parser.parse_args()

    for logfile in args.logfiles:
        print "Processing", logfile
        fileHelper = FileHelpers.FileHelper(logfile)

        nodes = []
        extract_from_log_and_compute(fileHelper, nodes)
        print_loved_event_values(fileHelper, nodes)
        print_nonloved_event_values(fileHelper, nodes)


