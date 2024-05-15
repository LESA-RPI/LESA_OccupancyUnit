import asyncio

import numpy
import numpy as np
import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
from bleak import BleakClient, BleakScanner
import threading
import queue
from matplotlib.colors import Normalize
import matplotlib.cm as cm
import matplotlib.gridspec as gridspec
import matplotlib.image as mpimg
import colour.plotting as cp

# Characteristic UUID and MAC address of the device
par_notification_characteristic = "0000fe42-8e22-4541-9d4c-21edae82ed19"
par_device_addr = "00:80:E1:26:FD:A2"

class ToFGUI:
    def __init__(self, master):
        self.master = master
        self.data_queue = queue.Queue()
        master.title("ToF and CIE Plot Display")

        self.cie_point = None
        self.cie_vector = None

        # Create a matplotlib figure with GridSpec for layout management
        self.fig = Figure(figsize=(10, 5), dpi=100)
        gs = gridspec.GridSpec(1, 3, width_ratios=[2, 0.5, 2], wspace=0.2)

        # Setup the axes for the heatmap and CIE plot
        self.ax_heatmap = self.fig.add_subplot(gs[0, 0])
        self.ax_cie = self.fig.add_subplot(gs[0, 2])
        cp.plot_chromaticity_diagram_CIE1931(standalone=False, axes=self.ax_cie)

        # Setup heatmap and CIE plot
        self.setup_heatmap()
        self.setup_cie_plot()

        # Create a canvas and add it to the main window
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.master)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        # Schedule an update
        self.master.after(200, self.update_gui)

    def setup_heatmap(self):
        # Setup heatmap configure aesthetics
        self.heatmap = self.ax_heatmap.imshow(numpy.zeros(shape=(8, 8)), cmap=cm.jet_r, norm=Normalize(vmin=0, vmax=2000), interpolation='nearest')
        self.ax_heatmap.set_title('Time of Flight Heatmap', fontsize=14)
        self.ax_heatmap.set_xlabel('Zone', fontsize=14)
        self.ax_heatmap.set_ylabel('Zone', fontsize=14)
        self.ax_heatmap.tick_params(axis='both', which='major', labelsize=14)

        # Setup colorbar and configure aesthetics
        cb_ax = self.fig.add_axes([0.45, 0.21, 0.02, 0.55])
        self.colorbar = plt.colorbar(self.heatmap, cax=cb_ax)
        self.colorbar.set_label('Distance (mm)')
        self.colorbar.set_label('Distance (mm)', fontsize=14)
        self.colorbar.ax.tick_params(labelsize=14)

    def setup_cie_plot(self):
        # Configure plot aesthetics
        self.ax_cie.set_title('CIE 1931 Chromaticity Diagram', fontsize=14)
        self.ax_cie.tick_params(axis='both', which='major', labelsize=14)
        self.ax_cie.set_xlim(0, 0.8)
        self.ax_cie.set_ylim(0, 0.9)
        self.ax_cie.set_xlabel('x', fontsize=16)
        self.ax_cie.set_ylabel('y', fontsize=16)

        # Set initial color point and vector
        self.cie_point, = self.ax_cie.plot(0.31271, 0.32902, 'ro', markersize=12)
        self.cie_vector = self.ax_cie.annotate(
            '', xy=(0.31271, 0.32902), xytext=(0.31271, 0.32902),
            arrowprops=dict(arrowstyle='->', lw=2.5, color='black')
        )

    def update_gui(self):
        try:
            while not self.data_queue.empty():
                data = self.data_queue.get_nowait()
                self.update_heatmap(data['ToF'])
                self.update_cie_plot(data['XYZ'])
                self.canvas.draw_idle()
        finally:
            self.master.after(100, self.update_gui)

    def update_heatmap(self, data):
        data = np.array(data).reshape(8, 8)

        self.heatmap.set_data(data)
        self.heatmap.set_norm(Normalize(vmin=0, vmax=2000))

        self.heatmap.changed()

    def update_cie_plot(self, xyz_data):
        X, Y, Z = xyz_data
        if X + Y + Z == 0:
            return
        x = X / (X + Y + Z)
        y = Y / (X + Y + Z)

        # Remove the previous point if it exists
        if hasattr(self, 'cie_point') and self.cie_point in self.ax_cie.lines:
            self.cie_point.remove()

        # Remove the previous vector if it exists
        #if hasattr(self, 'cie_vector') and self.cie_vector in self.ax_cie.artists:
        self.cie_vector.remove()

        # Update the color point
        self.cie_point, = self.ax_cie.plot(x, y, 'ro', markersize=12)

        # Update the color vector
        self.cie_vector = self.ax_cie.annotate(
                            '', xy=(x, y), xytext=(0.31271, 0.32902),
                            arrowprops=dict(arrowstyle='->', lw=2.5, color='black')
                        )

def notification_handler(characteristic, data):
    X = int.from_bytes(data[0:2], byteorder='little')
    Y = int.from_bytes(data[2:4], byteorder='little')
    Z = int.from_bytes(data[4:6], byteorder='little')
    IR = int.from_bytes(data[6:8], byteorder='little')

    ToFData = [int.from_bytes(data[i:i+2], byteorder='little') for i in range(8, 136, 2)]

    XYZData = [X, Y, Z]
    gui.data_queue.put({'ToF': ToFData, 'XYZ': XYZData})

async def run_bleak(gui):
    device = await BleakScanner.find_device_by_address(par_device_addr)
    if device is None:
        print(f"Could not find device with address '{par_device_addr}'")
        return

    async with BleakClient(device) as client:
        print("Connected")
        await client.start_notify(par_notification_characteristic, notification_handler)
        while True:
            await asyncio.sleep(0.2)

if __name__ == "__main__":
    root = tk.Tk()
    gui = ToFGUI(root)

    bleak_thread = threading.Thread(target=lambda: asyncio.run(run_bleak(gui)), daemon=True)
    bleak_thread.start()

    root.mainloop()
