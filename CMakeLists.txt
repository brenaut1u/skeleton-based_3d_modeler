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
        camera.cpp
        camera.h
        color.cpp
        color.h
        cone.cpp
        cone.h
        draw.cpp
        draw.h
        hittable.cpp
        hittable.h
        hittable_list.cpp
        hittable_list.h
        interactions.cpp
        interactions.h
        interval.h
        light.cpp
        light.h
        linked_spheres_group.cpp
        linked_spheres_group.h
        material.cpp
        material.h
        ray.h
        rtweekend.h
        sphere.cpp
        sphere.h
        vec3.cpp
        vec3.h
        save_load.cpp
        save_load.h
        screen_segment.h
        span3D.cpp
        span3D.h
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