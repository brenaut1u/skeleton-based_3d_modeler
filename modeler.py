import modelerVrai
import tkinter as tk


modeler = modelerVrai.modeler()

modeler.initializedWorld()
modeler.initializedCam()

w = tk.Tk()
w.title("scene")

width = 400
height = 225
canvs = tk.Canvas(w,width = width,height = height,bg="white")
#canvs.grid(row=1,column=1,columnspan = 3,sticky=tk.N)
canvs.pack()

def changePixel(line,column,color):
    canvs.create_rectangle(line,column,line+2,column+2,fill = color, width=0)

def _from_rgb(rgb):
    return "#%02X%02X%02X" % rgb 

def printImage(event):
    canvs.delete("all")
    for i in range(width +1 ):
        for j in range(height+1):
            #print(i,j)
            r = int(modeler.getRed(i+j*400)*255)
            g = int(modeler.getGreen(i+j*400)*255)
            b = int(modeler.getBlue(i+j*400)*255)
            #print(r,g,b)
            color = _from_rgb((r,g,b))
            #print(color)
            changePixel(i,j,color)
    #canvs.create_rectangle(10,10,10+2,10+2,fill = "#000055", width=0)
    #canvs.update()
    w.update()


w.bind("<Button-1>",printImage)

w.mainloop()

exit()
























