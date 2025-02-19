#import GUI
import tkinter as tk
from tkinter import font as tkfont
from tkinter import *

# Library voor grafieken
import matplotlib
import matplotlib.animation as animation

# Library om matplotlib in de venster te krijgen
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

# Library voor logo
from PIL import ImageTk, Image

#Verbinding met Arduino
import serial

#COM6 weergeven op startpagina
interface = ""

# Instellen van de grafiek
fig = Figure(figsize=(50, 50), dpi=100)
fig2 = Figure(figsize=(50, 50), dpi=100)
ax1 = fig.add_subplot(1, 1, 1)
ax2 = fig2.add_subplot(1, 1, 1)

#verbinding met arduino
def connectArduino(i):
    global interface
    #als bij de dropdownmenu op com6 klikt wordt een 1 meegegeven waardoor er
    #verbinden wordt gemaakt met com6, met een 2 COM8
    if i == 1:
        arduinoData = serial.Serial('com6', 19200)
        interface = "COM6"
        label["text"] = "Main page " + interface + "\nStatus: " + str(text) + " %"
    elif i == 2:
        arduinoData = serial.Serial('com8', 19200)
        interface = "COM8"
        label["text"] = "Main page " + interface + "\nStatus: " + str(text) + " %"


# Cijfers lezen uit een text file voor de lichtsensor.
def lichtsensorGraph(i):
    #data wordt uit een txt file gehaald en gesplitst
    #de gesplitste waardes worden in xar en yar gestopt
    pullData = open("Licht.txt", "r").read()
    dataArray = pullData.split('\n')
    xar = [] # list voor x 
    yar = [] # list voor y
    for eachLine in dataArray:
        if len(eachLine) > 1: 
            x, y = eachLine.split(',')
            xar.append(int(x))#Toevoegen aan list xar
            yar.append(int(y))#Toevoegen aan list yar
    ax1.clear()
    ax1.plot(xar, yar)


# Cijfers lezen uit een text file voor de Temperatuursensor.
def temperatuurGraph(i):
    #data wordt uit een txt file gehaald en gesplitst
    #de gesplitste waardes worden in xar en yar gestopt
    pullData = open("Temperatuur.txt", "r").read()
    dataArray = pullData.split('\n')
    xar = []
    yar = []
    for eachLine in dataArray:
        if len(eachLine) > 1:
            x, y = eachLine.split(',')
            xar.append(int(x))#Toevoegen aan list xar
            yar.append(int(y))#Toevoegen aan list yar
    ax2.clear()
    ax2.plot(xar, yar)


class GUI(tk.Tk):

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        self.title("HarmonicaIT ")# Title van de gui
        self.iconbitmap('image/test.ico')
        self.title_font = tkfont.Font(family='Comic', size=18)
        self.onder_font = tkfont.Font(family='Comic', size=15)
        self.normal_font = tkfont.Font(family='Comic', size=13)
        self.settings_font = tkfont.Font(family='Comic', size=15)

        # Container zorgt ervoor dat alle frames op elkaar komen
        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        # De pagina's worden aangemaakt en geillustreerd op de pagina
        self.frames = {}
        paginas = [StartPage, PageOne, PageTwo, PageThree]
        for F in (paginas):
            page_name = F.__name__
            frame = F(parent=container, controller=self)
            self.frames[page_name] = frame

            # Dit zet alle pagina's op elkaar
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame("StartPage")

    def show_frame(self, page_name):
        # Dit maakt de frame zichtbaar
        # tkraise zet de 'page_name' bovenaan de stackorder
        frame = self.frames[page_name]
        frame.tkraise()


class StartPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller

        # Tekst
        global label
        label = tk.Label(self, text="Main page " + interface,
                         font=controller.title_font, justify=LEFT, anchor='w')
        label1 = tk.Label(self, text="Sensoren: ", font=controller.normal_font, anchor='w')
        label2 = tk.Label(self, text="Opties: ", font=controller.normal_font, anchor='w')

        label.grid(row=0, column=0, sticky=W + E, ipady=2, ipadx=5, padx=(30, 0), pady=(30, 0))
        label1.grid(row=1, column=0, sticky=W + E, ipady=2, ipadx=5, padx=(30, 0), pady=(50, 0))
        label2.grid(row=3, column=0, sticky=W + E, ipady=2, ipadx=5, padx=(30, 0), pady=(50, 0))

        # Knoppen 
        button1 = tk.Button(self, text="Licht", width=15, height=2, bg='lightblue', fg='black',
                            command=lambda: controller.show_frame("PageOne"))
        button2 = tk.Button(self, text="Temperatuur", width=15, height=2, bg='lightblue', fg='black',
                            command=lambda: controller.show_frame("PageTwo"))
        button3 = tk.Button(self, text="Instellingen", width=15, height=2, bg='lightgrey', fg='black',
                            command=lambda: controller.show_frame("PageThree"))
        button4 = tk.Button(self, text="Scherm inrollen", width=15, height=2, bg='lightgrey', fg='black',
                            command=lambda: controller.show_frame("PageFour"))

        # White space zodat de foto "logo.png" op de juiste positie komt te staan
        w1 = Label(self, text="                                                                                                                        ")
        w1.grid(row=5, column=2)
        w1.grid(row=5, column=3)


        #Het plaatsen van de foto en de locatie ervan
        img = ImageTk.PhotoImage(Image.open("./image/logo.png"))
        panel = Label(self, image=img)
        panel.image = img
        panel.grid(row=5, column=4, sticky=E)
        #Het plaatsen van de buttons met de locaties waar die komt te staan
        button1.grid(row=2, column=0) 
        button2.grid(row=2, column=1, sticky=W)
        button3.grid(row=4, column=0)
        button4.grid(row=4, column=1, sticky=W)


