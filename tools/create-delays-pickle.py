#!/usr/bin/env python
# Script to create a pickle file out of an array of values read from a text file
# The text file is a file with float values per line.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 23-feb-2017

import sys, getopt, re
import pickle


def main(argv):

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print 'create-delays-pickle.py -i <inputfile>'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 'create-delays-pickle.py -i <inputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg

    picklefilename = re.sub('.txt', '.pickle', inputfilename)
    legendname = re.sub('.txt', '', inputfilename)

    inputfile = open(inputfilename, "r")
    picklefile = open(picklefilename, "w+")

    print "Input File:                   ", inputfile.name
    print "Pickle File:                  ", picklefile.name
    print "Label:                        ", legendname

    delayDict = {'delay': {'raw': []},
                 'label': legendname}

    for line in inputfile:
        line = line.strip()
        if line.strip().startswith("#"):
            continue
        val = float(line)
        delayDict["delay"]["raw"].append(val)

    pickle.dump(delayDict, picklefile, protocol=pickle.HIGHEST_PROTOCOL)
    picklefile.close()
    inputfile.close()

    # some test code
    # with open(picklefilename, 'rb') as handle:
    #    dict1copy = pickle.load(handle)
    # picklefile.close()
    # delays = dict1copy["delay"]["raw"]
    # print delays
    # print dict1copy

if __name__ == "__main__":
    main(sys.argv[1:])

