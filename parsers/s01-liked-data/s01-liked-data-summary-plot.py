#!/usr/bin/python
# Script to plot the stats extracted related to liked
# data.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 22-aug-2017

import numpy as np
import matplotlib.pyplot as plt

n_groups = 3

liked_received_bytes_total = (22963, 22963, 22963)
nonliked_received_bytes_total = (21844, 21653, 21616)
overheads_received_bytes_total = (34637, 34637, 34637)

# fig, ax = plt.subplots()

fig = plt.figure(num=1, figsize=(8, 3))

index = np.arange(n_groups)
bar_width = 0.2

opacity = 0.4

rects1 = plt.bar(index, liked_received_bytes_total, bar_width,
                 alpha=opacity,
                 color='blue', label='Liked Received')

rects2 = plt.bar(index + bar_width, nonliked_received_bytes_total, bar_width,
                 alpha=opacity,
                 color='green', label='Non-liked Received')

rects3 = plt.bar(index + (bar_width * 2), overheads_received_bytes_total, bar_width,
                 alpha=opacity,
                 color='red', label='Overheads Received')


plt.xlabel('Forwarding Algorithm')
plt.ylabel('Bytes Received')
plt.title('Liked, Non-liked and Overhead Receipts for Scenario 2')
plt.xticks(index + (bar_width * 2), ('Epidemic', 'RRS', 'Keetchi'))
plt.ylim(0, 50000)
plt.legend()

plt.tight_layout()
plt.show()

graph_filename = './scenario2.pdf'
fig.savefig(graph_filename, bbox_inches='tight')

