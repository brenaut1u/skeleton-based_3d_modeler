import build.modeler as modeler
import taichi as ti
import taichi.math as tm
import math as m
import numpy as np


import time

from tkinter import filedialog as fd
filetype = [("All files", "*.*")]

#filename = fd.askopenfilename(
#       title='Open a file',
#      initialdir='/',
#     filetypes=filetype)


ti.init(arch=ti.gpu)
n = 800
m = 450

image = ti.field(dtype=float, shape=(n,m))

pixels = np.ndarray((n,m,3), dtype='f')

modeler = modeler.modeler()
modeler.initializedWorld()

gui = ti.ui.Window("Modeler", res=(n, m),vsync=True)
gui2 = gui.get_gui()
canvas = gui.get_canvas()

selected_id = -1
hovered_id = -1




while gui.running:
    print("new loop")
    #gui.get_event()
    pos = gui.get_cursor_pos()
    hovered_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))

    mouse_clicked = False

    # with gui2.sub_window("Sub Window", x=10, y=10, width=300, height=100):
    #     gui2.text("text")
    #     is_clicked = gui2.button("name")
    #     if is_clicked :
    #         filename = fd.askopenfilename(title='Open a file',initialdir='/', filetypes=filetype)


    if gui.get_event(ti.ui.PRESS) :
        if gui.event.key == ti.GUI.LMB :
            mouse_clicked = True
            selected_id = hovered_id
            print("selection")
        print("hello") 

    if mouse_clicked :
        if selected_id != -1 :
            if gui.is_pressed('q'):
                modeler.add(int(pos[0]*n),int((1-pos[1])*m))
                print("add sphere")
            elif gui.is_pressed('d') :
                modeler.delete(selected_id)
                print("delete sphere")
        else :
            modeler.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
    elif gui.is_pressed(ti.GUI.LMB) and not gui.is_pressed('q') : 
        modeler.set_sphere_position(selected_id,int(pos[0]*n),int((1-pos[1])*m))
        print("move")

    
    #rotate the camera around a fixed point
    if gui.is_pressed(ti.GUI.LEFT) :
        modeler.rotate_camera(-0.1,0)
    elif gui.is_pressed(ti.GUI.RIGHT) :
        modeler.rotate_camera(0.1,0)
    elif gui.is_pressed(ti.GUI.UP) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler.move_camera_forward(0.1)
        else : 
            modeler.rotate_camera(0,0.1)
    elif gui.is_pressed(ti.GUI.DOWN) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler.move_camera_forward(-0.1)
        else : 
            modeler.rotate_camera(0,-0.1)
    """
    if gui2.button("menu"):
        print("menu")"""

    """
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
            modeler.delete(selected_id)"""

    """   
    if gui.get_event((ti.GUI.RELEASE,ti.GUI.LMB)) :
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        if selected_id != -1 :
            modeler.add(int(pos[0]*n),int((1-pos[1])*m))
        else :
            modeler.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
    elif gui.is_pressed(ti.GUI.RMB) :
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.delete(selected_id)"""



    """
    if gui.is_pressed(ti.GUI.MOVE and ti.GUI.LMB):
        print("you moved the mouse")
        pos = gui.get_cursor_pos()
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.set_sphere_position(selected_id,int(pos[0]*n),int((1-pos[1])*m))

    if gui.is_pressed(ti.GUI.SHIFT and ti.GUI.MOVE and ti.GUI.LMB):
        pos = gui.get_cursor_pos()
        selected_id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.changeRadius(selected_id,1)"""

    modeler.computeImageSpan(pixels)
    canvas.set_image(pixels)

    gui.show()