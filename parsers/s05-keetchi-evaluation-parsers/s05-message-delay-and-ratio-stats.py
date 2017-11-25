#!/usr/bin/python
#
# Script to extract and process per node per message
# delays and delivery ratios
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 20-sep-2017

import sys, getopt, re, tempfile

inputfile = 0
outputfile1 = 0
outputfile2 = 0
outputfile3 = 0

nodes = []
all_events = []
excluded_nodes = ["abc", "xyz"]

class Event:
    def __init__(self, event_name):
        self.event_name = event_name
        self.injected_time = -1.0
        self.delay = -1.0


class Node:
    def __init__(self, node_name):
        self.node_name = node_name
        self.loved_events = []
        self.loved_events_received = 0


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


def get_event(events, event_name, add_if_not):
    event_found = False
    for event in events:
        if event.event_name == event_name:
            event_found = True
            break

    if event_found:
        return event
    elif not event_found and add_if_not:
        event = Event(event_name)
        events.append(event)
        return(event)
    else:
        return None


def parse_param_n_open_files(argv):
    global inputfile
    global outputfile1
    global outputfile2
    global outputfile3

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print "s05-message-delay-and-ratio-stats.py -i <inputfile>"
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's05-message-delay-and-ratio-stats.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg

    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    outputfile1 = open(re.sub('.txt', '_md.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_madr.txt', newfilename), "w+")
    outputfile3 = open(re.sub('.txt', '_madrn.txt', newfilename), "w+")

    print "Input File:                           ", inputfile.name
    print "Per Node, Per Message Delays:         ", outputfile1.name
    print "Per Node Average Delays and Ratios:   ", outputfile2.name
    print "Network Average Delay and Ratio:      ", outputfile3.name


def extract_from_log_and_compute():
    global inputfile
    
    global nodes
    global all_events
    
    for line in inputfile:
        if "INFO" in line and "KHeraldApp" in line and ">!<NE>!<" in line:
            words = line.split(">!<")
            
            # print "ne node=", words[2].strip(), " data=", words[4].strip(), " type=", words[8].strip()
            
            global_event = get_event(all_events, words[4].strip(), True)
            if words[8].strip() == "L":

                # print "    type L"

                node = get_node(nodes, words[2].strip())
                event = get_event(node.loved_events, words[4].strip(), True)

        elif "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) \
            and ">!<UI>!<DM>!<" in line:
            words = line.split(">!<")
            
            # print "ui dm data=", words[8].strip(), " time=", words[1].strip()
            
            global_event = get_event(all_events, words[8].strip(), False)
            if global_event.injected_time == -1.0:
                global_event.injected_time = float(words[1].strip())

        elif "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) \
            and ">!<LI>!<DM>!<" in line:
            words = line.split(">!<")
            
            # print "li dm node=", words[2].strip(), " data=", words[8].strip(), " time=", words[1].strip()
            
            node = get_node(nodes, words[2].strip())
            event = get_event(node.loved_events, words[8].strip(), False)
            if not event is None and event.delay == -1.0:
                global_event = get_event(all_events, words[8].strip(), False)
                event.delay = float(words[1].strip()) - global_event.injected_time


def print_values():
    global outputfile1
    global outputfile2
    global outputfile3

    global nodes
    global all_events
    global excluded_nodes
    
    count = 0
    total_avg_delays = 0.0
    total_loved_events = 0
    total_loved_events_received = 0
    total_delivery_ratios = 0.0

    outputfile1.write("# seq >!< node name >!< data name >!< delay \n")
    outputfile2.write("# seq >!< node name >!< average delay >!< total loved >!< loved received >!< delivery ratio \n")
    outputfile3.write("# all >!< network average delay >!< network total loved >!< network loved received >!< network delivery ratio \n")

    msg_delay_seq = 0
    node_delay_ratio_seq = 0
    for node in nodes:
        if any(node.node_name in ex_node_name for ex_node_name in excluded_nodes):
            continue

        delay_acc = 0.0
        avg_delay = 0.0
        delivery_ratio = 0.0
        for event in node.loved_events:
            if event.delay > -1.0:
                node.loved_events_received += 1
                delay_acc += event.delay
                msg_delay_seq += 1
                outputfile1.write(str(msg_delay_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.delay) + "\n")
        if node.loved_events_received > 0:
            avg_delay = float(delay_acc) / node.loved_events_received
            delivery_ratio = float(node.loved_events_received) / len(node.loved_events)
        node_delay_ratio_seq += 1
        outputfile2.write(str(node_delay_ratio_seq) + " >!< " + node.node_name + " >!< " +  str(avg_delay) + " >!< " + \
                                str(len(node.loved_events)) + " >!< " + str(node.loved_events_received) + " >!< " + str(delivery_ratio) + "\n")
        count += 1
        total_avg_delays += avg_delay
        total_loved_events += len(node.loved_events)
        total_loved_events_received += node.loved_events_received
        total_delivery_ratios += delivery_ratio

    network_avg_delay = total_avg_delays / count
    network_delivery_ratio = total_delivery_ratios / count
    outputfile3.write(" all >!< " + str(network_avg_delay) + " >!< " +  str(total_loved_events) + " >!< " + \
                                str(total_loved_events_received) + " >!< " + str(network_delivery_ratio) + "\n")



def close_files():
    global inputfile
    global outputfile1
    global outputfile2
    global outputfile3

    inputfile.close()
    outputfile1.close()
    outputfile2.close()
    outputfile3.close()


def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log_and_compute()
    print_values()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

