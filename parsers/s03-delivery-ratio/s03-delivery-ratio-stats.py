#!/usr/bin/python
# Script to extract and process all data related entries from the
# OMNeT simulator log to compute the following statistics.
#
# - delivery ratio of data - with the Promote App
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 05-dec-2016

import sys, getopt, re, tempfile

inputfile = 0
tempfile1 = 0
outputfile1 = 0
outputfile2 = 0

data_generated = 0
data_received = 0
accumilated_delay = 0.0

data_items = []

max_sim_time = 0.0

class Data_Item:
    def __init__(self, data_name, create_time):
        self.data_name = data_name
        self.creation_time = create_time
        self.received_count = 0

def parse_param_n_open_files(argv):
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global max_sim_time

    try:
        opts, args = getopt.getopt(argv,"hi:m:",["ifile=", "maxtime="])
    except getopt.GetoptError:
        print 's03-delivery-ratio.py -i <inputfile> -m <max sim time>'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 's03-delivery-ratio.py -i <inputfile> -m <max sim time>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg
        elif opt in ("-m", "--maxtime"):
            max_sim_time = float(arg)

    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    inputfile = open(inputfilename, "r")
    tempfile1 = tempfile.TemporaryFile(dir='.')
    outputfile1 = open(re.sub('.txt', '_dr_01.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_dr_02.txt', newfilename), "w+")

    print "Input File:                   ", inputfile.name
    print "Temporary File:               ", tempfile1.name
    print "Delivery Ratio:               ", outputfile1.name

def extract_from_log():
    global inputfile
    global tempfile1
    global max_sim_time

    tempfile1.write("# all required tags from log file\n")

    for line in inputfile:
        if "INFO" in line and (":: At Source ::" in line or\
            ":: At Destination ::" in line):
            words = line.split("::")
            sim_time = float(words[1].strip())
            if max_sim_time == 0.0 or (max_sim_time > 0.0 and sim_time <= max_sim_time):
                tempfile1.write(line)

def accumulate_and_show_delivery_data():
    global tempfile1
    global outputfile1
    global outputfile2
    global data_items
    global data_generated
    global data_received
    global accumilated_delay

    outputfile1.write("# data generated :: data received  :: average data delay \n")
    outputfile2.write("# delay \n")

    tempfile1.seek(0)

    for line in tempfile1:
        if line.strip().startswith("#"):
            continue

        if ":: At Source ::" in line:
            words = line.split("::")
            found = False
            for data_item in data_items:
                if data_item.data_name == words[5].strip():
                    found = True
                    break
            if not found:
                data_item = Data_Item(words[5].strip(), words[1].strip())
                data_items.append(data_item)
                data_generated += 1
            else:
                print 'More than once generated data ' + words[5].strip()
                sys.exit(2)


        elif ":: At Destination ::" in line:
            words = line.split("::")
            found = False
            for data_item in data_items:
                if data_item.data_name == words[5].strip():
                    found = True
                    break
            if found:
                data_item.received_count += 1
                if data_item.received_count == 1:
                    data_received += 1
                    data_delay = float(words[1].strip()) - float(data_item.creation_time)
                    accumilated_delay += data_delay
                    outputfile2.write(str(data_delay) + "\n")
            else:
                print 'Non generated data ' + words[5].strip()
                sys.exit(2)
        else:
            print "Unknown line - " + line

    outputfile1.write(str(data_generated) + " :: " + str(data_received) + " :: " + str(accumilated_delay / data_received) + "\n")

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
    accumulate_and_show_delivery_data()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])

