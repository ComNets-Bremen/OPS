#!/usr/bin/python
# Script to plot the stats extracted related to sent
# packets.
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 22-aug-2017

import numpy as np
import matplotlib.pyplot as plt

n_groups = 3

total_packets =   (873589, 89272009, 15896577)
data =            ( 54868,    54308,    54224)
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

rects2 = plt.bar(index + bar_width, data, bar_width,
                 alpha=opacity,
                 color='darkblue', label='Data')

rects3 = plt.bar(index + (bar_width * 2), feedback, bar_width,
                 alpha=opacity,
                 color='darkred', label='Feedback')

rects4 = plt.bar(index + (bar_width * 3), summary_vector, bar_width,
                 alpha=opacity,
                 color='green', label='Summary Vector')

rects5 = plt.bar(index + (bar_width * 4), data_request, bar_width,
                 alpha=opacity,
                 color='darkgreen', label='Data Request')

plt.xlabel('Forwarding Algorithm')
plt.ylabel('Packets Sent')
plt.title('Packet Transmissions')
plt.xticks(index + (bar_width * 2), ('Epidemic', 'RRS', 'Keetchi'))
plt.ylim(0, 95000000)
plt.legend()

plt.tight_layout()
plt.show()

