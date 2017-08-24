#!/usr/bin/python
# Script to plot the stats extracted related to co-efficient.
# of variation (as means of how well traffic is spread in
# a network)
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 22-aug-2017

import numpy as np
import matplotlib.pyplot as plt

n_groups = 3

cov_val = (0.526160782877, 0.705607392984, 0.585239811206)

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.35

opacity = 0.4

rects1 = plt.bar(index, cov_val, bar_width,
                 alpha=opacity,
                 color='blue', label='Coefficient of Variation')

plt.xlabel('Forwarding Algorithm')
plt.ylabel('Coefficient of Variation')
plt.title('Traffic Spread in Network')
plt.xticks(index + (bar_width / 2), ('Epidemic', 'RRS', 'Keetchi'))
plt.ylim(0, 1.0)
plt.legend()

plt.tight_layout()
plt.show()
