import modeler
import tkinter as tk


s1 = modeler.sphere()
s1.setCenter(100,200)
s1.ray = 100


s2 = modeler.sphere()
s2.setCenter(200,300)
s2.ray = 100

s0 = modeler.sce()
print(s0.nbSphere())
s0.addSphere(s1)
print(s0.nbSphere())

w = tk.Tk()
w.title("scene")
canvs = tk.Canvas(w,width = 400,height=400,bg="white")
canvs.grid(row=1,column=1,columnspan = 3,sticky=tk.N)

def create_sphere(event):
    s1 = modeler.sphere()
    s1.setCenter(int(event.x),int(event.y))
    s1.ray = 100
    s0.addSphere(s1)
    print("Sphère créée à la position :", event.x, event.y)

def printSphere():
    for i in range(s0.nbSphere()) : 
        val = s0.getSphere(i)
        x = val.x()
        y = val.y()
        rayon = val.ray
        canvs.create_oval(x-rayon/2,y-rayon/2,x+rayon/2,y+rayon/2,fill='red')
    w.after(100,printSphere)

printSphere()

w.bind("<Button-1>",lambda event: create_sphere(event))

w.mainloop()

exit()
