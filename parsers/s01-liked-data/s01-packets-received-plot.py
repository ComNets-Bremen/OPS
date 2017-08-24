#!/usr/bin/python
# Script to plot the stats extracted related to received
# packets.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 22-aug-2017

import numpy as np
import matplotlib.pyplot as plt

n_groups = 3

total_packets =   (873589, 89272009, 15896577)
useful_data =     ( 54868,    54308,    54224)
additional_data = (   445, 89217701, 15769949)
feedback =        (     0,        0,    72404)
summary_vector =  (409146,        0,        0)
data_request =    (409130,        0,        0)

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.1

opacity = 0.4

rects1 = plt.bar(index, total_packets, bar_width,
                 alpha=opacity,
                 color='blue', label='Total Packets')

rects2 = plt.bar(index + bar_width, useful_data, bar_width,
                 alpha=opacity,
                 color='darkblue', label='Useful Data')

rects3 = plt.bar(index + (bar_width * 2), additional_data, bar_width,
                 alpha=opacity,
                 color='red', label='Additional Data')

rects4 = plt.bar(index + (bar_width * 3), feedback, bar_width,
                 alpha=opacity,
                 color='darkred', label='Feedback')

rects5 = plt.bar(index + (bar_width * 4), summary_vector, bar_width,
                 alpha=opacity,
                 color='green', label='Summary Vector')

rects6 = plt.bar(index + (bar_width * 5), data_request, bar_width,
                 alpha=opacity,
                 color='darkgreen', label='Data Request')

plt.xlabel('Forwarding Algorithm')
plt.ylabel('Packets Received')
plt.title('Packet Receipts')
plt.xticks(index + (bar_width * 2), ('Epidemic', 'RRS', 'Keetchi'))
plt.ylim(0, 95000000)
plt.legend()

plt.tight_layout()
plt.show()

