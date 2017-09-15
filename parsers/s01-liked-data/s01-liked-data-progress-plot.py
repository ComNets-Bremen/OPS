#!/usr/bin/python
# Script to plot the stats extracted related to received
# packets.
#
# This plot script requires the output files *ldp_txt files
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 15-sep-2017

import numpy as np
import matplotlib.pyplot as plt

data1 = np.genfromtxt('General_0_20170909_23_17_03_3328_epidemic2_log2_ldp.txt', delimiter='>!<', skip_header=1,
                     names=['x', 'y1', 'y2', 'y3', 'y4', 'y5', 'y6'])

data2 = np.genfromtxt('General_0_20170909_23_17_03_3328_epidemic2_log2_ldp.txt', delimiter='>!<', skip_header=1,
                     names=['x', 'y1', 'y2', 'y3', 'y4', 'y5', 'y6'])

fig = plt.figure(num=1, figsize=(8, 3))

plt.plot(data1['x'], data1['y2'], color='blue', label='Liked Data (Keetchi)')
plt.plot(data2['x'], data2['y2'], color='red', label='Liked Data (Epidemic)')

plt.xlabel('Time (seconds)')
plt.ylabel('Bytes Received')
plt.title('Byte Receipts for Scenario 2')
plt.ylim(0, 300000000)
plt.legend()

plt.show()

graph_filename = './scenario2.pdf'
fig.savefig(graph_filename, bbox_inches='tight')
