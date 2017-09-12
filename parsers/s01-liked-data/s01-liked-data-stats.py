#!/usr/bin/python
# Script to extract and process all data related entries from the
# OMNeT simulator log to compute the following statistics.
#
# - node activity summary - data received
# - node summary of liked/disliked data received
# - network wide summary of liked/disliked data received
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 29-aug-2016

import sys, getopt, re, tempfile

inputfile = 0
tempfile1 = 0
outputfile1 = 0
outputfile2 = 0
outputfile3 = 0
outputfile4 = 0

nodes = []

liked_total = 0
liked_received = 0
nonliked_total = 0
nonliked_received = 0

useful_data_received = 0
useful_data_received_bytes = 0
additional_data_received = 0
additional_data_received_bytes = 0
feedback_received = 0
feedback_received_bytes = 0
sum_vec_received = 0
sum_vec_received_bytes = 0
data_req_received = 0
data_req_received_bytes = 0

class Event:

    def __init__(self, data_name, popularity, likeness, goodness_val, valid_until):
        self.data_name = data_name
        self.popularity = popularity
        self.likeness = likeness
        self.goodness_val = goodness_val
        self.valid_until = valid_until
        self.times_received = 0
        self.first_received_time = 0.0
        self.times_feedback_received = 0
        self.data_size_tot_first = 0
        self.data_size_tot_rest = 0
        self.feedback_size_tot = 0

class Node:

    def __init__(self, name):
        self.name = name
        self.events = []
        self.liked_total = 0
        self.liked_received = 0
        self.nonliked_total = 0
        self.nonliked_received = 0
        self.sum_vec_received = 0
        self.data_req_received = 0
        self.sum_vec_size_tot = 0
        self.data_req_size_tot = 0

    def get_name():
        return self.name

    def add_event(self, data_name, popularity, likeness, goodness_val, valid_until):
        found = False
        for event in self.events:
            if event.data_name == data_name:
                found = True
                break
        if found:
            print "Same event for the node listed again !!!"
            sys.exit()

        event = Event(data_name, popularity, likeness, goodness_val, valid_until)
        self.events.append(event)

    def update_event(self, data_name, received_time, data_size):
        found = False
        for event in self.events:
            if event.data_name == data_name:
                found = True
                break
        if not found:
            print "Event not found in list"
            sys.exit()

        event.times_received += 1
        if event.first_received_time == 0.0:
            event.first_received_time = received_time
            event.data_size_tot_first += data_size
        else:
            event.data_size_tot_rest += data_size

    def update_feedback(self, data_name, feedback_size):
        found = False
        for event in self.events:
            if event.data_name == data_name:
                found = True
                break
        if not found:
            print "Event not found in list"
            sys.exit()

        event.times_feedback_received += 1
        event.feedback_size_tot += feedback_size

    def update_sum_vec_receipt(self, sum_vec_size):
        self.sum_vec_received += 1
        self.sum_vec_size_tot += sum_vec_size

    def update_data_req_receipt(self, data_req_size):
        self.data_req_received += 1
        self.data_req_size_tot += data_req_size


def parse_param_n_open_files(argv):
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global outputfile3
    global outputfile4

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print 's01-liked-data.py -i <inputfile>'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's01-liked-data.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg

    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    tempfile1 = tempfile.TemporaryFile(dir='.')
    outputfile1 = open(re.sub('.txt', '_ld_01.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_ld_02.txt', newfilename), "w+")
    outputfile3 = open(re.sub('.txt', '_ld_03.txt', newfilename), "w+")
    outputfile4 = open(re.sub('.txt', '_ld_04.txt', newfilename), "w+")

    print "Input File:                   ", inputfile.name
    print "Temporary File:               ", tempfile1.name
    print "Node Activity Summary:        ", outputfile1.name
    print "Node Like/Dislike Summary:    ", outputfile2.name
    print "Overall Like/Dislike Summary: ", outputfile3.name
    print "Total Receive Summary:        ", outputfile4.name

def extract_from_log():
    global inputfile
    global tempfile1

    tempfile1.write("# all required tags from log file")

    for line in inputfile:
        if "INFO" in line and\
            ("KKeetchiLayer" in line or\
             "KRRSLayer" in line or\
             "KEpidemicRoutingLayer" in line or\
             "KHeraldApp" in line or\
             "KPromoteApp" in line or\
             "KBruitApp" in line):
            tempfile1.write(line)

