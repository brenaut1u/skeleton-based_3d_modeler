project(my_project) # Replace 'my_project' with the name of your project
cmake_minimum_required(VERSION 3.15...3.27)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(extern/pybind11)

add_library(skeleton_modeler STATIC
        camera/beautiful_camera.cpp
        camera/beautiful_camera.h
        camera/camera.cpp
        camera/camera.h
        utilities/color.cpp
        utilities/color.h
        scene_objects/cone.cpp
        scene_objects/cone.h
        image/draw.cpp
        image/draw.h
        scene_objects/hittable.cpp
        scene_objects/hittable.h
        scene_objects/hittable_list.cpp
        scene_objects/hittable_list.h
        structures/interactions.cpp
        structures/interactions.h
        utilities/interval.h
        scene_objects/light.h
        structures/linked_spheres_group.cpp
        structures/linked_spheres_group.h
        utilities/material.cpp
        utilities/material.h
        camera/phong_camera.cpp
        camera/phong_camera.h
        utilities/ray.h
        utilities/rtweekend.h
        scene_objects/sphere.cpp
        scene_objects/sphere.h
        utilities/vec3.cpp
        utilities/vec3.h
        structures/save_load.cpp
        structures/save_load.h
        image/screen_segment.h
        image/span3D.cpp
        image/span3D.h
)

# CZ add all files used by the bindings
pybind11_add_module(main_modeler
        modeler.cpp
)

target_link_libraries(main_modeler PUBLIC skeleton_modeler)

# CZ add the C++ executable
add_executable(viewer
        main.cpp
)
target_link_libraries(viewer PUBLIC skeleton_modeler)