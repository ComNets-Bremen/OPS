#!/usr/bin/python
#
# Script to extract and process sent packets per node
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 17-sep-2017

import sys, getopt, re, tempfile

inputfile = 0
outputfile1 = 0
outputfile2 = 0

nodes = []
excluded_nodes = ["abc", "xyz"]

total_data_sent = 0
total_data_sent_bytes = 0
total_feedback_sent = 0
total_feedback_sent_bytes = 0
total_svm_sent = 0
total_svm_sent_bytes = 0
total_drm_sent = 0
total_drm_sent_bytes = 0

class Node:

    def __init__(self, name):
        self.name = name
        self.data_sent = 0
        self.data_sent_bytes = 0
        self.feedback_sent = 0
        self.feedback_sent_bytes = 0
        self.svm_sent = 0
        self.svm_sent_bytes = 0
        self.drm_sent = 0
        self.drm_sent_bytes = 0

def parse_param_n_open_files(argv):
    global inputfile
    global outputfile1
    global outputfile2

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print "s05-packets-sent-per-node-stats.py -i <inputfile>"
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's05-packets-sent-per-node-stats.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg

    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    outputfile1 = open(re.sub('.txt', '_ps.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_pst.txt', newfilename), "w+")

    print "Input File:                ", inputfile.name
    print "Packets Sent Per Node:     ", outputfile1.name
    print "Packets Sent by All Nodes: ", outputfile2.name

def get_node(node_name):
    global nodes
    
    node_found = False
    for node in nodes:
        if node.name == node_name:
            node_found = True
            break

    if not node_found:
        node = Node(node_name)
        nodes.append(node)

    return(node)

def extract_from_log_and_accumilate():
    global inputfile
    
    global total_data_sent
    global total_data_sent_bytes
    global total_feedback_sent
    global total_feedback_sent_bytes
    global total_svm_sent
    global total_svm_sent_bytes
    global total_drm_sent
    global total_drm_sent_bytes

    global nodes
    
    for line in inputfile:
        if "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line) and ">!<LO>!<DM>!<" in line:
            words = line.split(">!<")
            node = get_node(words[2].strip())
            total_data_sent += 1
            total_data_sent_bytes += int(words[10].strip())
            node.data_sent += 1
            node.data_sent_bytes += int(words[10].strip())

        elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LO>!<DM>!<" in line:
            words = line.split(">!<")
            node = get_node(words[2].strip())
            total_data_sent += 1
            total_data_sent_bytes += int(words[8].strip())
            node.data_sent += 1
            node.data_sent_bytes += int(words[8].strip())
            
        elif "INFO" in line and "KKeetchiLayer" in line and ">!<LO>!<FM>!<" in line:
            words = line.split(">!<")
            node = get_node(words[2].strip())
            total_feedback_sent += 1
            total_feedback_sent_bytes += int(words[11].strip())
            node.feedback_sent += 1
            node.feedback_sent_bytes += int(words[11].strip())

        elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LO>!<SVM>!<" in line:
            words = line.split(">!<")
            node = get_node(words[2].strip())
            total_svm_sent += 1
            total_svm_sent_bytes += int(words[10].strip())
            node.svm_sent += 1
            node.svm_sent_bytes += int(words[10].strip())

        elif "INFO" in line and "KEpidemicRoutingLayer" in line and ">!<LO>!<DRM>!<" in line:
            words = line.split(">!<")
            node = get_node(words[2].strip())
            total_drm_sent += 1
            total_drm_sent_bytes += int(words[8].strip())
            node.drm_sent += 1
            node.drm_sent_bytes += int(words[8].strip())

def print_values():
    outputfile1.write("# seq >!< node name >!< total sent count >!< total sent bytes >!< " + \
                     " data sent count >!< data sent bytes >!< feedback sent count >!< feedback sent bytes >!<" + \
                     " svm send count >!< svm sent bytes >!< drm sent count >!< drm sent bytes \n")
    i = 0
    for node in nodes:
        if any(node.name in ex_node_name for ex_node_name in excluded_nodes):
            continue
        
        i += 1
        total_node_sent = node.data_sent + node.feedback_sent + node.svm_sent + node.drm_sent
        total_node_sent_bytes = node.data_sent_bytes + node.feedback_sent_bytes + node.svm_sent_bytes + node.drm_sent_bytes

        outputfile1.write(str(i) + " >!< " + node.name + " >!< " +  str(total_node_sent) + " >!< " + str(total_node_sent_bytes) + " >!< " +\
                            str(node.data_sent) + " >!< " + str(node.data_sent_bytes) + " >!< " +\
                            str(node.feedback_sent) + " >!< " + str(node.feedback_sent_bytes) + " >!< " +\
                            str(node.svm_sent) + " >!< " + str(node.svm_sent_bytes) + " >!< " +\
                            str(node.drm_sent) + " >!< " + str(node.drm_sent_bytes) + "\n")
    
    total_sent = total_data_sent + total_feedback_sent + total_svm_sent + total_drm_sent
    total_sent_bytes = total_data_sent_bytes + total_feedback_sent_bytes + total_svm_sent_bytes + total_drm_sent_bytes

    outputfile2.write("# totals >!< total sent count >!< total sent bytes >!< " + \
                     " data sent count >!< data sent bytes >!< feedback sent count >!< feedback sent bytes >!<" + \
                     " svm send count >!< svm sent bytes >!< drm sent count >!< drm sent bytes \n")
    outputfile2.write(" totals >!< " +  str(total_sent) + " >!< " + str(total_sent_bytes) + " >!< " +\
                            str(total_data_sent) + " >!< " + str(total_data_sent_bytes) + " >!< " + \
                            str(total_feedback_sent) + " >!< " + str(total_feedback_sent_bytes) + " >!< " +\
                            str(total_svm_sent) + " >!< " + str(total_svm_sent_bytes) + " >!< " +\
                            str(total_drm_sent) + " >!< " + str(total_drm_sent_bytes) + "\n")

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
    print_values()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