def compute_node_acctivity_summary():
    global tempfile1
    global outputfile1
    global nodes

    tempfile1.seek(0)

    for line in tempfile1:
        if line.strip().startswith("#"):
            continue

        elif "KHeraldApp" in line and ">!<NLB>!<" in line:
            pass

        elif "KHeraldApp" in line and ">!<NE>!<" in line:
            words = line.split(">!<")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if not found:
                node = Node(words[2].strip())
                nodes.append(node)
            node.add_event(words[4].strip(), int(words[5]), int(words[6]), int(words[7]), float(words[9]))

        elif ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and ">!<LI>!<DM>!<" in line:
            words = line.split(">!<")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_event(words[8].strip(), float(words[1].strip()), int(words[10].strip()))

        elif ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and ">!<LI>!<FM>!<" in line:
            words = line.split(">!<")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_feedback(words[8].strip(), int(words[11].strip()))

        elif "KEpidemicRoutingLayer" in line and ">!<LI>!<SVM>!<" in line:
            words = line.split(">!<")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_sum_vec_receipt(int(words[8].strip()))

        elif "KEpidemicRoutingLayer" in line and ">!<LI>!<DRM>!<" in line:
            words = line.split(">!<")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_data_req_receipt(int(words[8].strip()))

    outputfile1.write("# node name >!< data name >!< goodness val >!< " + \
                     " valid until >!< times data received >!< first time received >!< times feedback received \n")

    for node in nodes:
        for event in node.events:
            outputfile1.write(node.name + " >!< " + event.data_name + " >!< " + str(event.goodness_val) + " >!< " \
                    + str(event.valid_until) + " >!< " + str(event.times_received) + " >!< " \
                    + str(event.first_received_time) + " >!< " + str(event.times_feedback_received) + "\n")

def compute_like_dislike_summary():
    global outputfile2
    global outputfile3
    global outputfile4
    global nodes
    global liked_total
    global liked_received
    global nonliked_total
    global nonliked_received

    global useful_data_received
    global useful_data_received_bytes
    global additional_data_received
    global additional_data_received_bytes
    global feedback_received
    global feedback_received_bytes
    global sum_vec_received
    global sum_vec_received_bytes
    global data_req_received
    global data_req_received_bytes

    for node in nodes:
        sum_vec_received += node.sum_vec_received
        sum_vec_received_bytes += node.sum_vec_size_tot
        data_req_received += node.data_req_received
        data_req_received_bytes += node.data_req_size_tot

        for event in node.events:
            if event.goodness_val == 100:
                node.liked_total += 1
                liked_total += 1
                if event.times_received > 0:
                    node.liked_received += 1
                    liked_received += 1
            else:
                node.nonliked_total += 1
                nonliked_total += 1
                if event.times_received > 0:
                    node.nonliked_received += 1
                    nonliked_received += 1

            if event.times_received > 0:
                useful_data_received += 1
                useful_data_received_bytes += event.data_size_tot_first
                additional_data_received += (event.times_received - 1)
                additional_data_received_bytes += event.data_size_tot_rest

            feedback_received += event.times_feedback_received
            feedback_received_bytes += event.feedback_size_tot

    outputfile2.write("# node name >!< liked total >!< liked received >!< " \
                    + " non-liked total >!< non-liked received \n")
    for node in nodes:
        outputfile2.write(node.name + " >!< " + str(node.liked_total) + " >!< " + str(node.liked_received) + " >!< " \
                    + str(node.nonliked_total) + " >!< " + str(node.nonliked_received) + "\n")

    outputfile3.write("# all >!< liked total >!< liked received >!< " \
                    + " non-liked total >!< non-liked received \n")
    outputfile3.write(" all >!< " + str(liked_total) + " >!< " + str(liked_received) + " >!< " \
                    + str(nonliked_total) + " >!< " + str(nonliked_received) + "\n")

    outputfile4.write("# all >!< total count >!< total bytes >!< useful data count >!< useful data bytes >!< " \
                    + " additional data count >!< additional data bytes >!< feedback count >!< feedback bytes >!<" \
                    + " summary vector count >!< summary vector bytes >!< data request count >!< data request bytes \n")

    total_count = useful_data_received + additional_data_received + feedback_received + sum_vec_received + data_req_received
    total_bytes = useful_data_received_bytes + additional_data_received_bytes + feedback_received_bytes + sum_vec_received_bytes + data_req_received_bytes
    outputfile4.write(" all >!< " + str(total_count) + " >!< " + str(total_bytes) + " >!< " \
                            + str(useful_data_received) + " >!< " + str(useful_data_received_bytes) + " >!< " \
                            + str(additional_data_received) + " >!< " + str(additional_data_received_bytes) + " >!< " \
                            + str(feedback_received) + " >!< " + str(feedback_received_bytes) + " >!< " \
                            + str(sum_vec_received) + " >!< " + str(sum_vec_received_bytes) + " >!< " \
                            + str(data_req_received) + " >!< " + str(data_req_received_bytes) + "\n")

def close_files():
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global outputfile3

    inputfile.close()
    tempfile1.close()
    outputfile1.close()
    outputfile2.close()
    outputfile3.close()

def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log()
    compute_node_acctivity_summary()
    compute_like_dislike_summary()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

