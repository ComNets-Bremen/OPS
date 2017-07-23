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
outputfile2mat = 0
outputfile3 = 0
outputfile3mat = 0
outputfile4 = 0
outputfile4mat = 0

nodes = []

liked_total = 0
liked_received = 0
nonliked_total = 0
nonliked_received = 0

useful_data_received = 0
additional_data_received = 0
feedback_received = 0
sum_vec_received = 0
data_req_received = 0

class Event:

    def __init__(self, data_name, goodness_val, valid_until):
        self.data_name = data_name
        self.goodness_val = goodness_val
        self.valid_until = valid_until
        self.times_received = 0
        self.first_received_time = 0.0
        self.times_feedback_received = 0

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
        
    def get_name():
        return self.name
        
    def add_event(self, data_name, goodness_val, valid_until):
        found = False
        for event in self.events:
            if event.data_name == data_name:
                found = True
                break
        if found:
            print "Same event for the node listed again !!!"
            sys.exit()

        event = Event(data_name, goodness_val, valid_until)
        self.events.append(event)
            
    def update_event(self, data_name, received_time):        
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

    def update_feedback(self, data_name):        
        found = False
        for event in self.events:
            if event.data_name == data_name:
                found = True
                break
        if not found:
            print "Event not found in list"
            sys.exit()
        
        event.times_feedback_received += 1

    def update_sum_vec_receipt(self): 
        self.sum_vec_received += 1

    def update_data_req_receipt(self): 
        self.data_req_received += 1


def parse_param_n_open_files(argv):
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global outputfile2mat
    global outputfile3
    global outputfile3mat
    global outputfile4
    global outputfile4mat
    
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
    outputfile2mat = open(re.sub('.txt', '_ld_02.m', newfilename), "w+")
    outputfile3 = open(re.sub('.txt', '_ld_03.txt', newfilename), "w+")
    outputfile3mat = open(re.sub('.txt', '_ld_03.m', newfilename), "w+")
    outputfile4 = open(re.sub('.txt', '_ld_04.txt', newfilename), "w+")
    outputfile4mat = open(re.sub('.txt', '_ld_04.m', newfilename), "w+")

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
        if "INFO" in line and (":: KKeetchiLayer ::" in line or\
            ":: KHeraldApp ::" in line or ":: KRRSLayer ::" in line or\
            ":: KEpidemicRoutingLayer ::" in line):
            tempfile1.write(line)

def compute_node_acctivity_summary():
    global tempfile1
    global outputfile1
    global nodes

    tempfile1.seek(0)

    for line in tempfile1:
        if line.strip().startswith("#"):
            continue
            
        elif "KHeraldApp" in line and "Notification List Begin" in line:
            pass

        elif "KHeraldApp" in line and "Notification Entry" in line: 
            words = line.split("::")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if not found:
                node = Node(words[2].strip())
                nodes.append(node)
            node.add_event(words[5].strip(), int(words[6]), float(words[7]))

        elif ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and "Lower In :: Data Msg" in line:
            words = line.split("::")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_event(words[9].strip(), float(words[1].strip()))
            
        elif ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) and "Lower In :: Feedback Msg" in line:
            words = line.split("::")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_feedback(words[9].strip())

        elif "KEpidemicRoutingLayer" in line and "Lower In :: Summary Vector Msg" in line:
            words = line.split("::")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_sum_vec_receipt()          

        elif "KEpidemicRoutingLayer" in line and "Lower In :: Data Request Msg" in line:
            words = line.split("::")
            found = False
            for node in nodes:
                if node.name == words[2].strip():
                    found = True
                    break
            if found:
                node.update_data_req_receipt()          

    outputfile1.write("# node name :: data name :: goodness val :: " + \
                     " valid until :: times data received :: first time received :: times feedback received \n")

    for node in nodes:
        for event in node.events:
            outputfile1.write(node.name + " :: " + event.data_name + " :: " + str(event.goodness_val) + " :: " \
                    + str(event.valid_until) + " :: " + str(event.times_received) + " :: " \
                    + str(event.first_received_time) + " :: " + str(event.times_feedback_received) + "\n")

