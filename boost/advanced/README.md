# Content 

### A1 (ANNIE's RAWData)
Inspecting ANNIE's RAWData. Using `BoostStore` class to we open the content of the RAW Data. We use the class members `Initialise`, and `Print` to do that job.

### A2 (PMT)
Using the same procedure than A1. We focus in the PMTData's BoostStore. By using a helpper function (coppied from PMT decoder), we inspect the sub-BoostStore which has `CardData` object. 

### A3 (PMT + ROOT)
This example basically uses the previus one but stores the information of the PMTData BoostStore to a ROOT Tree format by reading the CardData objects per event.

### A4 (MRD + ROOT)
With this example we inspect the MRD BoostStore. The sub-BoostStore for MRD data is named CCData. Similar to the PMT, we use a `MRDOut` class. Additionally, if ROOT is installed, it can save that information in a ROOT Tree.

### A5 (LAPPD)
Following the same procedure that PMT and MRD, here we inspect the LAPPD data, the ROOT convertion is not implemented. 
