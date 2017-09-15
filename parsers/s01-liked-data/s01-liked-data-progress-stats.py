#!/usr/bin/python
#
# Script to extract and process the progress of liked data over
# the simulation time.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 13-sep-2017

import sys, getopt, re, tempfile

# outputs every 3600 seconds
dump_interval = 3600.0
next_dump_time = dump_interval

inputfile = 0
outputfile1 = 0
outputfile2 = 0

nodes = []

liked_received = 0
nonliked_received = 0
overheads_received = 0

liked_received_bytes = 0
nonliked_received_bytes = 0
overheads_received_bytes = 0

liked_received_total = 0
nonliked_received_total = 0
overheads_received_total = 0

liked_received_bytes_total = 0
nonliked_received_bytes_total = 0
overheads_received_bytes_total = 0


class LineType():
    NE = 0
    DM = 1
    FM = 2
    SVM = 3
    DRM = 4

class Event:

    def __init__(self, data_name, goodness_val):
        self.data_name = data_name
        self.goodness_val = goodness_val

class Node:

    def __init__(self, name):
        self.name = name
        self.events = []


def parse_param_n_open_files(argv):
    global inputfile
    global outputfile1
    global outputfile2

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print "s01-liked-data-progress-stats.py -i <inputfile>"
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's01-liked-data-progress-stats.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg

    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    outputfile1 = open(re.sub('.txt', '_ldp.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_lds.txt', newfilename), "w+")

    print "Input File:               ", inputfile.name
    print "Activity Progress:        ", outputfile1.name
    print "Activity Summary:         ", outputfile2.name


def update_with_line(line_type, sim_time, node_name, data_name, goodness_value, msg_size):
    global dump_interval
    global next_dump_time
    
    global nodes
    
    global liked_received
    global nonliked_received
    global overheads_received
    global liked_received_bytes
    global nonliked_received_bytes
    global overheads_received_bytes

    global liked_received_total
    global nonliked_received_total
    global overheads_received_total
    global liked_received_bytes_total
    global nonliked_received_bytes_total
    global overheads_received_bytes_total

    global outputfile1

    # print "line type=" + str(line_type) + " time=" +  str(sim_time) + " node=" +  node_name + " data=" +  data_name + " gv=" +  str(goodness_value) + " size=" +  str(msg_size)

    # check and print if time to print
    if sim_time > next_dump_time:
        outputfile1.write(str(next_dump_time) + " >!< " + str(liked_received) + " >!< " + str(liked_received_bytes) + \
                                " >!< " + str(nonliked_received) + " >!< " + str(nonliked_received_bytes) + " >!< " + \
                                str(overheads_received) + " >!< " + str(overheads_received_bytes) + "\n")
        
        liked_received_total += liked_received
        nonliked_received_total += nonliked_received
        overheads_received_total += overheads_received
        liked_received_bytes_total += liked_received_bytes
        nonliked_received_bytes_total += nonliked_received_bytes
        overheads_received_bytes_total += overheads_received_bytes
        
        liked_received = 0
        nonliked_received = 0
        overheads_received = 0
        liked_received_bytes = 0
        nonliked_received_bytes = 0
        overheads_received_bytes = 0
        next_dump_time += dump_interval
    
    # accumilate values
    if line_type == LineType.FM or line_type == LineType.SVM or line_type == LineType.DRM:
        overheads_received += 1
        overheads_received_bytes += msg_size
        
    elif line_type == LineType.DM or line_type == LineType.NE:
        # find node
        node_found = False
        for node in nodes:
            if node.name == node_name:
                node_found = True
                break

        # find event
        event_found = False
        if node_found:
            for event in node.events:
                if event.data_name == data_name:
                    event_found = True
                    break
        
        # process each type of line
        if line_type == LineType.NE:
            if not node_found:
                node = Node(node_name)
                nodes.append(node)
                event = Event(data_name, goodness_value)
                node.events.append(event)
            elif node_found and not event_found:
                event = Event(data_name, goodness_value)
                node.events.append(event)
            else:
                print "Same event for the node listed again !!!"
                sys.exit()
    
        elif line_type == LineType.DM:
            if node_found and event_found:
                if event.goodness_val == 100:
                    liked_received += 1
                    liked_received_bytes += msg_size
                else:
                    nonliked_received += 1
                    nonliked_received_bytes += msg_size
            else:
                print "Node or event not found - some inconsistency !!!"
                sys.exit()
        else:
            print "Unknown line type (a) !!!"
            sys.exit()
            
    else:
        print "Unknown line type (b) !!!"
        sys.exit()


def extract_from_log_and_accumilate():
    global inputfile
    global outputfile1
    global outputfile2
    
    global liked_received_total
    global nonliked_received_total
    global overheads_received_total
    global liked_received_bytes_total
    global nonliked_received_bytes_total
    global overheads_received_bytes_total

    outputfile1.write("# sim time >!< liked received >!< liked received bytes >!< " + \
                     " non-liked received >!< non-liked received bytes >!< overheads received >!< overheads received bytes \n")

    for line in inputfile:
        if "INFO" in line and "KHeraldApp" in line and ">!<NLB>!<" in line:
            pass

        elif "INFO" in line and "KHeraldApp" in line and ">!<NE>!<" in line:
            words = line.split(">!<")            
            update_with_line(LineType.NE, float(words[1].strip()), words[2].strip(), words[4].strip(), int(words[7].strip()), 0)

        elif "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and ">!<LI>!<DM>!<" in line:
            words = line.split(">!<")
            update_with_line(LineType.DM, float(words[1].strip()), words[2].strip(), words[8].strip(), 0, int(words[10].strip()))

        elif "INFO" in line and "KKeetchiLayer" in line and ">!<LI>!<FM>!<" in line:
            words = line.split(">!<")            
            update_with_line(LineType.FM, float(words[1].strip()), "", "", 0, int(words[11].strip()))

        elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LI>!<SVM>!<" in line:
            words = line.split(">!<")            
            update_with_line(LineType.SVM, float(words[1].strip()), "", "", 0, int(words[8].strip()))

        elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LI>!<DRM>!<" in line:
            words = line.split(">!<")            
            update_with_line(LineType.DRM, float(words[1].strip()), "", "", 0, int(words[8].strip()))

    outputfile2.write("# total >!< total liked received >!< total liked received bytes >!< " + \
                     " total non-liked received >!< total non-liked received bytes >!< total overheads received >!< total overheads received bytes \n")
    outputfile2.write(" total >!< " + str(liked_received_total) + " >!< " + str(liked_received_bytes_total) + \
                                " >!< " + str(nonliked_received_total) + " >!< " + str(nonliked_received_bytes_total) + " >!< " + \
                                str(overheads_received_total) + " >!< " + str(overheads_received_bytes_total) + "\n")

def close_files():
    global inputfile
    global outputfile1
    global outputfile2

    inputfile.close()
    outputfile1.close()
    outputfile2.close()


def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log_and_accumilate()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

