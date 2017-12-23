#!/usr/bin/python
# Script to plot the stats extracted related to received
# packets.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 22-aug-2017

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import spline

class Info:
    def __init__(self, file_name, title, pdf_file):
        self.file_name = file_name
        self.title = title
        self.pdf_file = pdf_file

infolist = []

# init
infolist.append(Info("General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_ps.txt", 
                     "Keetchi - Reference Scenario", 
                     "keetchi_ref_ps_hist.pdf"))
infolist.append(Info("General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_ps.txt", 
                     "Keetchi - Reference Scenario", 
                     "keetchi_ref_ps_hist.pdf"))

plt.close('all')

# draw the total bytes histograms
for info in infolist:
    data = []
    with open(info.file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[3].strip()) / 1000000
                data.append(field)

    # print data
    bin_list = []
    unit_size = 10
    unit_range = 100
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range * unit_size)
    counts, bin_edges = np.histogram(data, bins=bin_list)
    fig = plt.figure(num=1, figsize=(8, 6))
    bar_width = unit_size * 0.75
    opacity = 0.4
    plt.bar(bin_edges[1:], counts, bar_width, alpha=opacity, color='blue', label=info.title, align='center')
    plt.xlabel('Bytes Sent (MB)')
    plt.ylabel('Frequency')
    plt.title('Histogram of Total Bytes Sent By Nodes')
    plt.ylim(0, 600)
    plt.xlim(0, unit_range * unit_size)
    plt.legend()
    plt.tight_layout()
    plt.show()
    fig.savefig(info.pdf_file, bbox_inches='tight')
    plt.close(fig)
