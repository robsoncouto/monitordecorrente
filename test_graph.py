import matplotlib
matplotlib.use("TkAgg")

import numpy
from matplotlib import pyplot as plt

plt.ion()

MINBOUND=20
MAXBOUND=50

fig, ax = plt.subplots()
plt.ylim([MINBOUND,MAXBOUND])

ydata = [0]*50
line, = ax.plot(ydata)

#HACK import RPi.GPIO as GPIO
import time
import sys
import random
#HACK GPIO.setmode(GPIO.BCM)


print ("Distance Measurement In Progress")

#HACK GPIO.setup(TRIG,GPIO.OUT)
#HACK GPIO.setup(ECHO,GPIO.IN)

#HACK GPIO.output(TRIG, False)
print ("Waiting For Sensor To Settle")
time.sleep(2)

frq = 20.0

print ("Frequency:", frq, "Hz")
print ("Distances (cm)")


while True:
    distance = random.randint(MINBOUND, MAXBOUND)
    ydata.append(distance)
    del ydata[0]

    line.set_ydata(ydata)
    fig.canvas.draw()
    fig.canvas.flush_events()

    time.sleep(1/frq)

#HACK GPIO.cleanup()
