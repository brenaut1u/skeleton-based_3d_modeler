import modelerVrai
import tkinter as tk


modeler = modelerVrai.modeler()

modeler.initializedWorld()

w = tk.Tk()
w.title("scene")

width = 400
height = 225
canvs = tk.Canvas(w,width = width,height = height,bg="white")
#canvs.grid(row=1,column=1,columnspan = 3,sticky=tk.N)
canvs.pack()

detect = -1

# CZ : an alternative would be to first create all rectangles,
# then update them using :
#    canvs.itemconfig(pixels[line+column*width], fill=color)
def createPixel(line,column,color):
    canvs.create_rectangle(line,column,line+2,column+2,fill = color, width=0)

def _from_rgb(rgb):
    return "#%02X%02X%02X" % rgb 

def printImage():
    canvs.delete('all')
    for i in range(width +1 ):
        for j in range(height+1):
            r = int(modeler.getRed(i+j*400)*255)%255
            g = int(modeler.getGreen(i+j*400)*255)%255
            b = int(modeler.getBlue(i+j*400)*255)%255
            color = _from_rgb((r,g,b))
            createPixel(i,j,color)
    w.update()

def addSphere(event):
    global detect
    if detect != -1 :
        modeler.add(event.x, event.y)
        print("add")
        # CZ : for testing directly update the displayed image
        modeler.computeImage()
        detect = -1
        printImage()
        
def deleteSphere(event):
    modeler.delete(event.x,event.y)
    print("delete")
    modeler.computeImage()
    printImage()

def detectSphere(event):
    global detect 
    detect = modeler.detect(event.x,event.y)
    print("coucou")


    
# CZ : Initialize the image when starting the application
modeler.computeImage()
printImage()

w.bind("<Button-2>",deleteSphere)
w.bind("<ButtonPress-1>",detectSphere)
w.bind("<ButtonRelease-1>",addSphere)

w.bind("f", lambda : printImage())

w.mainloop()

exit()
























