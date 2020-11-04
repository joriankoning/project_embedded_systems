#Ik zoek naar een betere oplossing hiervoor in de toekomst
import tkinter as tk
from tkinter import font as tkfont
from tkinter import *
from PIL import Image, ImageTk

text= 70
interface = "COM1"

class GUI(tk.Tk):

    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        self.title_font = tkfont.Font(family='Comic', size=18)
        self.onder_font = tkfont.Font(family='Comic', size=15)
        self.normal_font = tkfont.Font(family='Comic', size=13)


        # Container zorgt ervoor dat alle frames op elkaar komen
        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        # De pagina's worden aangemaakt en geillustreerd op de pagina
        self.frames = {}
        paginas = [StartPage, PageOne, PageTwo, PageThree, PageFour]
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
        label = tk.Label(self, text="Main page " + interface + "\nStatus: " + str(text) + " %", font=controller.title_font,
                         justify=LEFT, anchor='w')
        label1 = tk.Label(self, text="Sensoren: ", font=controller.normal_font, anchor='w')
        label2 = tk.Label(self, text="Opties: ", font=controller.normal_font, anchor='w')

        label.grid(row=0, column=0, sticky=W+E, ipady=2, ipadx=5, padx=(30,0), pady=(30,0))
        label1.grid(row=1, column=0, sticky=W+E, ipady=2,  ipadx=5, padx=(30,0), pady=(50,0))
        label2.grid(row=3, column=0, sticky=W+E, ipady=2, ipadx=5, padx=(30,0), pady=(50,0))

        #Bedrijfslogo, doet nog niks krijg em er niet op
        img = ImageTk.PhotoImage(Image.open("image/unknown.jpg"))
        img_label = Label(self, image=img)
        img_label.grid(row=5, column=0)

        #Knoppen
        button1 = tk.Button(self, text="Licht", width=15, height=2, bg='lightblue', fg='black',
                            command=lambda: controller.show_frame("PageOne"))
        button2 = tk.Button(self, text="Temperatuur", width=15, height=2, bg='lightblue', fg='black',
                            command=lambda: controller.show_frame("PageTwo"))
        button3 = tk.Button(self, text="Instellingen", width=15, height=2, bg='lightgrey', fg='black',
                            command=lambda: controller.show_frame("PageThree"))
        button4 = tk.Button(self, text="Scherm inrollen", width=15, height=2, bg='lightgrey', fg='black',
                            command=lambda: controller.show_frame("PageFour"))

        button1.grid(row=2, column=0)
        button2.grid(row=2, column=1, sticky=W)
        button3.grid(row=4, column=0)
        button4.grid(row=4, column=1, sticky=W)

class PageOne(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="This is page 1", font=controller.title_font)
        label.pack(side="top", fill="x", pady=10)
        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage"))
        button.pack()


class PageTwo(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="This is page 2", font=controller.title_font)
        label.pack(side="top", fill="x", pady=10)
        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage"))
        button.pack()

class PageThree(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="This is page 3", font=controller.title_font)
        label.pack(side="top", fill="x", pady=10)
        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage"))
        button.pack()

class PageFour(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        label = tk.Label(self, text="This is page 4", font=controller.title_font)
        label.pack(side="top", fill="x", pady=10)
        button = tk.Button(self, text="Go to the start page",
                           command=lambda: controller.show_frame("StartPage"))
        button.pack()

if __name__ == "__main__":
    app = GUI()

    menu = Menu(app)
    app.config(menu=menu)

    subMenu = Menu(menu)
    menu.add_cascade(label="File", menu=subMenu)
    subMenu.add_command(label="COM1")
    subMenu.add_command(label="COM2")
    subMenu.add_separator()
    subMenu.add_command(label="Exit")

    app.geometry('800x500')
    app.mainloop()
