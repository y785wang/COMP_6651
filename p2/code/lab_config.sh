#!/bin/bash                                                                                                                               
cd build
module load opencv
cmake ../ -DCMAKE_CXX_COMPILER=/encs/bin/g++
cd ../
