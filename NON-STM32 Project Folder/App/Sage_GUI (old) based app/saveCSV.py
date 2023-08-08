# -*- coding: utf-8 -*-      
# ^ to ensure text editor encodes properly
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


from PIL import ImageTk
from PIL import Image 
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import numpy as np
import tkinter as tki
import ctypes as ct
import cv2
import threading
import datetime
import os
import time
import csv


class PhotoBoothApp:
    mode = 4
        
    def __init__(self, root):   
        # Initialize left/right frames
        self.root = root
        self.Frame_init()

        # ToF and Color
        self.Color = Color()
        self.tof = ToF(self.mode)
        self.result = None

        # Initialize threads //Try to reduce, color plot must be on main thread bc of animation function?
        self.thread_save = None # ToF, Col save
        self.thread_plot = None # ToF plot
        self.thread_data = None # Dynamic Data, only for dummy data
        self.stopEvent = None # stop event
        self.dataEvent = None # stop event
                
        #save video
        self.out = None # video output object

        #Create file
        self.name = ""
        self.name_list = [] 

        # stop watch: Maybe don't need ###############################################
        self.StopWatch_counting = 0
        
        # Frame that holds wigets on the left side
        self.left_panel()  

		# start threads
        self.stopEvent = threading.Event()
        self.dataEvent = threading.Event()
        
        
        # initialize image panel
        self.panel_tof = None
        self.thread_plot = threading.Thread(target=self.ToF_plot, args=())
        self.thread_plot.start()

        # Create plot canvas
        plotcanvas = FigureCanvasTkAgg(self.Color.fig, self.right_frame)
        plotcanvas.get_tk_widget().pack(side="left", padx=10, pady=10)       

        # set a callback to handle when the window is closed
        self.root.wm_title("Sensor Data Collection App")
        self.root.wm_protocol("WM_DELETE_WINDOW", self.onClose)
    
    def save_init(self):
        # field names 
        self.pixels = [('PIXEL'+str(i)) for i in range(self.mode**2)]
        self.colors = ['RED','GREEN','BLUE']
        self.fields = ['TIMESTAMP'] 
        self.fields = np.append(self.fields,self.pixels)
        self.fields = np.append(self.fields,self.colors) 

        # Changing the current working directory
        print(self.name)
        self.fileName = self.name +"_mode"+str(self.mode)
        print(self.fileName)
        self.dirname = os.path.dirname(__file__)
        os.chdir(self.dirname)
        self.folderName = datetime.date.today().strftime("%Y-%m-%d")
        os.makedirs(self.folderName, exist_ok=True)
        self.fileName = self.folderName +"/" + self.fileName + '.csv'

    def save_data(self):    # Save incoming data into file

        # data rows as dictionary objects 
        self.dictlist =[]
        while not self.dataEvent.is_set():
            if self.btn_st_idx == 3:    
                self.get_data()
                with open(self.fileName, 'w', newline='') as file:
                    writer = csv.DictWriter(file, fieldnames = self.fields)  
                    writer.writeheader() 
                    writer.writerows(self.dictlist)
  
    def get_data(self): #copy data from color and tof
        mydict = {}
        time_stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S%f")
        pixels = self.tof.send_data()
        rgb = self.Color.send_data()
        mydict = pixels.copy()
        mydict['TIMESTAMP']=time_stamp
        mydict['RED']=rgb['RED']
        mydict['GREEN']=rgb['GREEN']
        mydict['BLUE']=rgb['BLUE']
        self.dictlist.append(mydict)

    def Frame_init(self):   # Create left and right frames
        self.left_frame = tki.Frame(root)
        self.right_frame = tki.Frame(root)
        self.left_frame.pack(side= "left", padx =10, pady = 10)  #, fill="y", expand=True
        self.right_frame.pack(side= "right", padx =10, pady = 10)  #, fill="y", expand=True

    def left_panel(self):   # Set left frame widgets
        # create a button, that when pressed, will take the current
		# frame and save it to file
        self.btn_st = ["Start ToF","Enter Name","Record","End recording","..."]
        self.btn_st_idx = 0 # this can be 0,1,2
        self.btn_tg = ["Current Mode: 4x4","Current Mode: 8x8","..."]
        self.btn_tg_idx = 0 # this can be 0,1,2
        '''self.btn_mg = ["Name your file"," Ready to record", " recording...", "reseted, enter new name", "..."]
        self.btn_mg_idx = 0 # this can be 0,1,2,3'''

        # button
        self.btn = tki.Button(self.left_frame, text=self.btn_st[self.btn_st_idx], command=self.btn_fun)
        self.btn.pack(side="bottom", padx=10, pady=10)
        self.btn1 = tki.Button(self.left_frame, text=self.btn_tg[self.btn_tg_idx], command=self.btn_toggle)
        self.btn1.pack(side="bottom", padx=10, pady=10)
        
        # enter name
        self.entry = tki.Entry(self.left_frame)
        self.entry.pack(side="bottom", fill="both", padx=10, pady=10)
        
        # hint to enter name
        self.Message = tki.Label(self.left_frame, text="Name your file")
        self.Message.pack(side="bottom")
        
        # Stop watch 
        self.StopWatch = tki.Label(self.left_frame, text = "{:.2f}".format(0), font = ('Arial', 25))
        self.StopWatch.pack(side="bottom")
        self.start_time = time.time()
        self.update_StopWatch()
    
    def destroy_left_panel(self):   
        # button
        self.btn.pack_forget()
        self.btn1.pack_forget()
        
        # enter name
        self.entry.pack_forget()
        
        # hint to enter name
        self.Message.pack_forget()
        
        # Stop watch 
        self.StopWatch.pack_forget()

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
                self.panel_tof.pack(side="right", fill=tki.BOTH, expand=tki.YES, padx=10, pady=10)
            # otherwise, simply update the panel
            else:
                self.panel_tof.configure(image=image_tof)
                self.panel_tof.image = image_tof
       
    def btn_fun(self):
        if self.btn_st_idx == 0: # Set resolution
            self.btn_st_idx = 1
            self.prev_tg_idx = self.btn_tg_idx
            self.btn1.pack_forget()
            self.thread_data = threading.Thread(target=self.tof.roll_data, args=())
            self.thread_data.start()
        elif self.btn_st_idx == 1: # state 1 : enter name
            self.name = self.entry.get()
            if self.name == '':
                self.name = datetime.datetime.now().strftime("%Y%m%d_%H%M%S%f")
            self.Message.configure(text = self.name+" Ready to record")
            same_name_num = self.name_list.count(self.name)
            self.name_list.append(self.name)
            if same_name_num > 0:
                self.name = self.name+str(same_name_num)
            self.save_init()
            self.btn_st_idx = 2
        elif self.btn_st_idx == 2: # state 2 : click to record data
            # Save data
            self.dataEvent.clear()
            print('cleared')
            self.thread_save = threading.Thread(target = self.save_data, args=())
            self.thread_save.start()
            self.btn_st_idx = 3
            self.Message.configure(text = " recording...")
            print('recording')
            
        else:   # reset
            print('end recording')
            self.dataEvent.set()
            print('set')
            self.btn_st_idx = 0
            self.Message.configure(text = "reseted, enter new name")
            self.destroy_left_panel()
            self.left_panel()
            self.btn1['text'] = self.btn_tg[self.prev_tg_idx]  
            self.btn_tg_idx = self.prev_tg_idx
            self.tof = ToF(self.mode)
        self.btn['text'] = self.btn_st[self.btn_st_idx]
    
    def btn_toggle(self):   # toggle between 4x4 and 8x8
        if self.btn_tg_idx == 0:  # 4x4
            self.mode = 8
            self.tof = ToF(self.mode)
            self.btn_tg_idx = 1
        else:   #8x8
            self.mode = 4
            self.tof = ToF(self.mode)
            self.btn_tg_idx = 0
        self.btn1['text'] = self.btn_tg[self.btn_tg_idx]
  
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
        if self.btn_st_idx == 3:
            self.elapsed_time = time.time() - self.start_time            
            self.StopWatch.configure(text=str("{:.2f}".format(self.elapsed_time)))
            self.StopWatch_counting += 1
        else:
            self.start_time = time.time()
            self.StopWatch.configure(text=str("{:.2f}".format(0)))
        self.root.after(50, self.update_StopWatch)

    def update_clock(self):
        now = time.strftime("%H:%M:%S")
        self.Time.configure(text=now)
        self.root.after(1000, self.update_clock)


