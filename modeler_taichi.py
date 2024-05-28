import build.main_modeler as main_modeler
import taichi as ti
import taichi.math as tm
import math as m
import numpy as np

import tkinter as tk
from tkinter import filedialog as fd


ti.init(arch=ti.gpu)
n = 400
m = 225


pixels = np.ndarray((n,m,3), dtype='f')

modeler1 = main_modeler.modeler()
modeler1.initializedWorld()

gui = ti.ui.Window("Modeler", res=(4*n, 4*m),vsync=True)
gui2 = gui.get_gui()
canvas = gui.get_canvas()

list_selected_id = [-1]
hovered_id = -1
origin = (-1,-1)
 
old_pos = (0, 0)

while gui.running:
    #gui.get_event()
    pos = gui.get_cursor_pos()

    hovered_id = modeler1.detect(int(pos[0]*n),int((1-pos[1])*m))

    mouse_clicked = False
   
    modeler1.select(hovered_id)
    
    if gui.get_event(ti.ui.PRESS) :
        if gui.event.key == ti.GUI.LMB :
            mouse_clicked = True
            if hovered_id == -1 :
                list_selected_id = [-1]
            elif gui.is_pressed('Control') and list_selected_id[0] != -1 :
                if hovered_id not in list_selected_id :
                    list_selected_id.append(hovered_id)
            else : 
                list_selected_id = [hovered_id]
    for id in list_selected_id:
        modeler1.select(id)        

    if mouse_clicked :
        if list_selected_id[0] != -1 :
            if gui.is_pressed('q'):
                modeler1.add(int(pos[0]*n),int((1-pos[1])*m))

            elif gui.is_pressed('r'):
                if origin == (-1,-1) or selected_id_past != list_selected_id[0]:
                    origin = pos
                    selected_id_past = list_selected_id[0]
                # elif list_selected_id[0] != -1 :
                #     ray = ((pos[0]-origin[0])**2+(pos[1]-origin[1])**2)**(1/2)*4
                #     modeler1.increaseRadius(list_selected_id[0],-(origin[0]-pos[0])/n*100)
        else :
            if gui.is_pressed('q'):
                modeler1.segment_cone(int(pos[0]*n),int((1-pos[1])*m))
            origin = pos
            selected_id_past = list_selected_id[0] = -1
        
    elif gui.is_pressed('d') :
        for id in list_selected_id:
            modeler1.delete(id)
        list_selected_id = [-1]

    elif gui.is_pressed(ti.GUI.LMB) and gui.is_pressed('r') and list_selected_id[0] != -1:
        for i in list_selected_id:
            modeler1.increaseRadius(i,-(origin[0]-pos[0])/n*100)
        # ray = ((pos[0]-origin[0])**2+(pos[1]-origin[1])**2)**(1/2)*4
        # modeler1.increaseRadius(list_selected_id[0],-(origin[0]-pos[0])/n*100)

    elif gui.is_pressed(ti.GUI.LMB) and not gui.is_pressed('q') and not gui.is_pressed('r') and list_selected_id[0] != -1 : 
        # for id in list_selected_id:
        #     modeler1.move_sphere(id, old_pos[0], old_pos[1], int(pos[0]*n), int((1-pos[1])*m))
        modeler1.move_sphere(list_selected_id[0], old_pos[0], old_pos[1], int(pos[0]*n), int((1-pos[1])*m))
    
    #rotate the camera around a fixed point
    if gui.is_pressed(ti.GUI.LEFT) :
        if gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(-0.05, 0.0)
        else:
            modeler1.rotate_camera(-0.1,0)
    elif gui.is_pressed(ti.GUI.RIGHT) :
        if gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(0.05, 0.0)
        else:
            modeler1.rotate_camera(0.1,0)
    elif gui.is_pressed(ti.GUI.UP) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler1.move_camera_forward(0.1)
        elif gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(0.0, -0.05)
        else : 
            modeler1.rotate_camera(0,0.1)
    elif gui.is_pressed(ti.GUI.DOWN) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler1.move_camera_forward(-0.1)
        elif gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(0.0, 0.05)
        else : 
            modeler1.rotate_camera(0,-0.1)
    #save and load functions
    elif gui.is_pressed(ti.GUI.SHIFT) :
        if gui.is_pressed('s') :
            #filename = input("filename : ")
            filename = tk.filedialog.asksaveasfilename()
            modeler1.save(filename)
        elif gui.is_pressed('l') :
            filename = tk.filedialog.askopenfilename()
            #filename = input("your filename : ")
            modeler1.load(filename)

    old_pos = (int(pos[0]*n),int((1-pos[1])*m))
    
    modeler1.computeImageSpan(pixels)
    canvas.set_image(pixels)

    gui.show()