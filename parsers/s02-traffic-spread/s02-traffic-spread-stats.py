#!/usr/bin/python
# Script to extract and process all sent packets from the
# OMNeT simulator log to compute the traffic spread over the
# network (i.e., CoV) and totals.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 29-aug-2016

import sys, getopt, re, tempfile, math

inputfile = 0
tempfile1 = 0
outputfile1 = 0
outputfile2 = 0

nodes = []

total = 0
total_data = 0
total_feedback = 0
total_sum_vec = 0
total_data_req = 0

class Node:

    def __init__(self, name):
        self.name = name
        self.node_sum = 0
        self.node_data = 0
        self.node_feedback = 0
        self.node_sum_vec = 0
        self.node_data_req = 0

def parse_param_n_open_files(argv):
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print 's01-traffic-spread.py -i <inputfile>'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's01-traffic-spread.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg
    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    tempfile1 = tempfile.TemporaryFile(dir='.')
    outputfile1 = open(re.sub('.txt', '_ts_01.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_ts_02.txt', newfilename), "w+")

    print "Input File:     ", inputfile.name
    print "Temporary File: ", tempfile1.name
    print "Stat File 1:    ", outputfile1.name
    print "Stat File 2:    ", outputfile2.name

def extract_from_log():
    global inputfile
    global tempfile1

    tempfile1.write("# all required tags from log file")

    for line in inputfile:
        if "INFO" in line and (":: KKeetchiLayer ::" in line or ":: KRRSLayer ::" in line or ":: KEpidemicRoutingLayer ::" in line)\
            and ":: Lower Out ::" in line and (":: Data Msg ::" in line or ":: Feedback Msg ::" in line or ":: Summary Vector Msg ::" in line\
            or ":: Data Request Msg ::" in line):
            tempfile1.write(line)

def compute_stats():
    global nodes
    global total
    global total_data
    global total_feedback
    global total_sum_vec
    global total_data_req
    global tempfile1

    tempfile1.seek(0)

    for line in tempfile1:
        if line.strip().startswith("#"):
            continue

        words = line.split("::")

        found = False
        for node in nodes:
            if node.name == words[2].strip():
                found = True
                break
        if not found:
            node = Node(words[2].strip())
            nodes.append(node)

        node.node_sum += 1
        total += 1

        if ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and "Lower Out :: Data Msg" in line:
            node.node_data += 1
            total_data += 1

        elif ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and "Lower Out :: Feedback Msg" in line:
            node.node_feedback += 1
            total_feedback += 1

        elif "KEpidemicRoutingLayer" in line and "Lower Out :: Summary Vector Msg" in line:
            node.node_sum_vec += 1
            total_sum_vec += 1

        elif "KEpidemicRoutingLayer" in line and "Lower Out :: Data Request Msg" in line:
            node.node_data_req += 1
            total_data_req += 1

def compute_cov():
    global outputfile1
    global nodes
    global total

    mean = total / len(nodes)
    deviation_total = 0.0
    for node in nodes:
        deviation_total += pow((node.node_sum - mean), 2.0)
    variance = deviation_total / len(nodes)
    std_deviation = math.sqrt(variance)

    cov = std_deviation / mean

    outputfile1.write("# Total Packets Sent (Data + Feedback + Summary Vector + Data Request) :: Coefficient of Variation \n")
    outputfile1.write(str(total) + " :: " + str(cov) + "\n")

def compute_breakup():
    global outputfile2
    global total
    global total_data
    global total_feedback
    global total_sum_vec
    global total_data_req

    outputfile2.write("# Total Packets Sent :: Total Data :: Total Feedback :: Total Summary Vector ::  Total Data Request \n")
    outputfile2.write(str(total) + " :: " + str(total_data) + " :: " + str(total_feedback) + " :: " + str(total_sum_vec) + " :: " + str(total_data_req) + "\n")

def close_files():
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2

    inputfile.close()
    tempfile1.close()
    outputfile1.close()
    outputfile2.close()

def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log()
    compute_stats()
    compute_cov()
    compute_breakup()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

