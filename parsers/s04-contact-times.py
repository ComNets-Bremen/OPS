#!/usr/bin/python
# Script to extract contact related data from the log. 
#
# - contact summary
# - periodic contact summary (based on the value of -p)
# - overall contact summary
#
# syntax:
#  s04-contact-times.py -i sfo.txt -s 2071530.0 -p 86400.0
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 10-mar-2017

import sys, getopt, re, tempfile

wireless_resolution_interval = 1.0 # seconds
interval_period = 10.0 # seconds
sim_time = 3600.0 # seconds
inputfile = 0
tempfile1 = 0
outputfile1 = 0
outputfile2 = 0
outputfile3 = 0

nodes = []

total_contacts = 0

class Contact:
    # holds the data of a single contact of a given node
    def __init__(self, contact_node_name, contact_node_mac):
        self.contact_node_name = contact_node_name
        self.contact_node_mac = contact_node_mac
        self.start_time = 0.0
        self.last_seen_time = 0.0


class Node:
    # holds node information
    def __init__(self, node_name, node_mac):
        self.node_name = node_name
        self.node_mac = node_mac
        self.contacts = []


def parse_param_n_open_files(argv):
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global outputfile3
    global interval_period
    global sim_time
    global wireless_resolution_interval
    
    # check parameters syntax
    try:
        opts, args = getopt.getopt(argv,"hi:s:p:r:",["ifile=", "simtime=", "period=", "resolution="])
    except getopt.GetoptError:
        print 's04-contact-times.py -i <inputfile> -s <sim time> -p <period> -r <resolution>'
        sys.exit(2)

    # read all parameters in 
    for opt, arg in opts:
        if opt == '-h':
            print 's04-contact-times.py -i <inputfile> -s <sim time> -p <period> -r <resolution>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg
        elif opt in ("-s", "--simtime"):
            sim_time = float(arg)
        elif opt in ("-p", "--period"):
            interval_period = float(arg)
        elif opt in ("-r", "--resolution"):
            wireless_resolution_interval = float(arg)
    
    # remove undesirable characters from the file names
    newfilename = re.sub(':', '_', inputfilename)
    newfilename = re.sub('-', '_', newfilename)

    # open files - dsome have to be created
    inputfile = open(inputfilename, "r")
    tempfile1 = tempfile.TemporaryFile(dir='.')
    outputfile1 = open(re.sub('.txt', '_ct_01.txt', newfilename), "w+")
    outputfile2 = open(re.sub('.txt', '_ct_02.txt', newfilename), "w+")
    outputfile3 = open(re.sub('.txt', '_ct_03.txt', newfilename), "w+")

    # show the file names
    print "Input File:                    ", inputfile.name
    print "Temporary File:                ", tempfile1.name
    print "Contacts List:                 ", outputfile1.name
    print "Periodic Contact Summary:      ", outputfile2.name
    print "Overall Contact Summary:       ", outputfile2.name
    
    # write the initial line of output files - with a # in front (as a comment line)
    outputfile1.write("# node name :: node mac :: start time :: end time :: duration :: contact node name :: contact node mac \n")
    outputfile2.write("# start time period    time period end    contact count    period number \n")
    outputfile3.write("# total contacts :: average contact time \n")


def extract_from_log():
    global inputfile
    global tempfile1
    
    # extract all the relevant lines from the original log file     
    for line in inputfile:
        if "INFO" in line and ":: KWirelessInterface ::" in line:
            tempfile1.write(line)


