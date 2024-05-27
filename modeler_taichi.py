import build.main_modeler as main_modeler
import taichi as ti
import taichi.math as tm
import math as m
import numpy as np

import tkinter as tk
from tkinter import filedialog as fd



import wx

app = wx.App(None)

def get_path(wildcard):
    global app
    #app = wx.App(None)
    style = wx.FD_OPEN | wx.FD_FILE_MUST_EXIST
    dialog = wx.FileDialog(None, 'Open', wildcard=wildcard, style=style)
    if dialog.ShowModal() == wx.ID_OK:
        path = dialog.GetPath()
    else:
        path = None
    dialog.Destroy()
    return path

#print(get_path('*.txt'))

#filetype = [("All files", "*.*")]

#filename = fd.askopenfilename(
 #       title='Open a file',
  #      initialdir='/',
   #     filetypes=filetype)

# filename = fd.askopenfilename(initialdir= "/",title="Select File",filetypes=(("png files","*.png"),("jpeg files","*.jpg"),("all files","*.*")))
# print(filename)

#path = easygui.fileopenbox()


ti.init(arch=ti.gpu)
n = 400
m = 225

#image = ti.field(dtype=float, shape=(n,m))

pixels = np.ndarray((n,m,3), dtype='f')

modeler1 = main_modeler.modeler()
modeler1.initializedWorld()

gui = ti.ui.Window("Modeler", res=(4*n, 4*m),vsync=True)
gui2 = gui.get_gui()
canvas = gui.get_canvas()

selected_id = -1
hovered_id = -1
origin = (-1,-1)
 
old_pos = (0, 0)
#save = gui2.button('save')
#filename = fd.askopenfilename(title='Open a file',initialdir='/', filetypes=filetype)


while gui.running:
    #gui.get_event()
    pos = gui.get_cursor_pos()
    hovered_id = modeler1.detect(int(pos[0]*n),int((1-pos[1])*m))
    
    mouse_clicked = False

   
    
    
    if gui.get_event(ti.ui.PRESS) :
        if gui.event.key == ti.GUI.LMB :
            mouse_clicked = True
            selected_id = hovered_id
            modeler1.select(selected_id)
        elif gui.event.key == ti.GUI.WHEEL :
            print("mouse wheel")

    if mouse_clicked :
        if selected_id != -1 :
            if gui.is_pressed('q'):
                modeler1.add(int(pos[0]*n),int((1-pos[1])*m))

            elif gui.is_pressed('d') :
                modeler1.delete(selected_id)
                selected_id = -1

            elif gui.is_pressed('r'):
                if origin == (-1,-1) or selected_id_past != selected_id:
                    origin = pos
                    selected_id_past = selected_id
                elif selected_id != -1 :
                    ray = ((pos[0]-origin[0])**2+(pos[1]-origin[1])**2)**(1/2)*4
                    modeler1.increaseRadius(selected_id,-(origin[0]-pos[0])/n*100)
            
        elif gui.is_pressed('t'):
                origin = (-1,-1)
                selected_id_past = -1
        else :
            if gui.is_pressed('q'):
                modeler1.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
        
            origin = pos
            selected_id_past = selected_id = -1
        
                
    elif gui.is_pressed(ti.GUI.LMB) and gui.is_pressed('r') and selected_id != -1:
        ray = ((pos[0]-origin[0])**2+(pos[1]-origin[1])**2)**(1/2)*4
        modeler1.increaseRadius(selected_id,-(origin[0]-pos[0])/n*100)

    elif gui.is_pressed(ti.GUI.LMB) and not gui.is_pressed('q') and not gui.is_pressed('r') and selected_id != -1 : 
        modeler1.move_sphere(selected_id, old_pos[0], old_pos[1], int(pos[0]*n), int((1-pos[1])*m))

    elif gui.is_pressed(ti.GUI.WHEEL):
        print("you click on the mouse")

    
    #rotate the camera around a fixed point
    if gui.is_pressed(ti.GUI.LEFT) :
        modeler1.rotate_camera(-0.1,0)
    elif gui.is_pressed(ti.GUI.RIGHT) :
        modeler1.rotate_camera(0.1,0)
    elif gui.is_pressed(ti.GUI.UP) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler1.move_camera_forward(0.1)
        else : 
            modeler1.rotate_camera(0,0.1)
    elif gui.is_pressed(ti.GUI.DOWN) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler1.move_camera_forward(-0.1)
        else : 
            modeler1.rotate_camera(0,-0.1)
    elif gui.is_pressed(ti.GUI.SHIFT) :
        if gui.is_pressed('s') :
            filename = input("filename : ")
            modeler1.save(filename)
        elif gui.is_pressed('l') :
            filename = input("your filename : ")
            modeler1.load(filename)


    """with gui2.sub_window("Sub Window", x=10, y=10, width=300, height=100):
        is_clicked = gui2.button("save2")
        if is_clicked : 
            print("you saved")
            is_clicked = False
            path = get_path('*.txt')
            #filename = fd.askopenfilename(initialdir= "/",title="Select File",filetypes=(("png files","*.png"),("jpeg files","*.jpg"),("all files","*.*")))
            #filename = fd.askopenfilename(title='Open a file',initialdir='/', filetypes=filetype)

    if gui2.button("menu"):
        print("menu")

    
    for e in gui.get_events():
        #create a new sphere on the surface of a new one
        if e.key == ti.ui.LMB : 
            selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
            if selected_id != -1 :
                modeler.add(int(pos[0]*n),int((1-pos[1])*m))
            else :
                modeler.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
        #delete a sphere
        elif e.key == ti.GUI.RMB :
            selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
            modeler.delete(selected_id)

    if gui.get_event((ti.GUI.RELEASE,ti.GUI.LMB)) :
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        if selected_id != -1 :
            modeler.add(int(pos[0]*n),int((1-pos[1])*m))
        else :
            modeler.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
    elif gui.is_pressed(ti.GUI.RMB) :
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.delete(selected_id)
 
    if gui.is_pressed(ti.GUI.MOVE and ti.GUI.LMB):
        print("you moved the mouse")
        pos = gui.get_cursor_pos()
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.set_sphere_position(selected_id,int(pos[0]*n),int((1-pos[1])*m))

    if gui.is_pressed(ti.GUI.SHIFT and ti.GUI.MOVE and ti.GUI.LMB):
        pos = gui.get_cursor_pos()
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.changeRadius(selected_id,1)"""
    old_pos = (int(pos[0]*n),int((1-pos[1])*m))
    
    modeler1.computeImageSpan(pixels)
    canvas.set_image(pixels)

    gui.show()