#!/usr/bin/python
# Script to plot the stats extracted related to message delays
# delivery ratio
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 25-aug-2017

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import spline

class Info:
    def __init__(self, msg_delay_file_name, net_delay_ratio_file_name, title, msg_delay_pdf_file, net_delay_pdf_file, net_ratio_pdf_file):
        self.msg_delay_file_name = msg_delay_file_name
        self.net_delay_ratio_file_name = net_delay_ratio_file_name
        self.title = title
        self.msg_delay_pdf_file = msg_delay_pdf_file
        self.net_delay_pdf_file = net_delay_pdf_file
        self.net_ratio_pdf_file = net_ratio_pdf_file

infolist = []

# init
infolist.append(Info("General_0_20170909_23_17_03_3328_epidemic2_log2_md.txt", "General_0_20170909_23_17_03_3328_epidemic2_log2_madr.txt", "Scenario 2 - Epidemic", "s05_md_hist_epidemic2.pdf", "s05_mdr1_hist_epidemic2.pdf", "s05_mdr2_hist_epidemic2.pdf"))
infolist.append(Info("General_0_20170909_23_14_35_3034_keetchi2_log2_md.txt", "General_0_20170909_23_14_35_3034_keetchi2_log2_madr.txt", "Scenario 2 - Keetchi", "s05_md_hist_keetchi2.pdf", "s05_mdr1_hist_keetchi2.pdf", "s05_mdr2_hist_keetchi2.pdf"))

cdf_msg_delay_pdf_file = "s05_md_cdf_msg_delay.pdf"
cdf_node_msg_delay_pdf_file = "s05_mdr_cdf_node_msg_delay.pdf"
cdf_node_ratio_pdf_file = "s05_mdr_cdf_node_ratio.pdf"

plt.close('all')


# draw the all message delays histograms
for info in infolist:
    data = []
    with open(info.msg_delay_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[3].strip())
                data.append(field)

    # print data
    bin_list = []
    unit_size = 100.0
    unit_range = 20
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range * unit_size)
    counts, bin_edges = np.histogram(data, bins=bin_list)
    fig = plt.figure(num=1, figsize=(8, 6))
    bar_width = unit_size * 0.75
    opacity = 0.4
    plt.bar(bin_edges[1:], counts, bar_width, alpha=opacity, color='blue', label=info.title, align='center')
    plt.xlabel('Delay (sec)')
    plt.ylabel('Frequency')
    plt.title('Histogram of Message Delays (of all Messages)')
    plt.ylim(0, 5000)
    plt.xlim(0.0, (unit_range * unit_size))
    plt.legend()
    plt.tight_layout()
    plt.show()
    fig.savefig(info.msg_delay_pdf_file, bbox_inches='tight')
    plt.close(fig)


# draw the node wide average message delay histograms
for info in infolist:
    data = []
    with open(info.net_delay_ratio_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[2].strip())
                data.append(field)

    # print data
    bin_list = []
    unit_size = 100.0
    unit_range = 10
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range * unit_size)
    counts, bin_edges = np.histogram(data, bins=bin_list)
    fig = plt.figure(num=1, figsize=(8, 6))
    bar_width = unit_size * 0.75
    opacity = 0.4
    plt.bar(bin_edges[1:], counts, bar_width, alpha=opacity, color='blue', label=info.title, align='center')
    plt.xlabel('Delay (sec)')
    plt.ylabel('Frequency')
    plt.title('Histogram of Average Message Delays (of Nodes)')
    plt.ylim(0, 600)
    plt.xlim(0.0, (unit_range * unit_size))
    plt.legend()
    plt.tight_layout()
    plt.show()
    fig.savefig(info.net_delay_pdf_file, bbox_inches='tight')
    plt.close(fig)


# draw the node wide average delivery ratio histograms
for info in infolist:
    data = []
    with open(info.net_delay_ratio_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[5].strip())
                data.append(field)

    # print data
    bin_list = []
    unit_size = 0.01
    unit_range = 100
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range * unit_size)
    counts, bin_edges = np.histogram(data, bins=bin_list)
    fig = plt.figure(num=1, figsize=(8, 6))
    bar_width = unit_size * 0.75
    opacity = 0.4
    plt.bar(bin_edges[1:], counts, bar_width, alpha=opacity, color='blue', label=info.title, align='center')
    plt.xlabel('Delivery Ratio')
    plt.ylabel('Frequency')
    plt.title('Histogram of Delivery Ratios (of Nodes)')
    plt.ylim(0, 600)
    plt.xlim(0.0, (unit_range * unit_size) + 0.1)
    plt.legend()
    plt.tight_layout()
    plt.show()
    fig.savefig(info.net_ratio_pdf_file, bbox_inches='tight')
    plt.close(fig)


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


# draw node average message delays CDF
fig = plt.figure(num=1, figsize=(8, 6))
for info in infolist:
    data = []
    with open(info.net_delay_ratio_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[2].strip())
                data.append(field)
    bin_list = []
    unit_size = 1.0
    unit_range = 2000
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range)
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

plt.xlabel('Message Delays')
plt.ylabel('CDF (%)')
plt.title('CDF of Message Delays (from Node Averages)')
plt.ylim(0, 200)
plt.xlim(0, unit_range * unit_size)
plt.yticks(np.arange(0, 125, 25))
plt.legend()
plt.show()
fig.savefig(cdf_node_msg_delay_pdf_file, bbox_inches='tight')


# draw node delivery ratio CDF
fig = plt.figure(num=1, figsize=(8, 6))
for info in infolist:
    data = []
    with open(info.net_delay_ratio_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[5].strip())
                data.append(field)
    bin_list = []
    unit_size = 0.01
    unit_range = 100
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range)
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

plt.xlabel('Delivery Ratio')
plt.ylabel('CDF (%)')
plt.title('CDF of Message Delivery Ratio')
plt.ylim(0, 200)
plt.xlim(0.0, (unit_range * unit_size))
plt.yticks(np.arange(0, 125, 25))
plt.legend()
plt.show()
fig.savefig(cdf_node_ratio_pdf_file, bbox_inches='tight')

