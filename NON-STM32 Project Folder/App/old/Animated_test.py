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
from numpy import random
import numpy as np
import tkinter as tki
import ctypes as ct
import cv2
import threading
import imutils
import datetime
import os
import time


class TestApp:

    def __init__(self, root):     
        self.root = root
        # Initialize left frame
        self.left_frame = tki.Frame(root)
        self.left_frame.pack(side= "left", padx =10, pady = 10)  #, fill="y", expand=True

        #save people's name and testing time
        self.name = ""
        self.name_list = []

        # Hold data
        self.xar = []
        self.yar = []

        # Set plot variables
        style.use('ggplot')
        self.fig = plt.figure(figsize=(5,5))
        self.ax1 = self.fig.add_subplot()
        self.ylim = 100
        self.ax1.set_ylim(0, self.ylim)
        self.line, = self.ax1.plot(self.xar, self.yar, 'r', marker='o')

        # Create plot canvas
        plotcanvas = FigureCanvasTkAgg(self.fig, root)
        plotcanvas.get_tk_widget().pack()       

        # Frame that holds wigets on the left side
        self.left_panel()
        
    def animate(self,i):
        self.yar.append(np.random.randint(self.ylim))   # Random inputs
        self.xar.append(i)  # Time
        self.line.set_data(self.xar, self.yar)
        if (i<5):
            self.ax1.set_xlim(0, i+1)
        else:
            self.ax1.set_xlim(i-5, i+1)

    def left_panel(self):
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
                          
    def update_StopWatch(self):
        if self.btn_st_idx == 2 or self.btn_st_idx == 3:
            self.StopWatch.configure(text=str(self.StopWatch_counting))
            self.StopWatch_counting += 1
        else:
            self.StopWatch_counting = 0
            self.StopWatch.configure(text=str(self.StopWatch_counting))
        self.root.after(1000, self.update_StopWatch)
   

if __name__ == "__main__":
    # Initialize app
    root = tki.Tk()
    app = TestApp(root)
    # Animate Color
    ani = animation.FuncAnimation(app.fig, app.animate, interval=1000, blit=False, save_count=50)
    root.mainloop()
        


'''import tkinter
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.backend_bases import key_press_handler
from matplotlib import pyplot as plt, animation
import numpy as np

# Set the figure size and adjust the padding between and around the subplots.
plt.rcParams["figure.figsize"] = [7.00, 3.50]
plt.rcParams["figure.autolayout"] = True

# Create a Toplevel widget of Tk which represents mostly the main window of an application
root = tkinter.Tk()
# Set the title of this widget.
root.wm_title("Embedding in Tk")

# Add an axes to the current figure and make it the current axes.
plt.axes(xlim=(0, 2), ylim=(-2, 2))

# Create a new figure or activate an existing figure.
fig = plt.Figure(dpi=100)
# Add an 'ax' to the figure as part of a subplot arrangement.
ax = fig.add_subplot(xlim=(0, 2), ylim=(-1, 1))
# Make a dummy line plot with linewidth=2.
line_r, = ax.plot([], [], lw=2)

# Create the canvas the figure renders into.
canvas = FigureCanvasTkAgg(fig, master=root)
# Create the figure canvas on which to operate.
canvas.draw()

toolbar = NavigationToolbar2Tk(canvas, root, pack_toolbar=False)
toolbar.update()

# Create a keypress event to quit the tkinter winter.
canvas.mpl_connect(
    "key_press_event", lambda event: print(f"you pressed {event.key}"))
canvas.mpl_connect("key_press_event", key_press_handler)

button = tkinter.Button(master=root, text="Quit", command=root.quit)
button.pack(side=tkinter.BOTTOM)

toolbar.pack(side=tkinter.BOTTOM, fill=tkinter.X)
canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

def init():
    line_r.set_data([], [])
    return line_r,

def animate(i):
    x = np.linspace(0, 2, 1000)
    y = np.sin(2 * np.pi * (x - 0.01 * i))
    line_r.set_data(x, y)
    return line_r,

# Create an animation by repeatedly calling a function *animate*.
anim = animation.FuncAnimation(fig, animate, init_func=init,frames=200, interval=20, blit=True)

tkinter.mainloop()
'''

