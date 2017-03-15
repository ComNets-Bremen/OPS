#!/usr/bin/python
# Script to kill a currently running OMNeT++ simulation when
# the simulation time reaches a given time. The script uses 
# the log file to determine the current simulation time and 
# then kill the process.
#
# This script must be invoked as a cron job with the input 
# file (-i) and the time (-k) in seconds. e.g.,
#
# kill-sim-at-time -i log-file.txt -k 326789
# 
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 17-feb-2017

import sys, getopt, subprocess, time

inputfilename = ""
kill_sim_time = 0.0
current_sim_time = 0.0


def parse_param(argv):
    global inputfilename
    global kill_sim_time
    
    try:
        opts, args = getopt.getopt(argv,"hi:k:",["ifile=", "killtime="])
    except getopt.GetoptError:
        print 'kil-sim-at-time.py -i <inputfile> -k <kill time>'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 'kil-sim-at-time.py -i <inputfile> -k <kill time>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfilename = arg
        elif opt in ("-k", "--killtime"):
            kill_sim_time = float(arg)


def get_current_sim_time():
    global inputfilename
    global current_sim_time

    command = "tail -n 10 " + inputfilename + " | grep INFO | grep KRRSLayer" 
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=None, shell=True)
    output = process.communicate()
    lines = output[0].split('\n')
    for line in lines:
        line = line.strip()
        if not line:
            continue
        words = line.split(" :: ")
        current_sim_time = float(words[1].strip())
        if current_sim_time > 0.0:
            break


def check_time_and_kill():
    global kill_sim_time
    global current_sim_time

    data_time = time.strftime("%Y-%m-%d %H:%M")
    print "Current Clock Time: ", data_time
    print "Current Sim Time  : ", current_sim_time, "seconds -", str(((current_sim_time / 3600) / 24)), "days"
    print "Kill Sim Time     : ", kill_sim_time, "seconds -", str(((kill_sim_time / 3600) / 24)), "days"
    
    if current_sim_time > kill_sim_time:
        print "Trying to kill ops-simu..."
        command = "killall -9 ops-simu" 
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=None, shell=True)
        output = process.communicate()
    else:
        print "Sim time not reached yet to kill ops-simu"
    print "\n"

def main(argv):
    parse_param(argv)
    get_current_sim_time()
    check_time_and_kill()


if __name__ == "__main__":
    main(sys.argv[1:])
  
