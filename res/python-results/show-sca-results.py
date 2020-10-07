#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Show scalar statistics from the scalar files generated in 
simulations. 

As input, the files datalist.csv and statlist.csv 
files are required. Check the accompanying samples
to know what to be included in these files.

Check OPSNetwork.ned and KOPSNode.ned files to get 
all statistics generated.

@author: Asanga Udugama (adu@comnets.uni-bremen.de)
@date:   Fri Aug 14 10:33:59 2020
"""

import csv
import argparse
import subprocess

# get params
print("running script", __file__)

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--datalist', help='List of the results files (.sca)',
                    required=True)
parser.add_argument('-s', '--statlist', help='List of the statistics to show',
                    required=True)
args = parser.parse_args()

print('parameters', '-d', args.datalist, '-s', args.statlist)

# init array to hold stats of each scenario (i.e., scalar data file)
all_data = []
headers_added = False

# extract data and build array
with open(args.datalist, 'r') as datalistfp:
    dlines = csv.reader(datalistfp, delimiter=',')
    for drow in dlines:
        if drow[0].startswith('#'):
            continue

        sca_file_name = drow[0].strip()
        net_name = drow[2].strip()
        short_desc = drow[3].strip()
        long_desc = drow[4].strip()

        data_line = []
        header_line = [] 

        with open(args.statlist, 'r') as statlistfp:
            slines = csv.reader(statlistfp, delimiter=',')
            for srow in slines:
                if srow[0].startswith('#'):
                    continue

                stat_name = srow[0].strip()
                sca_type = srow[1].strip()
                short_name = srow[2].strip()
                long_name = srow[3].strip()
                unit_name = srow[4].strip()
                data_type = srow[5].strip()

                if not headers_added:
                    header_line.append(long_name)

                # run the scavetool to extract a statistic
                filter_str = '\"module(' + net_name + ') AND name(' \
                                    + stat_name + ':' + sca_type + ')\"'
                tempfile = 'temp.csv'
                subprocess.call(['scavetool', 'export', '-v', 
                                 '-f', filter_str,
                                 '-o', tempfile, '-F', 'CSV-S', sca_file_name])

                # get the formatted value of the statistic
                val_str = ''
                with open(tempfile,'r') as tempfilefp:
                    tlines = csv.reader(tempfilefp, delimiter=',')
                    for i, trow in enumerate(tlines):
                        if i == 0:
                            continue
                        
                        # check and get formatted statistic
                        if 'int' in data_type:
                            val = 0 if 'Inf' in trow[4].strip() \
                                or 'NaN' in trow[4].strip() \
                                    else int(trow[4].strip())
                            val_str = '{:,d}'.format(val) + ' ' + unit_name
                        else:
                            val = 0.0 if 'Inf' in trow[4].strip() \
                                or 'NaN' in trow[4].strip() \
                                    else float(trow[4].strip())
                            val_str = '{:,.4f}'.format(val) + ' ' + unit_name

                        break

                # add formatted statistic to array
                data_line.append(val_str)

        # add the statistic header line
        if not headers_added:
            all_data.append(['Scen/Stat'] + header_line)
            headers_added = True
        
        # add the statistic values line
        all_data.append([long_desc] + data_line)

# print the data
for line in all_data:
    for val in line:
        print(val, end='\t\t')
    print('')

