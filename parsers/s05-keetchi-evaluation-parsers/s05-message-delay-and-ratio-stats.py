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
outputfile11 = 0
outputfile2 = 0
outputfile3 = 0
outputfile31 = 0
outputfile4 = 0
outputfile5 = 0

nodes = []
all_events = []
excluded_nodes = ["abc", "xyz"]

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


def parse_param_n_open_files(argv):
    global inputfile
    global outputfile1
    global outputfile11
    global outputfile2
    global outputfile3
    global outputfile31
    global outputfile4
    global outputfile5

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
    outputfile1 = open(re.sub('.txt', '_md_l.txt', newfilename), "w+")
    outputfile11 = open(re.sub('.txt', '_mh_l.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_madrh_l.txt', newfilename), "w+")
    outputfile3 = open(re.sub('.txt', '_md_nl.txt', newfilename), "w+")
    outputfile31 = open(re.sub('.txt', '_mh_nl.txt', newfilename), "w+")
    outputfile4 = open(re.sub('.txt', '_madrh_nl.txt', newfilename), "w+")
    outputfile5 = open(re.sub('.txt', '_madrh_net.txt', newfilename), "w+")

    print "Input File:                                                     ", inputfile.name
    print "Every Message Delay (Loved):                                    ", outputfile1.name
    print "Every Message Hops Travelled (Loved):                           ", outputfile11.name
    print "Per Node Average Delays, Ratios and Hops Travelled (Loved):     ", outputfile2.name
    print "Every Message Delay (non-Loved):                                ", outputfile3.name
    print "Every Message Hops Travelled (non-Loved):                       ", outputfile31.name
    print "Per Node Average Delays, Ratios and Hops Travelled (non-Loved): ", outputfile4.name
    print "Network Average Delay, Ratio and Travelled Hops (Network-wide): ", outputfile5.name


def extract_from_log_and_compute():
    global inputfile
    
    global nodes
    global all_events
    
    for line in inputfile:
        if "INFO" in line and "KHeraldApp" in line and ">!<NE>!<" in line:
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


def print_common_headers():
    global outputfile5

    outputfile5.write("# type >!< network average delay >!< network total >!< network received >!< network delivery ratio >!< network hops travelled \n")


def print_loved_event_values():
    global outputfile1
    global outputfile11
    global outputfile2
    global outputfile5

    global nodes
    global all_events
    global excluded_nodes


    outputfile1.write("# seq >!< node name >!< loved data name >!< delay \n")
    outputfile11.write("# seq >!< node name >!< loved data name >!< hops travelled \n")
    outputfile2.write("# seq >!< node name >!< average delay >!< total loved >!< loved received >!< delivery ratio >!< average hops travelled \n")
    
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
        if any(node.node_name in ex_node_name for ex_node_name in excluded_nodes):
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
                outputfile1.write(str(msg_delay_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.delay) + "\n")
            if event.travelled_hops > -1:
                travelled_hops_acc += event.travelled_hops
                travelled_hops_seq += 1
                outputfile11.write(str(travelled_hops_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.travelled_hops) + "\n")
                
        if node.loved_events_received > 0:
            avg_delay = float(delay_acc) / node.loved_events_received
            delivery_ratio = float(node.loved_events_received) / event_count
            avg_travelled_hops = float(travelled_hops_acc) / node.loved_events_received
            
        node_delay_ratio_seq += 1
        outputfile2.write(str(node_delay_ratio_seq) + " >!< " + node.node_name + " >!< " +  str(avg_delay) + " >!< " + \
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
    
    outputfile5.write(" loved >!< " + str(network_avg_delay) + " >!< " +  str(total_events) + " >!< " + \
                                str(total_events_received) + " >!< " + str(network_delivery_ratio) + " >!< " + str(network_avg_travelled_hops) + "\n")


def print_nonloved_event_values():
    global outputfile3
    global outputfile31
    global outputfile4
    global outputfile5

    global nodes
    global all_events
    global excluded_nodes


    outputfile3.write("# seq >!< node name >!< non-loved data name >!< delay \n")
    outputfile31.write("# seq >!< node name >!< non-loved data name >!< hops travelled \n")
    outputfile4.write("# seq >!< node name >!< average delay >!< total non-loved >!< non-loved received >!< delivery ratio >!< average hops travelled \n")
    
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
        if any(node.node_name in ex_node_name for ex_node_name in excluded_nodes):
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
                outputfile3.write(str(msg_delay_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.delay) + "\n")
            if event.travelled_hops > -1:
                travelled_hops_acc += event.travelled_hops
                travelled_hops_seq += 1
                outputfile31.write(str(travelled_hops_seq) + " >!< " + node.node_name + " >!< " +  event.event_name + " >!< " + str(event.travelled_hops) + "\n")

        if node.nonloved_events_received > 0:
            avg_delay = float(delay_acc) / node.nonloved_events_received
            delivery_ratio = float(node.nonloved_events_received) / event_count
            avg_travelled_hops = float(travelled_hops_acc) / node.nonloved_events_received

        node_delay_ratio_seq += 1
        outputfile4.write(str(node_delay_ratio_seq) + " >!< " + node.node_name + " >!< " +  str(avg_delay) + " >!< " + \
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
    
    outputfile5.write(" non-loved >!< " + str(network_avg_delay) + " >!< " +  str(total_events) + " >!< " + \
                                str(total_events_received) + " >!< " + str(network_delivery_ratio) + " >!< " + str(network_avg_travelled_hops) + "\n")


def close_files():
    global inputfile
    global outputfile1
    global outputfile11
    global outputfile2
    global outputfile3
    global outputfile31
    global outputfile4
    global outputfile5

    inputfile.close()
    outputfile1.close()
    outputfile11.close()
    outputfile2.close()
    outputfile3.close()
    outputfile31.close()
    outputfile4.close()
    outputfile5.close()


def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log_and_compute()
    print_common_headers()
    print_loved_event_values()
    print_nonloved_event_values()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