def compute_like_dislike_summary():
    global outputfile2
    global outputfile2mat
    global outputfile3
    global outputfile3mat
    global outputfile4
    global outputfile4mat
    global nodes
    global liked_total
    global liked_received
    global nonliked_total
    global nonliked_received

    global useful_data_received
    global additional_data_received
    global feedback_received
    global sum_vec_received
    global data_req_received

    for node in nodes:
        sum_vec_received += node.sum_vec_received
        data_req_received += node.data_req_received
        for event in node.events:
            if event.goodness_val >= 75:
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
                additional_data_received += (event.times_received - 1)
            
            feedback_received += event.times_feedback_received
                

    outputfile2.write("# node name :: liked total :: liked received :: " \
                    + " non-liked total :: non-liked received \n")
    for node in nodes:
        outputfile2.write(node.name + " :: " + str(node.liked_total) + " :: " + str(node.liked_received) + " :: " \
                    + str(node.nonliked_total) + " :: " + str(node.nonliked_received) + "\n")
                    
                    
    outputfile2mat.write("figure;\nhold on;\n")
    for node in nodes:
        if node.name == nodes[0].name:
            outputfile2mat.write("data = [\n")
        outputfile2mat.write("         " + str(node.liked_total) + " " + str(node.liked_received) + " " \
                    + str(node.nonliked_total) + " " + str(node.nonliked_received) + "\n")
        if node.name == nodes[-1].name:
            outputfile2mat.write("];\n")
    outputfile2mat.write("h = bar(data, \'group\');\n")
    for index, node in enumerate(nodes):
        if node.name == nodes[0].name:
            outputfile2mat.write("labels = {\n")
        outputfile2mat.write("         " + "\'" + str(index + 1) + "\'")
        if node.name == nodes[-1].name:
            outputfile2mat.write("\n};\n")
        else:
            outputfile2mat.write(",\n")
    outputfile2mat.write("cmap = [0, 0.0, 0.5\n0, 0.0, 1.0\n0.5, 0.0, 0\n1.0, 0.0, 0\n];\n")
    outputfile2mat.write("xlim([0 " + str(len(nodes) + 1) + "]);\n")
    outputfile2mat.write("ylim([0 " + str(len(nodes[0].events)) + "]);\n")
    outputfile2mat.write("xlabel(\'Node Index\');\n")
    outputfile2mat.write("ylabel(\'Notification Count (per node)\');\n")
    outputfile2mat.write("set(gca, \'XTick\', 1:" + str(len(nodes)) + ", \'XTickLabel\', labels);\n")
    outputfile2mat.write("colormap(cmap);\n")
    outputfile2mat.write("grid on;\n")
    outputfile2mat.write("leg = cell(1,4);\n")
    outputfile2mat.write("leg{1} = \'Liked Total\';\nleg{2} = \'Liked Received\';\nleg{3} = \'Non-liked Total\';\nleg{4} = \'Non-liked Received\';\n")
    outputfile2mat.write("legend(h, leg);\n\n")
                    

    outputfile3.write("# all :: liked total :: liked received :: " \
                    + " non-liked total :: non-liked received \n")
    outputfile3.write(" all :: " + str(liked_total) + " :: " + str(liked_received) + " :: " \
                    + str(nonliked_total) + " :: " + str(nonliked_received) + "\n")
                    
    outputfile3mat.write("figure;\nhold on;\n")
    outputfile3mat.write("data = [\n")
    outputfile3mat.write("         " + str(liked_total) + " " + str(liked_received) + " " \
                    + str(nonliked_total) + " " + str(nonliked_received) + "\n")
    outputfile3mat.write("         0 0 0 0\n")
    outputfile3mat.write("];\n")
    outputfile3mat.write("h = bar(data, \'group\');\n")
    outputfile3mat.write("labels = {\n")
    outputfile3mat.write("'Scenario 1', 'Scenario 2'")
    outputfile3mat.write("\n};\n")
    outputfile3mat.write("cmap = [0, 0.0, 0.5\n0, 0.0, 1.0\n0.5, 0.0, 0\n1.0, 0.0, 0\n];\n")
    outputfile3mat.write("xlim([0 4]);\n")
    outputfile3mat.write("ylim([0 " + (str(liked_total + 10) if liked_total > nonliked_total else str(nonliked_total + 10)) + "]);\n")
    outputfile3mat.write("xlabel(\'Scenario\');\n")
    outputfile3mat.write("ylabel(\'Notification Count (network wide)\');\n")
    outputfile3mat.write("set(gca, \'XTick\', 1:2, \'XTickLabel\', labels);\n")
    outputfile3mat.write("colormap(cmap);\n")
    outputfile3mat.write("grid on;\n")
    outputfile3mat.write("leg = cell(1,4);\n")
    outputfile3mat.write("leg{1} = \'Liked Total\';\nleg{2} = \'Liked Received\';\nleg{3} = \'Non-liked Total\';\nleg{4} = \'Non-liked Received\';\n")
    outputfile3mat.write("legend(h, leg);\n\n")
    
    
    outputfile4.write("# all :: total (data + feedback + sum vec + data req) :: useful data :: " \
                    + " additional data :: feedback :: summary vector :: data request \n")
    outputfile4.write(" all :: " + str(useful_data_received + additional_data_received + feedback_received + sum_vec_received + data_req_received) 
                        + " :: " + str(useful_data_received) + " :: " \
                    + str(additional_data_received) + " :: " + str(feedback_received) + " :: " + str(sum_vec_received) + " :: " + str(data_req_received) + " :: " + "\n")

    outputfile4mat.write("figure;\nhold on;\n")
    outputfile4mat.write("data = [\n")
    outputfile4mat.write("         " + str(useful_data_received) + ", " + str(additional_data_received) + ", " \
                    + str(feedback_received) + " \n")
    outputfile4mat.write("         0, 0, 0 \n")
    outputfile4mat.write("];\n")
    outputfile4mat.write("h = bar(data, \'stacked\');\n")
    outputfile4mat.write("cmap = [0.0, 0.0, 0.5 \n");
    outputfile4mat.write("0.0, 0.0, 1.0 \n");
    outputfile4mat.write("0.5, 0.0, 0.0]; \n");
    outputfile4mat.write("for i = 1:3 \n");
    outputfile4mat.write("    set(h(i), 'FaceColor', cmap(i,:)); \n");
    outputfile4mat.write("end \n");
    outputfile4mat.write("labels = {\n")
    outputfile4mat.write("'Scenario 1', 'Scenario 2'")
    outputfile4mat.write("\n};\n")
    outputfile4mat.write("xlim([0 3]);\n")
    outputfile4mat.write("ylim([0 " + (str(useful_data_received + additional_data_received + feedback_received + 1000)) + "]);\n")
    outputfile4mat.write("xlabel(\'Scenario\');\n")
    outputfile4mat.write("ylabel(\'Packets Received\');\n")
    outputfile4mat.write("set(gca, \'XTick\', 1:2, \'XTickLabel\', labels);\n")
    outputfile4mat.write("set(gca, \'YTickMode\',\'manual\');\n")
    outputfile4mat.write("set(gca, \'YTickLabel\', num2str(get(gca, \'YTick\')\'));\n")
    outputfile4mat.write("grid on;\n")
    outputfile4mat.write("leg = cell(1,3);\n")
    outputfile4mat.write("leg{1} = \'Useful Data\';\nleg{2} = \'Additional Data\';\nleg{3} = \'Feedback\';\n")
    outputfile4mat.write("legend(h, leg);\n\n")


def close_files():
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global outputfile2mat
    global outputfile3
    global outputfile3mat

    inputfile.close()
    tempfile1.close()
    outputfile1.close()
    outputfile2.close()
    outputfile2mat.close()
    outputfile3.close()
    outputfile3mat.close()

def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log()
    compute_node_acctivity_summary()
    compute_like_dislike_summary()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])
  