"""
import matplotlib.pyplot as plt
import numpy as np
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from tkinter import ttk

x_data, y_data = [], []

class Win(tk.Tk):

    def __init__(self):
    
        super().__init__()
    
        self.title('Color liveplot')
        #self.geometry('500x450')

        # Frame that holds wigets on the left side
        left_frame = ttk.Frame(self)
        left_frame.pack(side= "left", padx =10, pady = 10)  #, fill="y", expand=True
     
        self.fig = plt.figure(figsize=(4, 3.5), dpi=100)
        self.ax = self.fig.add_subplot(1,1,1)
        self.line, = self.ax.plot([0], [0])
        self.ax.set_xlabel('Time / Sec', fontsize = 12)
        self.ax.set_ylabel('Intensity', fontsize = 12)
        self.fig.tight_layout()
        self.canvas = FigureCanvasTkAgg(self.fig, master=self)
        #self.toolbar = NavigationToolbar2Tk(self.canvas, self)
        self.canvas.get_tk_widget().pack(side= tk.BOTTOM)
    
    
        voltage_range_label = tk.Label(left_frame, text = "Voltage range")
        voltage_range_label.pack(side = "top", padx =10, pady =2)
        self.voltage_range = tk.IntVar()
        self.voltage_range.set(10)
        voltage_range_spinbox = ttk.Spinbox(left_frame, from_=-3e2, to = 5e2, textvariable = self.voltage_range, width=8)
        voltage_range_spinbox.pack(side="top", padx =10, pady =5)
    
        voltage_step_label = tk.Label(left_frame, text = "Step")
        voltage_step_label.pack(side = "top", padx =10, pady =2)
        self.step = tk.IntVar()
        self.step.set(1)
        step_spinbox = ttk.Spinbox(left_frame, from_=-3e2, to = 5e2, textvariable = self.step, width =9)
        step_spinbox.pack(side="top", padx =10, pady =5)
    
        self.start = tk.BooleanVar(value = False)
        start_butt = ttk.Button(left_frame, text="Start", command= lambda: self.start.set(True))
        start_butt.pack(side='top', padx =10, pady =10)
         
        stop_butt = ttk.Button(left_frame, text="Resume", command=lambda: self.is_paused.set(False))
        stop_butt.pack(side="top", padx =10, pady =10)    
    
        self.is_paused = tk.BooleanVar()  # variable to hold the pause/resume state
        restart_butt = ttk.Button(left_frame, text="Pause", command=lambda: self.is_paused.set(True))
        restart_butt.pack(side="top", padx =10, pady =10)

    def update(self, k=1):

        if self.start.get() and not self.is_paused.get(): 

            # quasi For Loop
            idx = [i for i in range(0, k, self.step.get())][-1]
            x_data.append(idx)
            y_data.append(np.sin(idx/5))
            self.line.set_data(x_data, y_data)
            self.fig.gca().relim()
            self.fig.gca().autoscale_view()
            self.canvas.draw()
            #self.canvas.flush_events()
            k += self.step.get()[["enter image description here"][2]][2]
         
        if k <= self.voltage_range.get():
        
            self.after(1000, self.update, k)


if __name__ == "__main__":
    app = Win()
    app.after(1000, app.update)
    app.mainloop()"""

"""from imutils.video import VideoStream
import argparse
import time

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-c", "--camera", type=int, default=-1, help="whether or not the camera should be used")
args = vars(ap.parse_args())

# initialize the video stream and allow the camera sensor to warmup
print("ToF opened")
print("[INFO] warming up camera...")
vs = VideoStream(usePiCamera=args["camera"] > 0).start()
time.sleep(2.0)

# start the app
pba = PhotoBoothApp(vs)
pba.root.mainloop()"""

'''fig, ax = plt.subplots()

x = np.arange(0, 2*np.pi, 0.01)
line, = ax.plot(x, np.sin(x))
x1 = np.arange(0, 2*np.pi, 0.01)
line1, = ax.plot(x1, np.sin(x1))

def animate(i):
    line.set_ydata(np.sin(x + i / 50))  # update the data.
    line1.set_ydata(np.cos(x1 + i / 50))  # update the data.
    return line,line1

ani = animation.FuncAnimation(
    fig, animate, interval=20, blit=True, save_count=50)

# To save the animation, use e.g.
#ani.save("movie.mp4")

plt.show()'''

