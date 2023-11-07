#!/bin/bash 
g++ -std=c++20 main.cpp -o"resultat.out"

cmake --build build
cd build
python3 lise.py


