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
    #canvs.delete("all")
    for i in range(width +1 ):
        for j in range(height+1):
            r = int(modeler.getRed(i+j*400)*255)
            g = int(modeler.getGreen(i+j*400)*255)
            b = int(modeler.getBlue(i+j*400)*255)
            color = _from_rgb((r,g,b))
            changePixel(i,j,color)
    w.update()

"""
def addSphere(event):
    modeler.add()
"""

#w.bind("<Button-1>",addSphere)
w.bind("f",printImage)

w.mainloop()

exit()
























