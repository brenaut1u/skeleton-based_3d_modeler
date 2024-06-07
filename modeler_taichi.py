import build.main_modeler as main_modeler
import taichi as ti
import taichi.math as tm
import math as m
import numpy as np

from sys import platform


# Check if the OS is compatible with the use of both tkinter and taichi
use_tk = True
if platform == "darwin":
    use_tk = False
if use_tk:
    from tkinter import filedialog
    from tkinter import colorchooser
    from tkinter import simpledialog

# Initialize the taichi environment
ti.init(arch=ti.gpu)
n = 400
m = 225

# Create the canvas and the modeler
pixels = np.ndarray((n,m,3), dtype='f')
beautiful_render = np.ndarray((2*n, 2*m, 3), dtype='f')

modeler1 = main_modeler.modeler()
modeler1.initializedWorld()

gui = ti.ui.Window("Modeler", res=(4*n, 4*m),vsync=True)
gui2 = gui.get_gui()
canvas = gui.get_canvas()

modeler1.computeBeautifulRender(beautiful_render)

array_selected_id = np.array([-1])
hovered_id = -1
origin = (-1,-1)
old_pos = (0, 0)
show_skeleton = False




while gui.running:
    # Get the position of the cursor
    pos = gui.get_cursor_pos()
    mouse_clicked = False

    # Check if the cursor is hovering over a sphere
    hovered_id = modeler1.detect(int(pos[0]*n),int((1-pos[1])*m))
    modeler1.hovered(hovered_id)
    

    if gui.get_event(ti.ui.PRESS) :
        # Check if the left mouse button is clicked
        if gui.event.key == ti.GUI.LMB :
            # If it is, set the mouse_clicked variable to True
            mouse_clicked = True
            # and store the position of the cursor
            origin = (pos[0],pos[1])
            
            # If we hover nothing, that means we unselect everything
            if hovered_id == -1 :
                #we unselect everything
                modeler1.unselect(array_selected_id)
                #we reset the array of selected id
                array_selected_id = np.array([-1])
            # If we press the control key, it means we want to select multiple spheres
            elif gui.is_pressed('Control') and array_selected_id[0] != -1 :
                # If the hovered sphere is not already selected, we add it to the list
                if hovered_id not in array_selected_id :
                    array_selected_id =  np.append(array_selected_id,hovered_id)
                # If it is already selected, we remove it from the list
                else : 
                    array_selected_id = array_selected_id[array_selected_id != hovered_id]
                    modeler1.unselect(np.array([hovered_id]))
                    if len(array_selected_id) == 0 :              
                        array_selected_id = np.array([-1])
            # If we are not in one of the first two cases, we unselect the selected spheres and select the hovered one
            else : 
                modeler1.unselect(array_selected_id)
                array_selected_id = np.array([hovered_id])

            # If we press 'q' (or 'a'), we add a sphere if possible at the position of the cursor
            if gui.is_pressed('q') :
                modeler1.add(int(pos[0]*n),int((1-pos[1])*m))
                modeler1.computeBeautifulRender(beautiful_render)

        # If the 'c' button is pressed, we open a color picker to change the color of the selected spheres
        elif gui.is_pressed('c') and array_selected_id[0] != -1:
            if use_tk:
                color = colorchooser.askcolor()
                if color[0] != None:
                    modeler1.changeColor(array_selected_id, color[0][0], color[0][1], color[0][2])
                    modeler1.computeBeautifulRender(beautiful_render)
            else:
                print("Change color - Enter values between 0 and 255")
                try:
                    r = int(input("Red: "))
                    g = int(input("Green: "))
                    b = int(input("Blue: "))
                    if (r in range(0, 256) and g in range(0, 256) and b in range(0, 256)):
                        modeler1.changeColor(array_selected_id, r, g, b)
                        modeler1.computeBeautifulRender(beautiful_render)
                    else:
                        print("Value incorrect")
                except:
                    print("Value incorrect")
                
        # Make the skeleton visible or not
        elif gui.is_pressed('w'):
            show_skeleton = not show_skeleton

        # If the 'd' button is pressed, we delete the selected spheres
        elif gui.is_pressed('d') and array_selected_id[0] != -1:
            array_selected_id = np.sort(array_selected_id)
            modeler1.unselect(array_selected_id)
            modeler1.delete(array_selected_id)
            array_selected_id = np.array([-1])
            modeler1.computeBeautifulRender(beautiful_render)

        # If the 'f' button is pressed, we add a link between the selected spheres
        elif gui.is_pressed('f') :
            modeler1.addLink(array_selected_id[0],array_selected_id[1])
            modeler1.computeBeautifulRender(beautiful_render)     
        
    # We change the appearance of the selected spheres
    modeler1.select(array_selected_id)     
        
    # If these buttons are pressed, we rotate the spheres around the calera axis or around a vertex
    if gui.is_pressed('a'):
        if (array_selected_id.size > 1):
            modeler1.rotateSphereCamera(array_selected_id,0.2)
            modeler1.computeBeautifulRender(beautiful_render)
    elif gui.is_pressed('s'):
        if (array_selected_id.size > 1):
            modeler1.rotateSphereCamera(array_selected_id,-0.2)
            modeler1.computeBeautifulRender(beautiful_render)
    elif gui.is_pressed('p'):
        if (array_selected_id.size > 2):
            modeler1.rotateSphereAxis(array_selected_id,0.2)
            modeler1.computeBeautifulRender(beautiful_render)

    # If the 'r' button is pressed, we change the radius of the selected spheres
    elif gui.is_pressed(ti.GUI.LMB) and gui.is_pressed('r') and array_selected_id[0] != -1:
        # It can be done by entering a value
        if gui.is_pressed('Shift') :
            if use_tk :
                title = "Radius"    
                prompt = "Enter the radius of the circle: "
                radius = simpledialog.askfloat(title, prompt)
                for id in array_selected_id:
                    modeler1.changeRadius(id, radius)
                modeler1.computeBeautifulRender(beautiful_render)
            else : 
                print("Change radius - Enter values between 0 and 1")
                try:
                    radius = float(input("Radius: "))
                    if 0 < radius and radius < 10:
                        for id in array_selected_id:
                            modeler1.changeRadius(id, radius)
                        modeler1.computeBeautifulRender(beautiful_render)
                    else:
                        print("Value incorrect")
                except:
                    print("Value incorrect")
        # Or by moving the mouse
        else : 
            for id in array_selected_id:
                modeler1.increaseRadius(id,-(origin[0]-pos[0])/n*100)
            modeler1.computeBeautifulRender(beautiful_render)

    elif gui.is_pressed(ti.GUI.LMB) and not gui.is_pressed('q') and not gui.is_pressed('r') and array_selected_id[0] != -1: 
        modeler1.move_sphere(array_selected_id, old_pos[0], old_pos[1], int(pos[0]*n), int((1-pos[1])*m))
        modeler1.computeBeautifulRender(beautiful_render)

    # This part is used to move the camera
    if gui.is_pressed(ti.GUI.LEFT) :
        if gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(-0.05, 0.0)
        else:
            modeler1.rotate_camera(-0.1,0)
        modeler1.computeBeautifulRender(beautiful_render)
    elif gui.is_pressed(ti.GUI.RIGHT) :
        if gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(0.05, 0.0)
        else:
            modeler1.rotate_camera(0.1,0)
        modeler1.computeBeautifulRender(beautiful_render)
    elif gui.is_pressed(ti.GUI.UP) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler1.move_camera_forward(0.1)
        elif gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(0.0, -0.05)
        else :
            modeler1.rotate_camera(0,0.1)
        modeler1.computeBeautifulRender(beautiful_render)
    elif gui.is_pressed(ti.GUI.DOWN) :
        if gui.is_pressed(ti.GUI.SHIFT) :
            modeler1.move_camera_forward(-0.1)
        elif gui.is_pressed(ti.GUI.CTRL) :
            modeler1.move_camera_sideways(0.0, 0.05)
        else :
            modeler1.rotate_camera(0,-0.1)
        modeler1.computeBeautifulRender(beautiful_render)
    
    # If 's' or 'l' is pressed, it saves and loads a scene
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
                modeler1.computeBeautifulRender(beautiful_render)

    # Before ending this lopp, we store the position of the cursor
    old_pos = (int(pos[0]*n),int((1-pos[1])*m))

    if modeler1.isBeautifulRenderReady():
        canvas.set_image(beautiful_render)
    else:
        modeler1.computePhongRender(pixels, show_skeleton)
        canvas.set_image(pixels)

    # modeler1.computePhongRender(pixels, show_skeleton)
    # canvas.set_image(pixels)

    gui.show()