def extract_contact_times():
    global tempfile1
    global outputfile1
    global nodes
    global wireless_resolution_interval

    while True:
        
        # start from the begining of the extracted file
        tempfile1.seek(0)
        
        current_node = 0
        
        # look for a node already not considered
        more_nodes = False
        for line in tempfile1:
            words = line.split("::")

            found = False
            for node in nodes:
                if node.node_name == words[2].strip():
                    found = True
                    break
            if not found:
                node = Node(words[2].strip(), words[4].strip())
                nodes.append(node)
                current_node = node
                more_nodes = True
                break
                
        # if all nodes considered, then exit
        if not more_nodes:
            break 

        tempfile1.seek(0)
        for line in tempfile1:
            words = line.split("::")
            current_simulation_time = float(words[1].strip())
            
            if current_node.node_name == words[2].strip():

                # update (create) connection
                found = False
                for contact in current_node.contacts:
                    if contact.contact_node_name == words[6].strip():
                        found = True
                        break
                if not found:
                    contact = Contact(words[6].strip(), words[5].strip())
                    contact.start_time = current_simulation_time
                    current_node.contacts.append(contact)
            
                contact.last_seen_time = current_simulation_time
        
            # remove and print completed contacts
            removed = True
            while removed:
                removed = False
                for contact in current_node.contacts:
                    if current_simulation_time > (contact.last_seen_time + wireless_resolution_interval):
                        contact_duration = (contact.last_seen_time + wireless_resolution_interval) - contact.start_time
                        outputfile1.write(current_node.node_name + " :: " + current_node.node_mac + " :: " \
                                            + str(contact.start_time) + " :: " + str(contact.last_seen_time + wireless_resolution_interval) + " :: " \
                                            + str(contact_duration) + " :: " + contact.contact_node_name + " :: " + contact.contact_node_mac + "\n")
                        current_node.contacts.remove(contact)
                        removed = True
                        break


def dump_periodic_summary():
    global outputfile1
    global outputfile2
    global outputfile3
    global nodes
    global interval_period
    global sim_time

    # # find maximum simulation time
    # tempfile1.seek(0)
    # max_sim_time = 0.0
    # for line in tempfile1:
    #     words = line.split("::")
    #     max_sim_time = float(words[1].strip())
    
    # identify the upper limit of the final (last) periods
    max_sim_time = sim_time
    remainder = max_sim_time % interval_period
    if remainder > 0.0:
        max_sim_time = max_sim_time - remainder + interval_period
    
    # initialize counters
    total_contact_count = 0
    total_contact_time = 0.0

    # set initial period
    period_number = 1
    lower_sim_limit = 0.0
    upper_sim_limit = interval_period
    
    # accumulate contacts for the periods until the last period is reached 
    while upper_sim_limit <= max_sim_time:
        outputfile1.seek(0)
        contact_count = 0
        
        # accumilate counts relevant to the current period
        for line in outputfile1:
            if line.strip().startswith("#"):
                continue
            
            words = line.split("::")
            if lower_sim_limit == 0.0 and float(words[2].strip()) >= lower_sim_limit and float(words[2].strip()) <= upper_sim_limit:
                contact_count = contact_count + 1
                total_contact_count = total_contact_count + 1
                total_contact_time = total_contact_time + float(words[4].strip())
            elif float(words[2].strip()) > lower_sim_limit and float(words[2].strip()) <= upper_sim_limit:
                contact_count = contact_count + 1
                total_contact_count = total_contact_count + 1
                total_contact_time = total_contact_time + float(words[4].strip())

        # contact count is devided by 2 as the above accumulation also accumulates both sides
        # of the connections
        contact_count = contact_count / 2
        
        # dump the entry for the accumilated 
        outputfile2.write(str(lower_sim_limit) + "    " + str(upper_sim_limit) + "    " + str(contact_count) + "    " + str(period_number) + "\n")
        period_number = period_number + 1
        
        # set for next period
        lower_sim_limit = upper_sim_limit
        upper_sim_limit = upper_sim_limit + interval_period
            
    # dump the overall summary
    avg_contact_time = total_contact_time / total_contact_count
    outputfile3.write(str(total_contact_count) + " :: " + str(avg_contact_time) + "\n")


def close_files():
    global inputfile
    global tempfile1
    global outputfile1
    global outputfile2
    global outputfile3

    # close all open files
    inputfile.close()
    tempfile1.close()
    outputfile1.close()
    outputfile2.close()
    outputfile3.close()


def main(argv):
    # main function that calls all other functions
    parse_param_n_open_files(argv)
    extract_from_log()
    extract_contact_times()
    dump_periodic_summary()
    close_files()

if __name__ == "__main__":
    main(sys.argv[1:])
  
