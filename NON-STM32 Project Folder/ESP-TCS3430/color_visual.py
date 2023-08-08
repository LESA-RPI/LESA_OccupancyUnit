import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
from PIL import Image
import numpy as np
import serial
import time
import colour
from colour.plotting import *

# read from serial port
Baudrate = 115200
Port = 'com5'  # set the correct port before run it

Serial = serial.Serial(port=Port, baudrate=Baudrate)
Serial.timeout = 2  # set read timeout

if Serial.is_open:  
    # start serial read and plot
    data = Serial.readline()  
    plt.ion()
    
    arr_x = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    arr_y = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    RGB_colors = [[0,0,0],[0,0,0],[0,0,0],[0,0,0],[0,0,0]]


    fig = plt.figure()
    ax = fig.add_subplot(111)
    RGBplot = colour.plotting.plot_chromaticity_diagram_CIE1931(standalone=False, axes=ax)
    ax.legend().set_visible(False)
    olddata = ""
    
    # loop for some time
    while (1):
        # continue to read from serial
        data = Serial.readline()
        if (olddata==data):
            continue
        olddata = data
        # isolate values
        split = str(data).rsplit(":")
        for i in range(len(split)):
            if i == 3:  #ir1
                split[i] = [int(i) for i in split[i].split() if i.isdigit()][0]
            elif i == 7:    #B "\n'"
                split[i] = int(split[i].strip("'\ n"))
            elif i != 0:
                split[i] = int(split[i].strip("XYZIRGB "))

        [a, x, y, z, ir, r, g, b] = split 
        '''
        arr_x.append( x / (x + y + z))
        arr_y.append( y / (x + y + z))
        arr_x.pop(0)
        arr_y.pop(0)'''
        #z=3*z
        #x=0.8*x
        XYZ = np.array([x,y,z])
        xy = colour.XYZ_to_xy(XYZ)
        dot=plt.plot(xy[0], xy[1], 'o', markerfacecolor='none', markeredgecolor='black')
        
        #RGB = np.array([r,g,b])

        #RGB = colour.models.eotf_inverse_sRGB(np.array([x,y,z]) / 255)

        # Plot RGB chromaticities
        #RGB_colors.append(RGB)#x / (x + y + z), y / (x + y + z), z / (x + y + z)])
        #RGB_colors.pop(0)

        print(XYZ)        
        #print(arr_x[-1],arr_y[-1])
        
        #fig.canvas.draw()
        plt.show()
        plt.pause(0.3)
        dot[0].set_data([], [])
        #plt.clf()
        #fig.canvas.flush_events()
        '''plt.pause(1)
        plt.clf()'''

else:
    print('serial not open')
Serial.close()  # close z1serial if z1serial is open.



