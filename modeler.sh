#!/bin/bash 

cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
cp modeler.py build/
cd build
python3 modeler.py
