import modelerVrai
import tkinter as tk

vec = modelerVrai.vec3()

sphere = modelerVrai.sphere()

scene = modelerVrai.hittable_list()

scene.add(sphere)


w = tk.Tk()
w.title("scene")
canvs = tk.Canvas(w,width = 400,height=400,bg="white")
canvs.grid(row=1,column=1,columnspan = 3,sticky=tk.N)


def create_sphere(event):
    x = sphere.x()
    y = sphere.y()
    rayon = 100
    canvs.create_oval(x-rayon/2,y-rayon/2,x+rayon/2,y+rayon/2,fill='red')
    scene.add(sphere)

x = sphere.x()
y = sphere.y()
rayon = 100
canvs.create_oval(x-rayon/2,y-rayon/2,x+rayon/2,y+rayon/2,fill='red')


w.bind("<Button-1>",lambda event: create_sphere(event))

w.mainloop()

exit()


