class PageOne(tk.Frame):
    # Intialiseren van het fram voor pagine1, lichtsensor
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="Lichtsensor", font=controller.title_font)
        label.pack(side="top", fill="x", pady=10)
        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage"))
        button.pack()# Button plaatsen

        # De grafiek op het scherm drawen
        canvas = FigureCanvasTkAgg(fig, self)
        canvas.draw()
        canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage")) # Button terug naar de pagina
        button.pack()


class PageTwo(tk.Frame):
    # Intialiseren van het fram voor pagine1, Temperatuursensor
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller

        label = tk.Label(self, text="Temperatuursensor", font=controller.title_font) #label voor temperatuursensor
        label.pack(side="top", fill="x", pady=10)
        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage"))
        button.pack()# Button plaatsen

        # De grafiek op het scherm drawen
        canvas = FigureCanvasTkAgg(fig2, self)
        canvas.draw()
        canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

class PageThree(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller

        label = tk.Label(self, text="Instellingen", font=controller.title_font) #label voor instellingen
        label.pack(side="top", fill="x", pady=10)

        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage")) #button voor startpaginga
        button.pack()

        uitrollText = tk.Label(self, text="Uitrol instellen", font=controller.settings_font)
        uitrollText.place(relx=0.05, rely=0.25, anchor=W)

        afstandLabel = tk.Label(self, text="Afstand in MM:", font=(16)) #Label voor de tekst afstand in MM:
        afstandLabel.place(relx=0.05, rely=0.3, anchor=W)

        inputUitrol = Entry(self, width=10, borderwidth=1) 
        inputUitrol.place(relx=0.145, rely=0.301, anchor=W)

        #De knop gebruikt de waarde van uitrollText en gebruikt de input
        #in functie inputButtonText
        inputButton = tk.Button(self, text="Opslaan", command=lambda: inputButtonText(inputUitrol.get()))
        inputButton.place(relx=0.19, rely=0.3, anchor=W)


        def inputButtonText(input):
            #Zonder input wordt er niks ingevoerd.
            if len(input) == 0:
                opslaanText = tk.Label(self, text="Er is niks ingevoerd")
            #Zo ja dan zijn worden de waardes opgeschreven in Afstand.txt
            elif str(input).isdigit():
                opslaanText = tk.Label(self, text="Wijzigingen zijn opgeslagen")
                file = open("Afstand.txt", "a")
                file.truncate(0) # Zorgt ervoor dat Temperatuur.txt leeg wordt
                file.write('\\x'+str(input))
                file.close()
            #als het geen cijfers zijn wordt het niet opgeslagen
            else:
                opslaanText = tk.Label(self, text="Alleen cijfers toegestaan")
            opslaanText.place(relx=0.05, rely=0.322, anchor=W)
            app.after(1350, lambda: opslaanText.config(text=''))


if __name__ == "__main__":
    
    app = GUI()

    # menu bovenaan de venster
    menu = Menu(app)
    app.config(menu=menu)

    # De dropdown menu(commando's moeten toegevoegd worden)
    subMenu = Menu(menu)
    menu.add_cascade(label="File", menu=subMenu)
    subMenu.add_command(label="COM6", command=lambda: connectArduino(1))
    subMenu.add_command(label="COM8", command=lambda: connectArduino(2))
    subMenu.add_separator()
    subMenu.add_command(label="Exit", command=quit)

    # geeft het venster een vaste grootte
    app.geometry("1200x800")
    ani = animation.FuncAnimation(fig, lichtsensorGraph, interval=60000)
    ani2 = animation.FuncAnimation(fig2, temperatuurGraph, interval=60000)
    # runt klasse 'app'
    app.mainloop()
