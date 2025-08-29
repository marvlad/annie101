#!/bin/bash

if [ "$1" = "clean" ]; then
    rm out.bin out2.bin B3_2_file B3_1_file 
else
    g++ $1 -I /opt/homebrew/Cellar/boost/1.89.0/include -L /opt/homebrew/Cellar/boost/1.89.0/lib -lboost_serialization -lboost_iostreams -o "${1%.cpp}_file"
fi
