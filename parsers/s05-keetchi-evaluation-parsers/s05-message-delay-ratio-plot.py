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
    def __init__(self, msg_delay_file_name, node_delay_ratio_file_name, msg_hops_file_name, 
                 title, msg_delay_pdf_file, node_delay_pdf_file, node_ratio_pdf_file, msg_hops_pdf_file):

        self.msg_delay_file_name = msg_delay_file_name
        self.node_delay_ratio_file_name = node_delay_ratio_file_name
        self.msg_hops_file_name = msg_hops_file_name
        self.title = title
        self.msg_delay_pdf_file = msg_delay_pdf_file
        self.node_delay_pdf_file = node_delay_pdf_file
        self.node_ratio_pdf_file = node_ratio_pdf_file
        self.msg_hops_pdf_file = msg_hops_pdf_file

infolist = []

# init
infolist.append(Info("General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_md_l.txt", 
                     "General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_madrh_l.txt", 
                     "General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_mh_l.txt", 
                     "Keetchi Reference Scenario (Loved Data)", 
                     "keetchi_ref_loved_msg_delay_hist.pdf", 
                     "keetchi_ref_loved_node_msg_delay_hist.pdf", 
                     "keetchi_ref_loved_net_msg_ratio_hist.pdf", 
                     "keetchi_ref_loved_net_msg_hops_hist.pdf"))
infolist.append(Info("General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_md_nl.txt", 
                     "General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_madrh_nl.txt", 
                     "General_0_20171219_14_21_12_19973_keetchi_refscenario_log2_mh_nl.txt", 
                     "Keetchi Reference Scenario (non-Loved Data)", 
                     "keetchi_ref_nonloved_msg_delay_hist.pdf", 
                     "keetchi_ref_nonloved_node_msg_delay_hist.pdf", 
                     "keetchi_ref_nonloved_net_msg_ratio_hist.pdf", 
                     "keetchi_ref_nonloved_net_msg_hops_hist.pdf"))

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
    unit_size = 1000.0
    unit_range = 300
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
    plt.ylim(0, 8000)
    plt.xlim(0.0, (unit_range * unit_size))
    plt.legend()
    plt.tight_layout()
    plt.show()
    fig.savefig(info.msg_delay_pdf_file, bbox_inches='tight')
    plt.close(fig)


# draw the node wide average message delay histograms
for info in infolist:
    data = []
    with open(info.node_delay_ratio_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = float(words[2].strip())
                data.append(field)

    # print data
    bin_list = []
    unit_size = 1000.0
    unit_range = 300
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
    fig.savefig(info.node_delay_pdf_file, bbox_inches='tight')
    plt.close(fig)


# draw the node average delivery ratio histograms
for info in infolist:
    data = []
    with open(info.node_delay_ratio_file_name, 'rb') as source:
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
    fig.savefig(info.node_ratio_pdf_file, bbox_inches='tight')
    plt.close(fig)


# draw the all messages hops histograms
for info in infolist:
    data = []
    with open(info.msg_hops_file_name, 'rb') as source:
        for line in source:
            if not line.startswith("#"):
                words = line.split(">!<")
                field = int(words[3].strip())
                data.append(field)

    # print data
    bin_list = []
    unit_size = 10
    unit_range = 40
    for i in range(unit_range):
        bin_list.append(i * unit_size)
    bin_list.append(unit_range * unit_size)
    counts, bin_edges = np.histogram(data, bins=bin_list)
    fig = plt.figure(num=1, figsize=(8, 6))
    bar_width = unit_size * 0.75
    opacity = 0.4
    plt.bar(bin_edges[1:], counts, bar_width, alpha=opacity, color='blue', label=info.title, align='center')
    plt.xlabel('Hops')
    plt.ylabel('Frequency')
    plt.title('Histogram of Hops Travelled (of all Messages)')
    plt.ylim(0, 120000)
    plt.xlim(0.0, (unit_range * unit_size))
    plt.legend()
    plt.tight_layout()
    plt.show()
    fig.savefig(info.msg_hops_pdf_file, bbox_inches='tight')
    plt.close(fig)

