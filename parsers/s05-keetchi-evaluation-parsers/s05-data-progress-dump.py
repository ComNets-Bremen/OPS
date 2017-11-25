#!/usr/bin/python
#
# Script to show the progress of a given data item
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 12-oct-2017

import sys, getopt, re, tempfile

inputfile = 0
outputfile1 = 0

# popular data item
data_name = "/herald/item-22006"

# # non popular data item
# data_name = "/herald/item-22285"

def parse_param_n_open_files(argv):
    global inputfile
    global outputfile1

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print "s05-data-progress-dump.py -i <inputfile>"
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's05-data-progress-dump.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg

    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    outputfile1 = open(re.sub('.txt', '_dpd.txt', newfilename), "w+")

    print "Input File:          ", inputfile.name
    print "Data Progress Dump:  ", outputfile1.name


def extract_from_log_and_dump():
    global inputfile
    global outputfile1

    outputfile1.write("# all messages related to data item - " + data_name + " \n")

    for line in inputfile:


        if "INFO" in line and "KHeraldApp" in line and ">!<NE>!<" in line and data_name in line and ">!<L>!<" in line:
            words = line.split(">!<")
            outputfile1.write("Node " + words[2].strip() + " loves " + data_name + " \n")
            

        if "INFO" in line and ("KKeetchiLayer" in line or "KRRSLayer" in line or "KEpidemicRoutingLayer" in line) \
                and data_name in line:
            if "KKeetchiLayer" in line and "<LO>!<DM>" in line:
                words = line.split(">!<")
                outputfile1.write("\nSending Data " + words[1].strip() + " " + words[2].strip() + " \n")
                outputfile1.write("    From:" + words[3].strip() + " To:" + words[7].strip() + " \n")
                outputfile1.write("    GV:" + words[9].strip() + " \n")
            elif "KKeetchiLayer" in line and "<LI>!<DM>" in line:
                words = line.split(">!<")
                outputfile1.write("\nReceived Data " + words[1].strip() + " " + words[2].strip() + " \n")
                outputfile1.write("    At:" + words[3].strip() + " From:" + words[6].strip() + " \n")
                outputfile1.write("    GV:" + words[9].strip() + " \n")
            elif "KKeetchiLayer" in line and "<LO>!<FM>" in line:
                words = line.split(">!<")
                outputfile1.write("\nSending Feedback " + words[1].strip() + " " + words[2].strip() + " \n")
                outputfile1.write("    From:" + words[3].strip() + " To:" + words[7].strip() + " \n")
                outputfile1.write("    GV:" + words[9].strip() + " \n")
            elif "KKeetchiLayer" in line and "<LI>!<FM>" in line:
                words = line.split(">!<")
                outputfile1.write("\nReceived Feedback " + words[1].strip() + " " + words[2].strip() + " \n")
                outputfile1.write("    At:" + words[3].strip() + " From:" + words[6].strip() + " \n")
                outputfile1.write("    GV:" + words[9].strip() + " \n")
            
                

def close_files():
    global inputfile
    global outputfile1

    inputfile.close()
    outputfile1.close()


def main(argv):
    parse_param_n_open_files(argv)
    extract_from_log_and_dump()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

