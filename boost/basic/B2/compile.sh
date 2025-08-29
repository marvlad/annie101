#!/bin/bash

if [ "$1" = "clean" ]; then
    rm write_file read_file archive.dat 
else
    g++ $1 -I /opt/homebrew/Cellar/boost/1.89.0/include -L /opt/homebrew/Cellar/boost/1.89.0/lib -lboost_serialization -lboost_iostreams -o "${1%.cpp}_file"
fi
