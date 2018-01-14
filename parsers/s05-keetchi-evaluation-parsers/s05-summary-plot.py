#!/usr/bin/python
# Script to plot the summary stats
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 04-jan-2018

import numpy as np
import matplotlib.pyplot as plt


bar_pos = np.arange(13)
bar_width = 0.3
scenarios = ['Keetchi\nBO 50%', 'Epidemic\n64 Hops', 'BO 1%', 'BO 10%', 'BO 20%', 'BO 30%', 'BO 40%', 'BO 60%', 'BO 70%', 'BO 80%', 'BO 90%', '5 Hops', '20 Hops']



delay_loved     = [29296.46,  20873.49, 14206.18,  23314.24,  29534.80,  28582.61,  30552.05,  27311.33,  27249.69,  28330.67,  29364.66, 10951.11, 5969.59]
delay_non_loved = [273680.32, 20269.57, 22125.05, 123019.16, 203034.56, 240696.67, 259869.70, 286365.19, 307256.40, 320889.84, 331327.11, 10614.21, 5807.61]

plt.close('all')
fig = plt.figure(num=1, figsize=(16, 7))
bar1 = plt.bar(bar_pos - bar_width, delay_loved, bar_width, color='r')
bar2 = plt.bar(bar_pos, delay_non_loved, bar_width, color='b')

plt.ylabel('Delay (seconds)')
plt.xlabel('Scenarios')
plt.xticks(bar_pos + bar_width)
plt.xticks(np.arange(13), scenarios)
plt.legend((bar1[0], bar2[0]), ('Loved', 'non-Loved'))

plt.show()
fig.savefig('comp_delay.pdf', bbox_inches='tight')
plt.close(fig)



dratio_loved      = [88.59, 95.59, 96.29, 90.21, 90.23, 90.52, 89.22, 88.21, 87.92, 87.16, 87.36, 98.13, 99.20]
dration_non_loved = [23.27, 95.95, 37.84, 43.20, 41.93, 34.48, 28.24, 20.49, 18.00, 15.51, 13.43, 98.44, 99.39]

plt.close('all')
fig = plt.figure(num=1, figsize=(16, 7))
bar1 = plt.bar(bar_pos - bar_width, dratio_loved, bar_width, color='r')
bar2 = plt.bar(bar_pos, dration_non_loved, bar_width, color='b')

plt.ylabel('Delivery Ratio')
plt.xlabel('Scenarios')
plt.xticks(bar_pos + bar_width)
plt.xticks(np.arange(13), scenarios)
plt.legend((bar1[0], bar2[0]), ('Loved', 'non-Loved'))

plt.show()
fig.savefig('comp_dratio.pdf', bbox_inches='tight')
plt.close(fig)


hops_loved        = [14.17, 12.89, 19.26,  15.27, 14.43, 14.33, 14.15,  14.11,  13.95,  13.86,  13.88,   3.55,  11.10]
hops_non_loved    = [ 4.87, 12.59, 33.58,   7.16,  4.90,  4.67,  4.82,   4.96,   5.25,   5.56,   5.71,   3.55,  10.94]


plt.close('all')
fig = plt.figure(num=1, figsize=(16, 7))
bar1 = plt.bar(bar_pos - bar_width, hops_loved, bar_width, color='r')
bar2 = plt.bar(bar_pos, hops_non_loved, bar_width, color='b')

plt.ylabel('Delivery Ratio')
plt.xlabel('Scenarios')
plt.xticks(bar_pos + bar_width)
plt.xticks(np.arange(13), scenarios)
plt.legend((bar1[0], bar2[0]), ('Loved', 'non-Loved'))

plt.show()
fig.savefig('comp_hops.pdf', bbox_inches='tight')
plt.close(fig)


bytes_sent        = [14218146725, 2592234272932, 321024209125, 41844570550, 25260012550, 19428700625, 16349251200, 12951728575, 12035383425, 11145113300, 10551853850, 29856790384, 232990373875]
bytes_data        = [14218146725, 2575650789300, 321024209125, 41844570550, 25260012550, 19428700625, 16349251200, 12951728575, 12035383425, 11145113300, 10551853850, 26449518900, 222232545875]

plt.close('all')
fig = plt.figure(num=1, figsize=(16, 7))
bar1 = plt.bar(bar_pos - bar_width, bytes_sent, bar_width, color='g')
bar2 = plt.bar(bar_pos, bytes_data, bar_width, color='m')

plt.ylabel('Bytes Sent')
plt.xlabel('Scenarios')
plt.xticks(bar_pos + bar_width)
plt.xticks(np.arange(13), scenarios)
plt.legend((bar1[0], bar2[0]), ('Total Bytes', 'Data Bytes'))

plt.show()
fig.savefig('comp_bytes.pdf', bbox_inches='tight')
plt.close(fig)




