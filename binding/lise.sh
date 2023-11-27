#!/bin/bash 

cmake -S . -B build
cmake --build build
cp lise.py build/
cd build
python3 lise.py


