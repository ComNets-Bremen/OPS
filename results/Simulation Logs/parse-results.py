#!/usr/bin/env python

import sys,os,
from os import listdir
import tarfile
import zipfile
import shutil
import math  
import pandas as pd
from pandas import ExcelWriter

directory_path = os.getcwd()
          
def get_netfiles(directory, extension):
    return (f for f in listdir(directory) if f.endswith('_net.' + extension))    

def get_pstfiles(directory, extension):
    return (f for f in listdir(directory) if f.endswith('_pst.' + extension))    

# This function gets the values of metrics from the logs of all simulation runs. Metrics such as popular-delivery ratio, popular-delivery delay, 
# nonpopular-delivery ratio, nonpopular-delivery relay and total bytes sent

def get_alldata():
    ldratio = []
    ldelay = []
    nldratio = []
    nldelay = []
    data_sbytes = []
    total_sbytes = []
    overhead_sbytes = []
    
    netfiles = get_netfiles(os.getcwd(),extension = "txt")
  
    for value in netfiles:
        l_dratio, l_delay, nl_dratio, nl_delay = get_netlist(os.getcwd(),value)
        ldratio.append(l_dratio)
        ldelay.append(l_delay)
        nldratio.append(nl_dratio)
	nldelay.append(nl_delay)

    pstfiles = get_pstfiles(os.getcwd(),extension = "txt")

    for value in pstfiles:
        data_bytes, total_bytes = get_pstlist(os.getcwd(),value)
        data_sbytes.append(data_bytes)
	total_sbytes.append(total_bytes)        
 
    loved_delratio = [item for sublist in ldratio for item in sublist]
    loved_deldelay = [item for sublist in ldelay for item in sublist]
    nonloved_delratio = [item for sublist in nldratio for item in sublist]
    nonloved_deldelay = [item for sublist in nldelay for item in sublist]
    data_sentbytes = [item for sublist in data_sbytes for item in sublist]
    total_sentbytes = [item for sublist in total_sbytes for item in sublist]   

    return loved_delratio, loved_deldelay, nonloved_delratio, nonloved_deldelay, data_sentbytes, total_sentbytes


#This function collects the *_net.txt files to get the values of popular and nonpopular data 

def get_netlist(directory,f):
    l_delratio = []
    l_deldelay = []
    nl_delratio = []
    nl_deldelay = []
    x_file = open(directory+"/"+f, "r")
    for line in x_file:
        if " loved" in line:
	    words = line.split(">!<")
            delratio = words[4].strip()
	    deldelay = words[1].strip()
	    l_deldelay.append(float(deldelay))
            l_delratio.append(float(delratio))
        elif " non-loved" in line:
	    words = line.split(">!<")
            delratio = words[4].strip()
	    deldelay = words[1].strip()
	    nl_deldelay.append(float(deldelay))
            nl_delratio.append(float(delratio))
    return l_delratio, l_deldelay, nl_delratio, nl_deldelay

# This function collects the *_pst.txt files to get the values of total bytes sent during the simulation
def get_pstlist(directory,f):
    data_sentbytes = []
    total_sentbytes = []
    x_file = open(directory+"/"+f, "r")
    for line in x_file:
        if "# totals " not in line:
	    words = line.split(">!<")
	    total = words[2].strip()
	    data = words[4].strip()
	    total_sentbytes.append(float(total))
            data_sentbytes.append(float(data))      
    return data_sentbytes,total_sentbytes


def main(argv):
    
    file_name = os.path.basename(directory_path)
    popular_delratio, popular_deldelay, nonpopular_delratio, nonpopular_deldelay, data_sentbytes, total_sentbytes = get_alldata() # gets the delivery ratio list for one     set of random seeds  
    error_message = " "
    if len(popular_delratio) == 0:
        error_message = 'Log files broken or not found. Required metric values not found in log'
    elif len(popular_deldelay) == 0:
        error_message = 'Log files broken or not found. Required metric values not found in log'
    elif len(nonpopular_delratio) == 0:
        error_message = 'Log files broken or not found. Required metric values not found in log'
    elif len(nonpopular_deldelay) == 0:
        error_message = 'Log files broken or not found. Required metric values not found in log'
    else:
        writer = ExcelWriter(file_name + '.xlsx')
        title = ['Popular-DeliveryRatio', 'Popular-DeliveryDelay', 'Nonpopular-DeliveryRatio', 'Nonpopular-DeliveryDelay', 'Totalbytessent']
        df = pd.DataFrame(columns = title)
        df.to_excel(writer,'Sheet1',index=False)
        df2 = pd.DataFrame([popular_delratio, popular_deldelay, nonpopular_delratio, nonpopular_deldelay, total_sentbytes])
        df3 = df2.transpose()
        df.to_excel(writer,'Sheet1',index=False)
        df3.to_excel(writer,'Sheet1',startrow=1, index=False, header=0)
        writer.save()

    print error_message

    if len(total_sentbytes) == 0:
        error_message = 'Log files broken or not found. Total bytes exchanged for popular and non-popular data items not found in log'
        print error_message

if __name__ == "__main__":
    main(sys.argv[1:])


