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


class PhotoBoothApp:
    mode = 4
    frame_rate = 15

    def __init__(self, vs):
        # store the video stream object and output path, then initialize
		# the most recently read frame, thread for reading frames, and
		# the thread stop event
        
        # video
        self.vs = vs
        self.frame = None
        
        # ToF/Color
        self.tof = ToF(self.mode, self.frame_rate)      #self.tof = TOFRanging(self.mode, self.frame_rate)
        #self.col = Color()      
        self.result = None

        # threads: //Future will use 2 threads, since Tof operation doesn't coincide with Color
        self.thread = None  # video
        self.thread_tof = None # ToF, Col save
        self.thread_plot = None # ToF, Col plot
        self.thread_data = None # Dynamic Data
        self.stopEvent = None # stop event
        
        #save video
        self.out = None # video output object

        #save people's name and testing time
        self.name = ""
        self.name_list = []

        # stop watch: Maybe don't need ###############################################
        #self.time_seconds = None
        self.StopWatch_counting = 0
        
        # save file name 
        #self.fileName = "data.txt" # May don't need ##################################################
        self.folderName = datetime.date.today().strftime("%Y-%m-%d")
        os.makedirs(self.folderName, exist_ok=True)
        
        # initialize the root window and image panel
        self.root = tki.Tk()
        self.panel = None
        self.panel_tof = None
        self.panel_col = None

        
        
		# start a thread that constantly pools the video sensor for
		# the most recently read frame
        self.stopEvent = threading.Event()
        self.thread = threading.Thread(target=self.videoLoop, args=())
        self.thread.start()

        self.thread_data = threading.Thread(target=self.tof.roll_data, args=())
        self.thread_data.start()
        
        self.thread_plot = threading.Thread(target=self.ToF_plot, args=())
        self.thread_plot.start()
        
        '''self.thread_tof = threading.Thread(target = self.w_tof, args=())
        self.thread_tof.start()'''
        # Frame that holds wigets on the left side
        self.left_frame = tki.Frame(self.root)
        self.left_frame.pack(side= "left", padx =10, pady = 10)  #, fill="y", expand=True
        
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

        '''# Clock
        self.Time = tki.Label(text = "")
        self.Time.pack(side="bottom", padx=10, pady=10)
        self.update_clock()'''
        
        # Stop watch 
        self.StopWatch = tki.Label(self.left_frame, text = "", font = ('Arial', 25))
        self.StopWatch.pack(side="bottom")
        self.update_StopWatch()
		# set a callback to handle when the window is closed
        self.root.wm_title("Distance Data Collection App")
        self.root.wm_protocol("WM_DELETE_WINDOW", self.onClose)

    '''def dynamic_data(self):
        while(1):
            self.tof.roll_data
            #self.col.animate
            time.sleep(1)'''

    def ToF_plot(self):
        
        while not self.stopEvent.is_set():
            result = self.tof.result
            result = result/(result.max())*255    #result needs to be in 0-255 range
            resized_result = cv2.resize(result, (200,200), interpolation = cv2.INTER_AREA)
            image_tof = Image.fromarray(resized_result)
            image_tof = ImageTk.PhotoImage(image_tof)
            
            '''col_result = self.col.result
            col_result = col_result/(col_result.max())*255    #result needs to be in 0-255 range
            col_resized_result = cv2.resize(col_result, (200,200), interpolation = cv2.INTER_AREA)
            image_col = Image.fromarray(col_resized_result)
            image_col = ImageTk.PhotoImage(image_col)'''
            # if the panel is not None, we need to initialize it
            if self.panel_tof is None:
                self.panel_tof = tki.Label(image=image_tof)
                self.panel_tof.image = image_tof
                self.panel_tof.pack(side="bottom", fill=tki.BOTH, expand=tki.YES)
            # otherwise, simply update the panel
            else:
                self.panel_tof.configure(image=image_tof)
                self.panel_tof.image = image_tof
            '''# if the panel is not None, we need to initialize it
            if self.panel_col is None:
                self.panel_col = tki.Label(image=image_col)
                self.panel_col.image = image_col
                self.panel_col.pack(side="right", fill=tki.BOTH, expand=tki.YES)
            # otherwise, simply update the panel
            else:
                self.panel_col.configure(image=image_col)
                self.panel_col.image = image_col'''

    def videoLoop(self):
		# DISCLAIMER:
		# I'm not a GUI developer, nor do I even pretend to be. This
		# try/except statement is a pretty ugly hack to get around
		# a RunTime error that Tkinter throws due to threading
        try:
			# keep looping over frames until we are instructed to stop
            while not self.stopEvent.is_set():
				# grab the frame from the video stream and resize it to
				# have a maximum width of 300 pixels
                self.frame = self.vs.read()
                self.frame = imutils.resize(self.frame, width=300)
                height, width, layers = self.frame.shape
                size = (width,height)
                
                if self.btn_st_idx == 1:
                    self.out = cv2.VideoWriter(self.folderName +"/" +self.name + '.avi',cv2.VideoWriter_fourcc(*'DIVX'), 45, size)
                elif self.btn_st_idx == 2 or self.btn_st_idx == 3:
                    self.out.write(self.frame)
                else:
                    if self.out is not None:
                        try:
                            self.out.release()
                        except RuntimeError:
                            print("[INFO] can't release video writing flow")
				# OpenCV represents images in BGR order; however PIL
				# represents images in RGB order, so we need to swap
				# the channels, then convert to PIL and ImageTk format
                image = cv2.cvtColor(self.frame, cv2.COLOR_BGR2RGB)
                image = Image.fromarray(image)
                image = ImageTk.PhotoImage(image)
		
				# if the panel is not None, we need to initialize it
                if self.panel is None:
                    self.panel = tki.Label(image=image)
                    self.panel.image = image
                    self.panel.pack(side="bottom", padx=10, pady=10)
		
				# otherwise, simply update the panel
                else:
                    self.panel.configure(image=image)
                    self.panel.image = image
                    
        except RuntimeError:
            print("[INFO] caught a RuntimeError")
            
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
        self.vs.stop()
        #self.tof.EndToF()
        #self.col.EndCol()
        #self.out.release()
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
        # ranging.ranging.argtypes = [ct.c_char_p, ct.c_char_p]
        while not self.stopEvent.is_set():
            time_stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S%f")
            #self.tof.Ranging()
            if self.btn_st_idx == 2:
                fileName = self.folderName +"/" +self.name + '.txt'
                with open(fileName, "a") as f:
                    f.write("time stamp: ")
                    f.write(time_stamp)
                    f.write(" ; pixels: ")
                    np.savetxt(f, self.tof.result.reshape((1,-1)), fmt='%5d')
                    np.savetxt(f, self.col.result.reshape((1,-1)), fmt='%5d')
