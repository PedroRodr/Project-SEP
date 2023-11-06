import tkinter as tk
from tkinter import Canvas, Label, StringVar, Text, Variable, ttk, messagebox
from PIL import ImageTk, Image
import serial
import sys
import glob
from serial.tools.list_ports import comports
import time

class App(tk.Tk):

    def _init_(self):
        super()._init_()
        self.title('GUI_SEP')
        # layout on the root window
        self.columnconfigure(0, weight=4)
        self.columnconfigure(1, weight=1)

        # background of the root window
        self.config(bg='white')

        self.__create_widgets()

    def __create_widgets(self):

        logo_frame = LogoFrame(self)
        logo_frame.grid(column=0, row=0, sticky='nw')
        # create the input frame
        BUTTON_frame = BUTTONframe(self, logo_frame)
        BUTTON_frame.grid(column=0, row=2, sticky='sw')
        text_frame = TextFrame(self,BUTTON_frame)
        text_frame.grid(column=0, row=1, sticky='nw')

        for widget in self.winfo_children():
            widget.grid(padx=30, pady=10)


class LogoFrame(tk.Frame):
    def _init_(self, container):
        super()._init_(container)
        self.config(bg='white')
        self.__create_widgets()

    def callback(self, event):
        print(self.menu.get())
        self.com = self.menu.get()
        self.port = serial.Serial(self.com, 9600, timeout=10)
        print(self.port.name)

    def __create_widgets(self):
        self.img = ImageTk.PhotoImage(Image.open(
            'ist.png').resize((100, 97), Image.Resampling.LANCZOS))
        self.canvas = tk.Canvas(
            self, bg="white", width=250, height=97, bd=0, highlightthickness=0)
        self.canvas.pack()
        self.canvas.create_image(75, 0, image=self.img, anchor='nw')

        self.aval_ports = serial_ports()
   
        self.com = 0

        self.menu = ttk.Combobox(
            self, values=self.aval_ports)
        self.menu.set('Please choose the COM port')
        self.menu.bind("<<ComboboxSelected>>", self.callback)
        self.menu.pack(side='bottom', fill='x', pady=(20,0))

class TextFrame(tk.Frame):

    def _init_(self, container, BUTTON_frame):
        super()._init_(container)
        self.BUTTON_FRAME=BUTTON_frame
        self.config(bg='white')
        self.__create_widgets()

    def __create_widgets(self):
        global receivedbyte
        global label
        label = Label(self,background='white' ,text= f"Luminosity:{receivedbyte}")
        label.config(font=("Courier", 14))
        label.pack()



class BUTTONframe(tk.Frame):

    def _init_(self, container, logo_frame):
        super()._init_(container)
        # setup the grid layout manager
        self.logo = logo_frame
        self.columnconfigure(1, weight=1)
        self.config(bg='white')
        self.__create_widgets()


    def button_command(self):
        global receivedbyte
        if self.logo.com == 0:
            messagebox.showwarning('Warning', 'Please select COM port')
        else:

            # Send the character 'H' over the serial port
            self.logo.port.write(b'H')  # b'H' converts the string 'H' to bytes
            # Introduce a small delay (e.g., 0.5 seconds) to allow time for the response
            time.sleep(0.5)
            self.button.config(image=self.off_switch)
            h=self.logo.port.readline()
            h=h.decode("utf-8")
            receivedbyte=h
            label.configure(text=f"Luminosity: {h}")
            print(f"Luminosity: {h}")


    def __create_widgets(self):

        self.off_switch = ImageTk.PhotoImage(Image.open(
            'button.png').resize((51, 51), Image.Resampling.LANCZOS))

        # Button frame, label and button init
        self.button_frame = tk.Frame(self)
        self.button_frame.config(background='white')
        self.button_frame.grid(column=0, row=2, sticky='we', padx=(60, 0))
        self.button_label = tk.Label(self.button_frame, text='Measure Luminosity', background='white')
        self.button_label.pack(side='bottom')
        self.button = tk.Button(self.button_frame, background='white', bd=0,highlightthickness=0, command=self.button_command, image=self.off_switch)
        self.button.pack(side='bottom')


def serial_ports():
        """ Lists serial port names

            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result


receivedbyte=''
label=''

if __name__ == "__main__":
    app = App()
    app.mainloop()