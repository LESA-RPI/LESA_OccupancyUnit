import serial
import serial.tools.list_ports
import tkinter as tk
import matplotlib.pyplot as plt
import numpy as np
import collections

# Get a list of available COM ports
com_ports = [port.device for port in serial.tools.list_ports.comports()]

# Set default baud rate
baud_rate = 115200

# FrameInfo and BlobInfo namedtuples
FrameInfo = collections.namedtuple("FrameInfo", ["num_blobs", "delta_X", "delta_Y"])
BlobInfo = collections.namedtuple("BlobInfo", ["id", "x", "y", "height", "velocity", "direction"])

# List to store blobs
blobs = []

# 8x8 grid for matrix data
grid = np.zeros((8, 8))

# Create the main window
root = tk.Tk()
root.title("COM Port Reader")

# Create the figure, axes, and canvas for the grid plot
fig, ax = plt.subplots()
cax = ax.imshow(grid, interpolation='nearest', cmap='hot', vmin=-100, vmax=4000)
fig.colorbar(cax)

# Create a label and dropdown for selecting the COM port
com_label = tk.Label(root, text="Select COM Port:")
com_label.pack()
com_var = tk.StringVar(root)
com_var.set(com_ports[0])
com_dropdown = tk.OptionMenu(root, com_var, *com_ports)
com_dropdown.pack()

# Create a label and dropdown for selecting the baud rate
baud_label = tk.Label(root, text="Select Baud Rate:")
baud_label.pack()
baud_var = tk.StringVar(root)
baud_var.set(baud_rate)
baud_dropdown = tk.OptionMenu(root, baud_var, 9600, 19200, 38400, 57600, 115200, 1000000)
baud_dropdown.pack()

# Function to update the baud rate when changed in the dropdown
def update_baud(*args):
    global baud_rate
    baud_rate = int(baud_var.get())

# Trace changes to the baud rate variable
baud_var.trace("w", update_baud)

# Function to read data from the selected COM port when the button is clicked
def read_com_port():
    com_port = com_var.get()
    ser = serial.Serial(com_port, baud_rate, timeout=0)
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line.startswith('##FRAME##'):
                handle_frame_info(line.split('|'))
            elif line.startswith('##BLOBS##'):
                handle_blob_info(line.split('|'))
            elif line.startswith('##Matrix##'):
                handle_matrix_info(line.split('|'))
            elif line.startswith('NEWFRAMEEND'):
                handle_new_frame(line.split('|'))
        except:
            UnicodeDecodeError
        

def handle_frame_info(data):
    frame_info = FrameInfo(*map(float, data[1:4]))
    #print(frame_info)
    ax.cla()
    cax = ax.imshow(grid, interpolation='nearest', cmap='hot', vmin=-100, vmax=2500)
    ax.set_xlim(-0.5, 7.5)
    ax.set_ylim(-0.5, 7.5)
    for blob in blobs:
        ax.plot(blob.x, blob.y, 'bo')  # blue dot for center
        ax.arrow(blob.x, blob.y, np.cos(blob.direction), np.sin(blob.direction), fc="k", ec="k", head_width=0.05, head_length=0.1 )  # black arrow for direction
    fig.canvas.draw()
    blobs.clear()

def handle_blob_info(data):
    blob_info = BlobInfo(*map(float, data[1:7]))
    #print(blob_info)
    blobs.append(blob_info)

def handle_matrix_info(data):
    global grid
    grid = np.array(list(map(float, data[1:65])))        
    grid = np.reshape(grid, (8,8))
    cax.set_array(grid)
    fig.canvas.draw()  
    

def handle_new_frame(data):
    fig.canvas.draw_idle()
    plt.pause(0.0001)


# Create a button to start reading from the selected COM port
read_button = tk.Button(root, text="Read COM Port", command=read_com_port)
read_button.pack()

# Run the main loop
root.mainloop()

