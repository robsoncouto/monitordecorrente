import matplotlib.pyplot as plt
import time
import random

from queue import *
q = Queue(maxsize=50)
while q.full()==False:
    q.put(random.randint(1, 10),False)

#plt.figure(1)

plotting = True;

#function for closing program from GUI
def handle_close(evt):
    global plotting
    plotting=False
    print('Closed Figure! See ya!')
    #sys.exit(0)

#creates figure
fig = plt.figure()
#connects figure close event to handle_close function
fig.canvas.mpl_connect('close_event', handle_close)
#plt.ion()


#HACK plots the data, for now testing with random data
while plotting:
    t=q.get()
    q.put(random.randint(1, 10),False)
    l=list(q.queue)
    #print(l)
    #plt.subplot(211)
    plt.plot(l)
    plt.ylabel('Corrente (A)')
    plt.xlabel('Tempo')
    plt.draw()
    plt.pause(0.01)
    plt.clf()
    
    #plt.show()
