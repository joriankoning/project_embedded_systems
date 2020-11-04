import tkinter as tk

class Page(tk.Frame):
    def __init__(self, *args, **kwargs):
        tk.Frame.__init__(self, *args, **kwargs)
    def show(self):
        self.lift()

class temperatuur(Page):
   def __init__(self, *args, **kwargs):
       Page.__init__(self, *args, **kwargs)
       label = tk.Label(self, text="Dit is de temperatuur pagina")
       label.pack(side="top", fill="both", expand=True)

class lichtpagina(Page):
   def __init__(self, *args, **kwargs):
       Page.__init__(self, *args, **kwargs)
       label = tk.Label(self, text="Dit is de lichtsensor pagina")
       label.pack(side="top", fill="both", expand=True)

class instellingen(Page):
   def __init__(self, *args, **kwargs):
       Page.__init__(self, *args, **kwargs)
       label = tk.Label(self, text="De pagina voor de instellingen")
       label.pack(side="top", fill="both", expand=True)

class MainView(tk.Frame):
    def __init__(self, *args, **kwargs):
        tk.Frame.__init__(self, *args, **kwargs)
        temp = temperatuur(self)
        licht = lichtpagina(self)
        instel = instellingen(self)

        buttonframe = tk.Frame(self)
        container = tk.Frame(self)
        buttonframe.pack(side="top", fill="x", expand=False)
        container.pack(side="top", fill="both", expand=True)

        temp.place(in_=container, x=0, y=0, relwidth=1, relheight=1)
        licht.place(in_=container, x=0, y=0, relwidth=1, relheight=1)
        instel.place(in_=container, x=0, y=0, relwidth=1, relheight=1)

        b1 = tk.Button(buttonframe, text="Page 1", command=temp.lift)
        b2 = tk.Button(buttonframe, text="Page 2", command=licht.lift)
        b3 = tk.Button(buttonframe, text="Page 3", command=instel.lift)

        b1.pack(side="left")
        b2.pack(side="left")
        b3.pack(side="left")

        temperatuur.show(self)

if __name__ == "__main__":
    root = tk.Tk()
    main = MainView(root)
    main.pack(side="top", fill="both", expand=True)
    root.wm_geometry("1000x1000")
    root.mainloop()