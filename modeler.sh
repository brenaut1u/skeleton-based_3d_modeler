#!/bin/bash 

cmake -S . -B build
cmake --build build
cp modeler.py build/
cd build
python3 modeler.py
