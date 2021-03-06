#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Plot the vector boxplots for each statistics using
matplotlib.

As input, the files datalist.csv and statlist.csv 
files are required. Check the accompanying samples
to know what to be included in these files.

Check OPSNetwork.ned and KOPSNode.ned files to get 
all statistics generated.

@author: Asanga Udugama (adu@comnets.uni-bremen.de)
@date:   Sat Aug 15 16:53:50 2020
"""

import csv
import argparse
import subprocess
import matplotlib.pyplot as plt
import os

# get params
print("running script", __file__)

parser = argparse.ArgumentParser()
parser.add_argument('-d', '--datalist', help='List of the results files (.vec)',
                    required=True)
parser.add_argument('-s', '--statlist', help='List of the statistics to plot',
                    required=True)
args = parser.parse_args()

print('parameters', '-d', args.datalist, '-s', args.statlist)

# extract results and plot
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

        stat_data = []
        stat_labels = []
        with open(args.datalist, 'r') as datalistfp:
            dlines = csv.reader(datalistfp, delimiter=',')
            for drow in dlines:
                if drow[0].startswith('#'):
                    continue

                vec_file_name = drow[1].strip()
                net_name = drow[2].strip()
                short_desc = drow[3].strip()

                # run the scavetool to extract a statistic
                filter_str = '\"module(' + net_name + ') AND name(' \
                                    + stat_name + ':vector)\"'
                tempfile = 'temp.csv'
                subprocess.call(['scavetool', 'export', '-v', 
                                 '-f', filter_str,
                                 '-o', tempfile, '-F', 'CSV-S', vec_file_name])

                y = []
                with open(tempfile,'r') as tempfilefp:
                    tlines = csv.reader(tempfilefp, delimiter=',')
                    for i, trow in enumerate(tlines):
                        if i == 0:
                            continue

                        # get statistic val
                        if 'int' in data_type:
                            stat_val = 0 if 'Inf' in trow[1].strip() \
                                or 'NaN' in trow[1].strip() \
                                    else int(trow[1].strip())
                        else:
                            stat_val = 0.0 if 'Inf' in trow[1].strip() \
                                or 'NaN' in trow[1].strip() \
                                    else float(trow[1].strip())

                        y.append(stat_val)
                stat_data.append(y)
                stat_labels.append(short_desc)

        # plot the boxplot
        plt.figure(figsize=(12, 4))
        plt.gca().yaxis.grid()

        plt.boxplot(stat_data, notch=True, showfliers=True)

        plt.ylabel(long_name + ' (' + unit_name + ')')
        plt.xticks([[i + 1] for i in range(len(stat_labels))], stat_labels)
        plt.title('Box Plots - ' + long_name)
        plt.tight_layout()
        pdf_file = os.path.join('.', 'boxplot-' + stat_name + '.pdf')
        plt.savefig(pdf_file)
        plt.close()
