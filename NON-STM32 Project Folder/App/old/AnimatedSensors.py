# -*- coding: utf-8 -*-      
# ^ to ensure text editor encodes properly
# ToFApp.py -> testapp.py
"""
This will be a test to experiment with
the app code from the LESA Sage GUI project

TOF orientation:

|-----------------------|
|                       |
|      15 11  7  3      |
|      14 10  6  2      |
|      13  9  5  1      |
|      12  8  4  0      |
|-----------------------|
"""

# GUI_tk_pil.py

from PIL import ImageTk
from PIL import Image 
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import random
import numpy as np
import tkinter as tki
#from tkinter import *
import ctypes as ct
import cv2
import threading
import imutils
import datetime
import os
import time
import argparse


class PhotoBoothApp:
    mode = 4
        
    def __init__(self, root):   
        # Initialize left/right frames
        self.root = root
        self.Frame_init()

        # ToF and Color
        self.Color_init()
        self.tof = ToF(self.mode)
        self.result = None

        #save people's name and testing time
        self.name = ""
        self.name_list = []

        # Initialize threads //Try to reduce, color plot must be on main thread bc of animation function?
        #self.thread_tof = None # ToF, Col save
        self.thread_plot = None # ToF plot
        self.thread_data = None # Dynamic Data, only for dummy data
        self.stopEvent = None # stop event
                
        #save video
        self.out = None # video output object

        # stop watch: Maybe don't need ###############################################
        self.StopWatch_counting = 0
        
        # save file name 
        #self.fileName = "data.txt" # May don't need ##################################################
        self.folderName = datetime.date.today().strftime("%Y-%m-%d")
        os.makedirs(self.folderName, exist_ok=True)
        
		# start threads
        self.stopEvent = threading.Event()
        self.thread_data = threading.Thread(target=self.tof.roll_data, args=())
        self.thread_data.start()
        
        # initialize image panel
        self.panel_tof = None
        self.thread_plot = threading.Thread(target=self.ToF_plot, args=())
        self.thread_plot.start()

        # Create plot canvas
        plotcanvas = FigureCanvasTkAgg(self.fig, self.right_frame)
        plotcanvas.get_tk_widget().pack(side="left")       

        # Frame that holds wigets on the left side
        self.left_panel()  
        
        '''self.thread_tof = threading.Thread(target = self.w_tof, args=())
        self.thread_tof.start()'''   

        # set a callback to handle when the window is closed
        self.root.wm_title("Sensor Data Collection App")
        self.root.wm_protocol("WM_DELETE_WINDOW", self.onClose)
    
    
    def Color_init(self):   # Initialize Color variables
        # Setup Axes
        style.use('ggplot')

        self.fig = plt.figure(figsize=(5,5))
        self.fig.tight_layout()
        self.ax1 = self.fig.add_subplot()

        self.ylim = 255
        self.ax1.set_ylim(0, self.ylim)
        self.ax1.set_title('Color', fontsize = 12)
        self.ax1.set_xlabel('Time / Sec', fontsize = 12)
        self.ax1.set_ylabel('Intensity', fontsize = 12)

        # RGB, Clear, ALS, Prox
        # RED
        self.xar = []
        self.yar = []
        self.line, = self.ax1.plot(self.xar, self.yar, 'r', marker='o')
        # BLUE
        self.xar1 = []
        self.yar1 = []
        self.line1, = self.ax1.plot(self.xar1, self.yar1, 'b', marker='o')
        # GREEN
        self.xar2 = []
        self.yar2 = []
        self.line2, = self.ax1.plot(self.xar2, self.yar2, 'g', marker='o')
        
    def animate(self,i):
        #RED
        self.yar.append(np.random.randint(self.ylim))   # Random inputs
        self.xar.append(i)  # Time
        self.line.set_data(self.xar, self.yar)
        #BLUE
        self.yar1.append(np.random.randint(self.ylim))   # Random inputs
        self.xar1.append(i)  # Time
        self.line1.set_data(self.xar1, self.yar1)
        #GREEN
        self.yar2.append(np.random.randint(self.ylim))   # Random inputs
        self.xar2.append(i)  # Time
        self.line2.set_data(self.xar2, self.yar2)
        if (i<5):
            self.ax1.set_xlim(0, i+1)
        else:
            self.ax1.set_xlim(i-5, i+1)

    def Frame_init(self):   # Create left and right frames
        self.left_frame = tki.Frame(root)
        self.right_frame = tki.Frame(root)
        self.left_frame.pack(side= "left", padx =10, pady = 10)  #, fill="y", expand=True
        self.right_frame.pack(side= "right", padx =10, pady = 10)  #, fill="y", expand=True

    def left_panel(self):   # Set left frame widgets
        # create a button, that when pressed, will take the current
		# frame and save it to file
        self.btn_st = ["Enter Name", "Record", "End recording","..."]
        self.btn_st_idx = 0 # this can be 0,1,2

        # button
        self.btn = tki.Button(self.left_frame, text=self.btn_st[self.btn_st_idx], command=self.btn_fun)
        self.btn.pack(side="bottom", padx=10, pady=10)
        
        # enter name
        self.entry = tki.Entry(self.left_frame)
        self.entry.pack(side="bottom", fill="both", padx=10, pady=10)
        
        # hint to enter name
        self.Message = tki.Label(self.left_frame, text="Name your file")
        self.Message.pack(side="bottom")
        
        # Stop watch 
        self.StopWatch = tki.Label(self.left_frame, text = "", font = ('Arial', 25))
        self.StopWatch.pack(side="bottom")
        self.update_StopWatch()
            
    def ToF_plot(self):
        
        while not self.stopEvent.is_set():
            result = self.tof.result
            result = result/(result.max())*255    #result needs to be in 0-255 range
            resized_result = cv2.resize(result, (300,300), interpolation = cv2.INTER_AREA)
            image_tof = Image.fromarray(resized_result)
            image_tof = ImageTk.PhotoImage(image_tof)
            
            # if the panel is not None, we need to initialize it
            if self.panel_tof is None:
                self.panel_tof = tki.Label(self.right_frame, image=image_tof)
                self.panel_tof.image = image_tof
                self.panel_tof.pack(side="right", fill=tki.BOTH, expand=tki.YES)
            # otherwise, simply update the panel
            else:
                self.panel_tof.configure(image=image_tof)
                self.panel_tof.image = image_tof
       
    def btn_fun(self):
        if self.btn_st_idx == 0: # state 0 : enter time and name
            self.name = self.entry.get()
            self.Message.configure(text = self.name+" Ready to record")
            same_name_num = self.name_list.count(self.name)
            self.name_list.append(self.name)
            if same_name_num > 0:
                self.name = self.name+str(same_name_num)
            
            self.btn_st_idx = 1
     
        elif self.btn_st_idx == 1: # state 1 : click to record data
            self.btn_st_idx = 2
            self.Message.configure(text = " recording")
            
        elif self.btn_st_idx == 2 or self.btn_st_idx == 3:
            self.btn_st_idx = 0
        
        else: # reset
            self.btn_st_idx = 0
            self.Message.configure(text = "reseted, enter new name")
            
        self.btn['text'] = self.btn_st[self.btn_st_idx]
                
    def onClose(self):
		# set the stop event, cleanup the camera, and allow the rest of
		# the quit process to continue
        print("[INFO] closing...")
        self.stopEvent.set()
        time.sleep(1.0)
        self.root.quit()
        self.root.destroy()
        os._exit(0)
                
    def update_StopWatch(self):
        if self.btn_st_idx == 2 or self.btn_st_idx == 3:
            self.StopWatch.configure(text=str(self.StopWatch_counting))
            self.StopWatch_counting += 1
        else:
            self.StopWatch_counting = 0
            self.StopWatch.configure(text=str(self.StopWatch_counting))
        self.root.after(1000, self.update_StopWatch)
            
    def update_clock(self):
        now = time.strftime("%H:%M:%S")
        self.Time.configure(text=now)
        self.root.after(1000, self.update_clock)
    
    def w_tof(self):
        while not self.stopEvent.is_set():
            time_stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S%f")
            if self.btn_st_idx == 2:
                fileName = self.folderName +"/" +self.name + '.txt'
                with open(fileName, "a") as f:
                    f.write("time stamp: ")
                    f.write(time_stamp)
                    f.write(" ; pixels: ")
                    np.savetxt(f, self.tof.result.reshape((1,-1)), fmt='%5d')
                    np.savetxt(f, self.col.result.reshape((1,-1)), fmt='%5d')
     
            
class ToF:
    
    def __init__(self, mode):
        # Power up Time of Flight sensor
        #self.StartTOF()
        self.mode = mode
        self.thread_data = None # Constantly changing data
        if mode == 4:
            self.Is_4x4 = ct.c_bool(True) 
            self.Mod = ct.c_int(4)
            self.result = np.array([[10*((i+1)+4*j) for i in range(4)] for j in range(4)])  #dummy data
        elif mode == 8:
            self.Is_4x4 = ct.c_bool(False)
            self.Mod = ct.c_int(8)
            self.result = np.array([[10*((i+1)+8*j)  for i in range(8)] for j in range(8)]) #dummy data
        else:
            print("error ToF working mode")
            os._exit(0)    

    def roll_data(self):
        while(1):
            print("shift")
            self.result = np.roll(self.result, 1)
            time.sleep(1)


# initialize the video stream and allow the camera sensor to warmup
print("ToF opened")
print("[INFO] warming up camera...")
time.sleep(2.0)

# Initialize app
root = tki.Tk()
pba = PhotoBoothApp(root)
# Animate Color
ani = animation.FuncAnimation(pba.fig, pba.animate, interval=1000, blit=False, save_count=50)
pba.root.mainloop()
        


