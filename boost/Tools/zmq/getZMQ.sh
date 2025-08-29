#!/bin/bash

git clone https://github.com/ToolDAQ/zeromq-4.0.7.git
echo "------> Done with git clone zqm"
cd zeromq-4.0.7
./configure --prefix=`pwd`
echo "------> Done configuring zqm"
make -j8
echo "------> Done \'make -j8\'"
make install
echo "------> Done \'make install\'"
export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
