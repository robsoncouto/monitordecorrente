import matplotlib
#matplotlib.use("TkAgg")

import numpy
from matplotlib import pyplot as plt
import serial

NOSERIAL=0
try:
    port=serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
except:
    NOSERIAL=1
    print("Failed to open the serial port, falling back into test mode")
    print("Random data will be used")

print("Please select one of the modes:");
print("\t 1) Current Mode 0-300A")
print("\t 2) ADV Voltage Mode 0-5V")
ans=int(input(""));

if ans==1:
    MINBOUND=0
    MAXBOUND=300
if ans==2:
    MINBOUND=0
    MAXBOUND=5
    
plt.ion()

fig, [ax1, ax2,ax3] = plt.subplots(3, 1,sharex='col')
ax1.set_ylim(MINBOUND,MAXBOUND)
ax2.set_ylim(MINBOUND,MAXBOUND)
ax3.set_ylim(MINBOUND,MAXBOUND)

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

plotting = True;
def handle_close(evt):
    global plotting
    plotting=False
    print('Closed Figure! See ya!')
    #sys.exit(0)
fig.canvas.mpl_connect('close_event', handle_close)

while plotting:
    if NOSERIAL==0:
        while port.inWaiting()<30:
            print(".",end='')
            time.sleep(0.1)
        data = port.read(1)
        if (data==b"\x55"):
            data = port.read(1)
            if (data==b"\x55"):
                data = port.read(1)
                if (data==bytes("A","ASCII")):
                    data = port.read(6)
                    y1data.append(data)
                    del y1data[0]
                if (data==bytes("B","ASCII")):
                    data = port.read(6)
                    y2data.append(data)
                    del y2data[0]
                if (data==bytes("C","ASCII")):
                    data = port.read(6)
                    y3data.append(data)
                    del y3data[0]
        
    else:    
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
