#!/usr/bin/python
# Script to extract and process all data related entries from the  
# OMNeT simulator log to compute the traffic spread over the 
# network (i.e., CoV).
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 29-aug-2016

import sys, getopt, re, tempfile, math

inputfile = 0
tempfile1 = 0
outputfile1 = 0
outputfile1mat1 = 0
outputfile1mat2 = 0

nodes = []

total = 0

class Node:

    def __init__(self, name):
        self.name = name
        self.node_sum = 0

def parse_param_n_open_files(argv):
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile1mat1
    global outputfile1mat2
    
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
    outputfile1mat1 = open(re.sub('.txt', '_ts_011.m', newfilename), "w+")
    outputfile1mat2 = open(re.sub('.txt', '_ts_012.m', newfilename), "w+")

    print "Input File:     ", inputfile.name
    print "Temporary File: ", tempfile1.name
    print "Stat File:      ", outputfile1.name

def extract_from_log():
    global inputfile
    global tempfile1
    
    tempfile1.write("# all required tags from log file")
    
    for line in inputfile:
        if "INFO" in line and (":: KKeetchiLayer ::" in line or ":: KRRSLayer ::" in line or ":: KEpidemicRoutingLayer ::" in line) \
            and ":: Lower In ::" in line and (":: Data Msg ::" in line or ":: Feedback Msg ::" in line or ":: Summary Vector Msg ::" in line \
            or ":: Data Request Msg ::" in line):
            tempfile1.write(line)

def compute_stats():
    global nodes
    global total
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

def compute_cov():
    global outputfile1
    global outputfile1mat1
    global outputfile1mat2
    global nodes
    global total

    mean = total / len(nodes)
    deviation_total = 0.0
    for node in nodes:
        deviation_total += pow((node.node_sum - mean), 2.0)
    variance = deviation_total / len(nodes)
    std_deviation = math.sqrt(variance)

    cov = std_deviation / mean

    outputfile1.write("# Total Packets (Data + Feedback + Summary Vector + Data Request) :: Coefficient of Variation \n")
    outputfile1.write(str(total) + " :: " + str(cov) + "\n")
    
    outputfile1mat1.write("figure;\nhold on;\n")
    outputfile1mat1.write("data = [" + str(total) + " 5000];\n")
    outputfile1mat1.write("h = bar(1, data(1));\n")
    outputfile1mat1.write("set(h, \'FaceColor\', [0.0, 0.5, 0.5]);\n")
    outputfile1mat1.write("h = bar(2, data(2));\n")
    outputfile1mat1.write("set(h, \'FaceColor\', [0.5, 0.5, 0.0]);\n")
    outputfile1mat1.write("labels = {\n")
    outputfile1mat1.write("'Scenario 1', 'Scenario 2'")
    outputfile1mat1.write("\n};\n")
    outputfile1mat1.write("xlim([0 3]);\n")
    outputfile1mat1.write("ylim([0 " + str(total + (total // 8)) + "]);\n")
    outputfile1mat1.write("xlabel(\'Scenario\');\n")
    outputfile1mat1.write("ylabel(\'Packets Received by All Nodes (Data + Feedback + Summary Vector + Data Request)\');\n")
    outputfile1mat1.write("set(gca, \'XTick\', 1:2, \'XTickLabel\', labels);\n")
    outputfile1mat1.write("set(gca, \'YTickMode\',\'manual\');\n")
    outputfile1mat1.write("set(gca, \'YTickLabel\', num2str(get(gca, \'YTick\')\'));\n")
    outputfile1mat1.write("grid on;\n")

    outputfile1mat2.write("figure;\nhold on;\n")
    outputfile1mat2.write("data = [" + str(cov) + " 0.1];\n")
    outputfile1mat2.write("h = bar(1, data(1));\n")
    outputfile1mat2.write("set(h, \'FaceColor\', [0.0, 0.5, 0.5]);\n")
    outputfile1mat2.write("h = bar(2, data(2));\n")
    outputfile1mat2.write("set(h, \'FaceColor\', [0.5, 0.5, 0.0]);\n")
    outputfile1mat2.write("labels = {\n")
    outputfile1mat2.write("'Scenario 1', 'Scenario 2'")
    outputfile1mat2.write("\n};\n")
    outputfile1mat2.write("xlim([0 3]);\n")
    outputfile1mat2.write("ylim([0 0.1]);\n")
    outputfile1mat2.write("xlabel(\'Scenario\');\n")
    outputfile1mat2.write("ylabel(\'Coefficient of Variation (Packets Received) \');\n")
    outputfile1mat2.write("set(gca, \'XTick\', 1:2, \'XTickLabel\', labels);\n")
    outputfile1mat2.write("set(gca, \'YTickMode\',\'manual\');\n")
    outputfile1mat2.write("set(gca, \'YTickLabel\', num2str(get(gca, \'YTick\')\'));\n")
    outputfile1mat2.write("grid on;\n")

def close_files():
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile1mat1
    global outputfile1mat2

    inputfile.close()
    tempfile1.close()
    outputfile1.close()
    outputfile1mat1.close()
    outputfile1mat2.close()

def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log()
    compute_stats()
    compute_cov()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])
  
