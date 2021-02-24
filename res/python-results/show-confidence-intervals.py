#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Show the confidence intervals for each statistics.

As input, the files datalist.csv, statlist.csv
the alpha value and whether samples are Student-t
or Normally distributed are required. Check the 
accompanying samples to know what to be included 
in these files. 

Check OPSNetwork.ned and KOPSNode.ned files to get 
all statistics generated.

@author: Asanga Udugama (adu@comnets.uni-bremen.de)
@date:   Sat Aug 15 16:53:50 2020
"""

import csv
import argparse
import subprocess
import scipy.stats as st
import numpy as np
import sys

# get params
print("running script", __file__)

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--datalist', help='List of the results files (.vec)',
                    required=True)
parser.add_argument('-s', '--statlist', help='List of the statistics to plot',
                    required=True)
parser.add_argument('-a', '--alpha', type=float, help='Alpha value to use - 0.99, 0.95',
                    required=True)
parser.add_argument('-t', '--tdist', action='store_true', help='Assume t-distributed instead of normaly distributed')
args = parser.parse_args()

print('parameters', '-d', args.datalist, '-s', args.statlist, '-a', args.alpha, ('-t' if args.tdist else ''))

all_data = []

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

        stat_val_list = []
        with open(args.datalist, 'r') as datalistfp:
            dlines = csv.reader(datalistfp, delimiter=',')
            for drow in dlines:
                if drow[0].startswith('#'):
                    continue

                sca_file_name = drow[0].strip()
                net_name = drow[2].strip()
                short_desc = drow[3].strip()

                # run the scavetool to extract a statistic
                filter_str = '\"module(' + net_name + ') AND name(' \
                                    + stat_name + ':' + sca_type + ')\"'
                tempfile = 'temp.csv'
                subprocess.call(['scavetool', 'export', '-v', 
                                 '-f', filter_str,
                                 '-o', tempfile, '-F', 'CSV-S', sca_file_name])

                with open(tempfile,'r') as tempfilefp:
                    tlines = csv.reader(tempfilefp, delimiter=',')
                    for i, trow in enumerate(tlines):
                        if i == 0:
                            continue
                        
                        # check and get formatted statistic
                        val = 0
                        if 'int' in data_type:
                            val = 0 if 'Inf' in trow[4].strip() \
                                or 'NaN' in trow[4].strip() \
                                    else int(trow[4].strip())
                        else:
                            val = 0.0 if 'Inf' in trow[4].strip() \
                                or 'NaN' in trow[4].strip() \
                                    else float(trow[4].strip())

                        stat_val_list.append(val)
                        break

        # compute the confidence intervals
        if not all(v == 0.0 for v in stat_val_list):
            mean_val = np.mean(stat_val_list)
            std_err_val = st.sem(stat_val_list)
            if args.tdist:
                ci_range = st.t.interval(args.alpha, len(stat_val_list)-1, loc=mean_val, scale=std_err_val)
            else:
                ci_range = st.norm.interval(alpha=args.alpha, loc=mean_val, scale=std_err_val)
        else:
            mean_val = 0.0
            std_err_val = 0.0
            ci_range = [0.0, 0.0]

        # make and append to info line to list 
        val_str = long_name + '\t\t' + '{:,.4f}'.format(mean_val) + '\t\t' + unit_name + '\t\t' + '{:,.4f}'.format(ci_range[0]) + ' - ' + '{:,.4f}'.format(ci_range[1])
        all_data.append(val_str)

# print the data
if len(all_data) > 0:
    print('Statistic Name\t\t\tMean Value\t\t\tConfidence Interval')
    for line in all_data:
        print(line)
else:
    print('No data found to compute confidence intervals')