class ToF:
    
    def __init__(self, mode):
        # Power up Time of Flight sensor
        #self.StartTOF()
        self.mode = mode
        #self.thread_data = None # Constantly changing data
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
            self.result = np.roll(self.result, 1)
            time.sleep(1)

    def send_data(self):
        tofDict={}
        results = self.result.flatten()
        for i in range(self.mode**2):
            pixel_str = 'PIXEL'+str(i)
            tofDict[pixel_str]=results[i]
        return tofDict


class Color:
    def __init__(self):
        # Setup Axes
        style.use('ggplot')

        self.fig = plt.figure(figsize=(7,5))
        self.fig.tight_layout()
        self.ax1 = self.fig.add_subplot()

        self.ylim = 255
        self.ax1.set_ylim(0, self.ylim)
        self.ax1.set_title('Color', fontsize = 12)
        self.ax1.set_xlabel('Time ( Sec )', fontsize = 12)
        self.ax1.set_ylabel('Intensity', fontsize = 12)

        # CLEAR
        # RED
        self.xar = []
        self.yar = []
        self.line, = self.ax1.plot(self.xar, self.yar, 'r', marker='o')
        # GREEN
        self.xar1 = []
        self.yar1 = []
        self.line1, = self.ax1.plot(self.xar1, self.yar1, 'g', marker='o')
        # BLUE
        self.xar2 = []
        self.yar2 = []
        self.line2, = self.ax1.plot(self.xar2, self.yar2, 'b', marker='o')
        # PROX
        
    def animate(self,i):
        #CLEAR
        #RED
        self.yar.append(np.random.randint(self.ylim))   # Random inputs
        self.xar.append(i)  # Time
        self.line.set_data(self.xar, self.yar)
        #GREEN
        self.yar1.append(np.random.randint(self.ylim))   # Random inputs
        self.xar1.append(i)  # Time
        self.line1.set_data(self.xar1, self.yar1)
        #BLUE
        self.yar2.append(np.random.randint(self.ylim))   # Random inputs
        self.xar2.append(i)  # Time
        self.line2.set_data(self.xar2, self.yar2)
        if (i<6):
            self.ax1.set_xlim(0, i+1)
        else:
            self.ax1.set_xlim(i-6, i+1)

    def send_data(self):
        colDict = {'RED':self.yar[-1],
                   'GREEN':self.yar1[-1],
                   'BLUE':self.yar2[-1]}
        return colDict


# initialize the video stream and allow the camera sensor to warmup
print("ToF opened")
print("[INFO] warming up camera...")
time.sleep(2.0)

# Initialize app
root = tki.Tk()
root.resizable(False,False)
pba = PhotoBoothApp(root)
# Animate Color
ani = animation.FuncAnimation(pba.Color.fig, pba.Color.animate, interval=1000, blit=False, save_count=50)
pba.root.mainloop()
        
