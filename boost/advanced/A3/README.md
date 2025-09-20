# ANNIE PMT Data

## Classes used:
The code gets the ANNIE's PMT data from the RAWData files. It focuses in two classes.
- BoostStore
- std::vector<CardData>
The `CardData` class is modified so it does not have a ZMQ dependens. Additionally, parts of the PMT decoder code is added in the `include/pmt_helper.h` file. 

## Usage:
Use the Makefile to compile the file. This example additionally takes the rawdata to rootfile format of the waveforms.
