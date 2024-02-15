import modelerVrai
import taichi as ti
import taichi.math as tm
import math as m
import numpy as np
import tkinter as tk

import time



ti.init(arch=ti.gpu)

n = 400
m = 225

image = ti.field(dtype=float, shape=(n,m))

pixels = np.ndarray((n,m,3), dtype='f')

modeler = modelerVrai.modeler()

modeler.initializedWorld()

"modeler.computeImageSpan(pixels)"


gui = ti.GUI("Modeler", res=(n, m))

for i in range(n):
    pixels[i,0,0] = 1

save = gui.button('save')


while gui.running:
    #gui.get_event(ti.GUI.RELEASE)

    for e in gui.get_events(gui.RELEASE):
        #create a new sphere on the surface of a new one
        if e.key == ti.GUI.LMB : 
            pos = gui.get_cursor_pos()
            id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
            if id != -1 :
                modeler.add(int(pos[0]*n),int((1-pos[1])*m))
            else :
                modeler.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
        #delete a sphere
        elif e.key == ti.GUI.RMB :
            pos = gui.get_cursor_pos()
            id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
            modeler.delete(id)

        
    for e in gui.get_events(gui.MOTION):
        print(e.key)

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

    if gui.get_event(ti.GUI.MOVE):
        pos = gui.get_cursor_pos()
        modeler.changeRadius(modeler.detect(int(pos[0]*n),int((1-pos[1])*m)),0.9)
    
    if gui.is_pressed(ti.GUI.MOVE and ti.GUI.LMB):
        print("you moved the mouse")
        pos = gui.get_cursor_pos()
        id = modeler.detect(int(pos[0]*n),int((1-pos[1])*m))
        modeler.set_sphere_position(id,int(pos[0]*n),int((1-pos[1])*m))
    """if gui.is_pressed(ti.GUI.LMB) :
        pos = gui.get_cursor_pos()
        modeler.add(int(pos[0]*n),int(pos[1]*m))"""
    
    
    modeler.computeImageSpan(pixels)
    gui.set_image(pixels)

    gui.show()
