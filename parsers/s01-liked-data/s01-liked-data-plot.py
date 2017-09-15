#!/usr/bin/python
# Script to plot the stats extracted related to liked
# data.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 22-aug-2017

import numpy as np
import matplotlib.pyplot as plt

n_groups = 3

liked_total = (22963, 22963, 22963)
liked_received = (21844, 21653, 21616)
non_liked_total = (34637, 34637, 34637)
non_liked_received = (33024, 32655, 32608)

# fig, ax = plt.subplots()

fig = plt.figure(num=1, figsize=(8, 3))

index = np.arange(n_groups)
bar_width = 0.2

opacity = 0.4

rects1 = plt.bar(index, liked_total, bar_width,
                 alpha=opacity,
                 color='blue', label='Liked Total')

rects2 = plt.bar(index + bar_width, liked_received, bar_width,
                 alpha=opacity,
                 color='darkblue', label='Liked Received')

rects3 = plt.bar(index + (bar_width * 2), non_liked_total, bar_width,
                 alpha=opacity,
                 color='red', label='Non-liked Total')

rects4 = plt.bar(index + (bar_width * 3), non_liked_received, bar_width,
                 alpha=opacity,
                 color='darkred', label='Non-liked Received')

plt.xlabel('Forwarding Algorithm')
plt.ylabel('Packets Received')
plt.title('Liked/Non-liked Receipts')
plt.xticks(index + (bar_width * 2), ('Epidemic', 'RRS', 'Keetchi'))
plt.ylim(0, 50000)
plt.legend()

plt.tight_layout()
plt.show()

graph_filename = './scenario2.pdf'
fig.savefig(graph_filename, bbox_inches='tight')