#                     f.write("\n")        
#                self.tof.SaveRangeData(fileName)

     
            
class ToF:
    
    def __init__(self, mode, frame_rate):
        # Power up Time of Flight sensor
        #self.StartTOF()
        # Set frame rate and working mode (using C type formats for ToF driver) //not using driver for summer 23
        self.frame_rate = ct.c_int(frame_rate)
        self.mode = mode
        self.thread_data = None # Constantly changing data
        #self.result = np.zeros((self.mode,self.mode))
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
            #self.FreeGPIO()
            os._exit(0)    
        self.p_dev = None
        self.Ranging_ini()

    def roll_data(self):
        while(1):
            print("shift")
            self.result = np.roll(self.result, 1)
            time.sleep(1)
    
    def Ranging_ini(self):
        # Initial ToF and prepare for ranging
        class PResult(ct.Structure):
            _fields_ = [("result", ct.c_int16*(self.mode**2))]
        self.restype = ct.POINTER(PResult)
        self.p_dev = True
    
    def Ranging(self):
        # Ranging 
        #p_result = self.ranging(self.p_dev,self.Mod)
        self.result = np.reshape(self.result,(self.mode,self.mode))
    
    def StopRanging(self):
        # Stop ranging
        status = self.StopRanging(self.p_dev)

    def EndToF(self):
        # fast end
        self.StopRanging()
        #self.FreeGPIO()
    
    def SaveRangeData(self, fileName):
        # save the range data in to file "fileName"
        with open(fileName+"ToF", "a") as f:
            np.savetxt(f, self.result, fmt='%5d')
            f.write("\n")


class Color:
    
    def __init__(self):
        # Power up Color sensor
        #self.StartCol()

        self.xar = []
        self.yar = []
        self.i = 0

        style.use('ggplot')
        self.fig = plt.figure(figsize=(5, 5))
        self.ax1 = self.fig.add_subplot()
        self.ylim = 100
        self.ax1.set_ylim(0, self.ylim)
        self.line, = self.ax1.plot(self.xar, self.yar, 'r', marker='o')
    
    def animate(self):
        print("generate")
        self.yar.append(random.randint(0,self.ylim))
        self.xar.append(self.i)
        self.line.set_data(self.xar, self.yar)
        self.ax1.set_xlim(0, self.i+1)
    
    def SaveRangeData(self, fileName):
        # save the range data in to file "fileName"
        with open(fileName+"Color", "a") as f:
            np.savetxt(f, self.result, fmt='%5d')
            f.write("\n")

         
# from pyimagesearch.photoboothapp import PhotoBoothApp
from imutils.video import VideoStream
import argparse
import time

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
# ap.add_argument("-o", "--output", required=True, help="path to output directory to store snapshots")
ap.add_argument("-c", "--camera", type=int, default=-1, help="whether or not the camera should be used")
args = vars(ap.parse_args())

# initialize the video stream and allow the camera sensor to warmup
print("ToF opened")
print("[INFO] warming up camera...")
vs = VideoStream(usePiCamera=args["camera"] > 0).start()
time.sleep(2.0)

# start the app
pba = PhotoBoothApp(vs)
pba.root.mainloop()
        



