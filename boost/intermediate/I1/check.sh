#!/bin/bash

echo "---------------------------"
echo "Using cat to check data.bin:"
echo " "
cat data.bin

echo  "\n\n"
echo "---------------------------"
echo "Using gzip -dc to check data.bin:"
echo " "
gzip -dc data.bin
