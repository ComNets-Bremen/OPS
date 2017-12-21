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
# infolist.append(Info("General_0_20170923_22_38_49_16134_epidemic_refscenario_log2_ps.txt", "Reference Scenario - Epidemic", "s05_ps_hist_epidemic_refscenario.pdf"))
# cdf_total_bytes_pdf_file = "s05_ps_cdf_total_bytes.pdf"
# cdf_total_overheads_pdf_file = "s05_ps_cdf_overhead_bytes_refscenario.pdf"

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


# # draw total bytes CDF
# fig = plt.figure(num=1, figsize=(8, 6))
# for info in infolist:
#     data = []
#     with open(info.file_name, 'rb') as source:
#         for line in source:
#             if not line.startswith("#"):
#                 words = line.split(">!<")
#                 field = float(words[3].strip()) / 1000000
#                 data.append(field)
#     bin_list = []
#     unit_size = 10000.0
#     unit_range = 16
#     for i in range(unit_range):
#         bin_list.append(i * unit_size)
#     bin_list.append(unit_range * unit_size)
#     counts, bin_edges = np.histogram(data, bins=bin_list)
#     cdf = np.cumsum(counts)
#     total = np.sum(counts)
#     cdf2 = []
#     for i in range(unit_range):
#         val = float(cdf[i]) / total * 100
#         cdf2.append(val)
#     plt.plot(bin_edges[1:], cdf2, label=info.title)
#     # xnew = np.linspace(bin_edges[1:].min(), bin_edges[1:].max(), 1000)
#     # ynew = spline(bin_edges[1:], cdf2, xnew)
#     # plt.plot(xnew, ynew, label=info.title)
#
# plt.xlabel('Total Bytes Sent (MB)')
# plt.ylabel('CDF (%)')
# plt.title('CDF of Total Bytes Sent')
# plt.ylim(0, 200)
# plt.xlim(0, 200000)
# plt.yticks(np.arange(0, 125, 25))
# plt.legend()
# plt.show()
# fig.savefig(cdf_total_bytes_pdf_file, bbox_inches='tight')



# # draw overhead bytes CDF
# fig = plt.figure(num=1, figsize=(8, 6))
# for info in infolist:
#     data = []
#     with open(info.file_name, 'rb') as source:
#         for line in source:
#             if not line.startswith("#"):
#                 words = line.split(">!<")
#                 overhead_total = int(words[7].strip()) + int(words[9].strip()) + int(words[11].strip())
#                 field = float(overhead_total) / 1000000
#                 data.append(field)
#     bin_list = []
#     unit_size = 10000.0
#     unit_range = 16
#     for i in range(unit_range):
#         bin_list.append(i * unit_size)
#     bin_list.append(unit_range * unit_size)
#     counts, bin_edges = np.histogram(data, bins=bin_list)
#     cdf = np.cumsum(counts)
#     total = np.sum(counts)
#     cdf2 = []
#     for i in range(unit_range):
#         val = float(cdf[i]) / total * 100
#         cdf2.append(val)
#     plt.plot(bin_edges[1:], cdf2, label=info.title)
#     # xnew = np.linspace(bin_edges[1:].min(), bin_edges[1:].max(), 1000)
#     # ynew = spline(bin_edges[1:], cdf2, xnew)
#     # plt.plot(xnew, ynew, label=info.title)
#
# plt.xlabel('Overhead Bytes Sent (KB)')
# plt.ylabel('CDF (%)')
# plt.title('CDF of Overhead Bytes Sent')
# plt.ylim(0, 200)
# plt.xlim(0, 200000)
# plt.yticks(np.arange(0, 125, 25))
# plt.legend()
# plt.show()
# fig.savefig(cdf_total_overheads_pdf_file, bbox_inches='tight')

