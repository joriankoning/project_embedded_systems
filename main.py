import tkinter as tk

class Page(tk.Frame):
    def __init__(self, *args, **kwargs):
        tk.Frame.__init__(self, *args, **kwargs)
    def show(self):
        self.lift()
        
        
class mainPage(Page):
   def __init__(self, *args, **kwargs):
       Page.__init__(self, *args, **kwargs)
       label = tk.Label(self, text="Dit is de hoofdpagina")
       label.pack(side="top", fill="both", expand=True)



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
        main = mainPage(self)

        buttonframe = tk.Frame(self)
        container = tk.Frame(self)
        buttonframe.pack(side="top", fill="x", expand=False)
        container.pack(side="top", fill="both", expand=True)

        temp.place(in_=container, x=0, y=0, relwidth=1, relheight=1)
        licht.place(in_=container, x=0, y=0, relwidth=1, relheight=1)
        instel.place(in_=container, x=0, y=0, relwidth=1, relheight=1)
        
        
        home1 = tk.Button(buttonframe, text="Home", command=temp.lift)
        home2 = tk.Button(buttonframe, text="Home", command=licht.lift)
        home3 = tk.Button(buttonframe, text="Home", command=instel.lift)

        home1.pack(side="left")
        home2.pack(side="left")
        home3.pack(side="left")

        mainPage.show(self)

if __name__ == "__main__":
    root = tk.Tk()
    main = MainView(root)
    main.pack(side="top", fill="both", expand=True)
    root.wm_geometry("1000x1000")
    root.mainloop()