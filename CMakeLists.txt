project(my_project) # Replace 'my_project' with the name of your project
cmake_minimum_required(VERSION 3.15...3.27)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python 3.8 COMPONENTS Interpreter Development REQUIRED)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(extern/pybind11)

# CZ add all files used by the bindings
pybind11_add_module(modelerVrai modeler.cpp
    camera.h
    color.h
    command.h
    cone.h
    hittable.h
    hittable_list.h
    interactions.h
    interval.h
    light.h
    linked_spheres_group.h
    material.h
    ray.h
    rtweekend.h
    sphere.h
    vec3.h
)

# copy python file in build folder
configure_file(modeler.py ${CMAKE_BINARY_DIR}/modeler.py COPYONLY)

# CZ add the C++ executable
add_executable(viewer main.cpp
    camera.h
    color.h
    command.h
    cone.h
    hittable.h
    hittable_list.h
    interactions.h
    interval.h
    light.h
    linked_spheres_group.h
    material.h
    ray.h
    rtweekend.h
    sphere.h
    vec3.h
)