import modelerVrai
import tkinter as tk


modeler = modelerVrai.modeler



w = tk.Tk()
w.title("scene")
canvs = tk.Canvas(w,width = 400,height=400,bg="white")
canvs.grid(row=1,column=1,columnspan = 3,sticky=tk.N)



w.bind("<Button-1>",print("coucou"))

w.mainloop()

exit()
























