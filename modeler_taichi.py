import build.main_modeler as main_modeler
import taichi as ti
import taichi.math as tm
import math as m
import numpy as np

from sys import platform

use_tk = True
if platform == "darwin":
    use_tk = False

if use_tk:
    from tkinter import filedialog
    from tkinter import colorchooser

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

show_skeleton = True
 
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
        elif gui.event.key == ti.GUI.WHEEL :
            print("mouse wheel")

        elif gui.is_pressed('w'):
            show_skeleton = not show_skeleton
            # print("\n")
            # segments = modeler1.getSkeletonScreenCoordinates()
            # for i in range(segments.size()):
            #     print("(%d %d), (%d %d)\n"%(segments.at(i).first.x, segments.at(i).first.y, \
            #                                 segments.at(i).second.x, segments.at(i).second.y))

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

    elif gui.is_pressed(ti.GUI.LMB) and gui.is_pressed('c') and selected_id != -1:
        if use_tk:
            color = colorchooser.askcolor()
            if color[0] != None:
                modeler1.changeColor(selected_id, color[0][0], color[0][1], color[0][2])
        else:
            print("Change color - Enter values between 0 and 255")
            try:
                r = int(input("Red: "))
                g = int(input("Green: "))
                b = int(input("Blue: "))

                if (r in range(0, 256) and g in range(0, 256) and b in range(0, 256)):
                    modeler1.changeColor(selected_id, r, g, b)
                else:
                    print("Value incorrect")
            except:
                print("Value incorrect")


    elif gui.is_pressed(ti.GUI.LMB) and not gui.is_pressed('q') and not gui.is_pressed('r') and selected_id != -1 : 
        modeler1.move_sphere(selected_id, old_pos[0], old_pos[1], int(pos[0]*n), int((1-pos[1])*m))

    elif gui.is_pressed(ti.GUI.WHEEL):
        print("you click on the mouse")

    
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
    elif gui.is_pressed(ti.GUI.SHIFT) :
        if gui.is_pressed('s') :
            if use_tk:
                filename = filedialog.asksaveasfilename()
            else:
                filename = input("Save - Enter filename: ")
            if filename != '':
                modeler1.save(filename)
        elif gui.is_pressed('l') :
            if use_tk:
                filename = filedialog.askopenfilename()
            else:
                filename = input("Open - Enter filename: ")
            if filename != '':
                modeler1.load(filename)

    old_pos = (int(pos[0]*n),int((1-pos[1])*m))
    
    modeler1.computeImageSpan(pixels)
    canvas.set_image(pixels)

    f = ti.Vector.field(n=2, dtype=int, shape=2)
    f[0] = (0, 0)
    f[1] = (1, 1)
    canvas.lines(f, 1)
    # if show_skeleton:
    #     segments = modeler1.getSkeletonScreenCoordinates()
    #     for i in range(segments.size()):
    #         print("(%d %d), (%d %d)\n"%(segments.at(i).first.x, segments.at(i).first.y, \
    #                                     segments.at(i).second.x, segments.at(i).second.y))

    gui.show()