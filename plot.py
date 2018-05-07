import matplotlib
matplotlib.use("TkAgg")

import numpy
from matplotlib import pyplot as plt

plt.ion()

MINBOUND=20
MAXBOUND=50

fig, [ax1, ax2,ax3] = plt.subplots(3, 1,sharex='col')
ax1.set_ylim(MINBOUND,MAXBOUND)
ax2.set_ylim(MINBOUND,MAXBOUND)
ax3.set_ylim(MINBOUND,MAXBOUND)
#plt.ylim([MINBOUND,MAXBOUND])

y1data = [MINBOUND]*50# 50 samples, fixed
y2data = [MINBOUND]*50
y3data = [MINBOUND]*50

line1, = ax1.plot(y1data)
line2, = ax2.plot(y2data)
line3, = ax3.plot(y3data)

import time
import sys
import random

frq = 20.0

print ("Frequency:", frq, "Hz")

while True:
    y1data.append(random.randint(MINBOUND, MAXBOUND))
    y2data.append(random.randint(MINBOUND, MAXBOUND))
    y3data.append(random.randint(MINBOUND, MAXBOUND))
    del y1data[0]
    del y2data[0]
    del y3data[0]

    line1.set_ydata(y1data)
    line2.set_ydata(y2data)
    line3.set_ydata(y3data)
    
    fig.canvas.draw()
    fig.canvas.flush_events()

    time.sleep(1/frq)
