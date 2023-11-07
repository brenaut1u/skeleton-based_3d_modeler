import sphere
import scene
import tkinter as tk

s1 = sphere.sph()
s1.setCenter(100,200)
#print(sphere.cent)
x = s1.x()
y = s1.y()
rayon = 50
print(x)
print(y)

s0 = scene.sce()
print(s0.nbSphere())
s0.addSphere(s1)
print(s0.nbSphere())

w = tk.Tk()
w.title("scene")
canvs = tk.Canvas(w,width = 400,height=400,bg="white")
canvs.grid(row=1,column=1,columnspan = 3,sticky=tk.N)
canvs.create_oval(x-rayon/2,y-rayon/2,x+rayon/2,y+rayon/2,fill='red')
w.mainloop()




exit()
