#!/usr/bin/python
# Script to plot the CDF of message delays
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 23-dec-2017

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import spline

class Info:
    def __init__(self, msg_delay_file_name, title):

        self.msg_delay_file_name = msg_delay_file_name
        self.title = title

infolist = []

# init
infolist.append(Info("General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_md_l.txt", 
                     "Keetchi Reference Scenario (Loved Data)"))
infolist.append(Info("General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_md_nl.txt", 
                     "Keetchi Reference Scenario (non-Loved Data)"))


cdf_msg_delay_pdf_file = "all_scenarios_msg_delay_cdf.pdf"

plt.close('all')




# draw all message delays CDF
fig = plt.figure(num=1, figsize=(8, 6))
for info in infolist:
    data = []
    with open(info.msg_delay_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[3].strip())
                data.append(field)
    bin_list = []
    unit_size = 1.0
    unit_range = 2000
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_size * unit_range)
    counts, bin_edges = np.histogram(data, bins=bin_list)
    cdf = np.cumsum(counts)
    total = np.sum(counts)
    cdf2 = []
    for i in range(unit_range):
        val = float(cdf[i]) / total * 100
        cdf2.append(val)
    plt.plot(bin_edges[1:], cdf2, label=info.title)
    # xnew = np.linspace(bin_edges[1:].min(), bin_edges[1:].max(), 1000)
    # ynew = spline(bin_edges[1:], cdf2, xnew)
    # plt.plot(xnew, ynew, label=info.title)

plt.xlabel('Message Delays (sec)')
plt.ylabel('CDF (%)')
plt.title('CDF of Message Delays (All Messages)')
plt.ylim(0, 200)
plt.xlim(0, unit_range * unit_size)
plt.yticks(np.arange(0, 125, 25))
plt.legend()
plt.show()
fig.savefig(cdf_msg_delay_pdf_file, bbox_inches='tight')
