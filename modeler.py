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

detected = -1
mode = "creation"

# CZ : an alternative would be to first create all rectangles,
# then update them using :
#    canvs.itemconfig(pixels[line+column*width], fill=color)
def createPixel(line,column,color):
    canvs.create_rectangle(line,column,line+2,column+2,fill = color, width=0)

def _from_rgb(rgb):
    return "#%02X%02X%02X" % rgb 

def printImage(event=None):
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
    modeler.add(event.x, event.y)
    # CZ : for testing directly update the displayed image
    #modeler.computeImage()
    #printImage()
    #detected = -1
        
def deleteSphere(event):
    modeler.delete(event.x,event.y)
    modeler.computeImage()
    printImage()

def changeRayon(event):
    global detected 
    if detected != -1 :
        modeler.changeRayon(0.9,detected)
        detected = -1
    #modeler.computeImage()
    #printImage()
    #detected = -1

def detectSphere(event):
    global detected
    detected = modeler.detect(event.x,event.y)

def actions(event):
    global mode
    global detected
    if mode == "creation" :
        addSphere(event)
    elif mode == "modification" :
        w.bind("<ButtonRelease-1>",changeRayon)
    modeler.computeImage()
    printImage()
    detected = -1
    

#creation of a button allowing us to switch between creation of a new sphere
    #and modification of the ray of an existing sphere
def on_button_click(event=None):
    global mode 
    if mode == "creation" :
        mode = "modification"
    elif mode == "modification" :
        mode = "creation"
    modeButton.config(text=mode)

modeButton = tk.Button(w, text=mode,command= on_button_click)
modeButton.pack(pady=20)
    
# CZ : Initialize the image when starting the application
modeler.computeImage()
printImage()

w.bind("<Button-2>",deleteSphere)
w.bind("c",on_button_click)
w.bind("<ButtonPress-1>",detectSphere)
w.bind("<ButtonRelease-1>",actions)

w.bind("f",printImage)

w.mainloop()

exit()
























