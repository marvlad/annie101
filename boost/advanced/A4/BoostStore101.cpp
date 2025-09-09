// The idea here is to extract the MRD part from the RAWData with all the tools learned 
// in the previous examples.  Multiple parts of the this code were shamelessly copy from 
// several parts of the ToolAnalysis

#include<iostream>
#include "BoostStore.h"
#include <vector>
#include <bitset>
#include <math.h>
#include <cstdint>
#include "MRDOut.h"

#ifdef HAVE_ROOT
#include"TFile.h"
#include"TTree.h"
#endif

//using namespace std;
int verbose = 0;
bool DaylightSavings = false; // We love to suffer in ANNIE, why not UTC?!

uint64_t toUTC(long TimeStamp){
    // The timestamp in the datadecoder is used as:
    uint64_t TimeZoneShift; 
    TimeZoneShift = 21600000;
    bool DaylightSavings = false; // We love to suffer in ANNIE, why not UTC?!
    // from what ever to UTC
    uint64_t timestamp = static_cast<uint64_t>(TimeStamp); 
    timestamp = (timestamp+TimeZoneShift)*1E6;
    if(verbose > 3) std::cout << "Print the timestamp in UTC: " << timestamp << std::endl;
    return timestamp;
}

#ifdef HAVE_ROOT
void saveToRootfile(BoostStore *MRDData) {
    std::cout << " ------------ MRD Data 2 ROOTFILE ---------------" << std::endl;
    
    // Create ROOT file and tree
    TFile *f = new TFile("MRDRawData.root", "RECREATE");
    TTree *MRDTree = new TTree("MRDTree", "MRD Data Tree");

    int Event;
    unsigned int OutN, Trigger;
    std::vector<unsigned int> Value, Slot, Channel, Crate;
    std::vector<std::string> Type;
    uint64_t timestamp;

    // Set up branches
    MRDTree->Branch("Event", &Event, "Event/I");
    MRDTree->Branch("OutN", &OutN, "OutN/i");
    MRDTree->Branch("Trigger", &Trigger, "Trigger/i");
    MRDTree->Branch("Value", &Value);
    MRDTree->Branch("Slot", &Slot);
    MRDTree->Branch("Channel", &Channel);
    MRDTree->Branch("Crate", &Crate);
    MRDTree->Branch("Type", &Type);
    MRDTree->Branch("timestamp", &timestamp, "timestamp/i");
	
    MRDOut* Mdata = new MRDOut;
    MRDData->Get("Data",*Mdata);

    long trigtotalentriesMRD;
    MRDData->Header->Get("TotalEntries",trigtotalentriesMRD); // From the MRDData BoostStore

    int counts = 0;
    
    // Loop through Fulldata
    for (int evnt = 0; evnt < trigtotalentriesMRD; evnt++) {	
        if(evnt%100==0) std::cout << "Event " << evnt << std::endl;
	Event = counts;
 	Value = Mdata->Value; // hit time values
	Slot = Mdata->Slot;
	Channel = Mdata->Channel;
	Crate = Mdata->Crate;
	OutN = Mdata->OutN; // This is the number of event in this rawfile, same of the size of the vectors.
	Trigger = Mdata->Trigger; // What is this??
	Type = Mdata->Type; // Prints 'TDC' the number of vector size. If there other type?
	timestamp = toUTC(Mdata->TimeStamp);
	MRDTree->Fill();
	counts++;
    }

    std::cout << "Total counts: " << counts << std::endl;

    // Write and close the file
    MRDTree->Write();
    f->Close();
    delete f; // Clean up
}
#endif

void printMRDInfo(BoostStore *MRDData, int MRDEntryNum){
	MRDData->GetEntry(MRDEntryNum);
	MRDOut* Mdata = new MRDOut;
	MRDData->Get("Data",*Mdata);

	std::cout << "MRDEntryNum " << MRDEntryNum << std::endl;

	// The damm class has this members:
	//  unsigned int OutN, Trigger;
 	//  std::vector<unsigned int> Value, Slot, Channel, Crate;
	//                   --> From the MRD decoder we learn that Value is the hit time value
	//  std::vector<std::string> Type;
  	//  long TimeStamp;
	
	unsigned int OutN, Trigger;
	std::vector<unsigned int> Value, Slot, Channel, Crate;
	std::vector<std::string> Type;
	long TimeStamp;

	Value = Mdata->Value; // hit time values
	Slot = Mdata->Slot; 
	Channel = Mdata->Channel;
	Crate = Mdata->Crate;
	
	OutN = Mdata->OutN; // This is the number of event in this rawfile, same of the size of the vectors.
	Trigger = Mdata->Trigger; // What is this?? 
	
	Type = Mdata->Type; // Prints 'TDC' the number of vector size. If there other type?
	TimeStamp = Mdata->TimeStamp; // What is the units?, ha! from MRD decoder //in ms since 1970/1/1

	if(verbose > 3){	
		std::cout << "Sizes: Value " << Value.size() <<", Slot " << Slot.size() << ", Channel " << Channel.size() <<", Crate " << Crate.size() << std::endl; 
		for(int i=0; i<Value.size(); i++) std::cout << "Values " <<Value[i] << std::endl;	
		for(int i=0; i<Slot.size(); i++) std::cout << "Slots " <<Slot[i] << std::endl;	
		for(int i=0; i<Channel.size(); i++) std::cout << "Channels " <<Channel[i] << std::endl;	
		for(int i=0; i<Crate.size(); i++) std::cout << "Crates " <<Crate[i] << std::endl;	
		std::cout << "The size of the vec is: " << Value.size() << std::endl;
		std::cout << "The Trigger in MRDout class: " << Trigger << std::endl;
		std::cout << "The Type in MRDout class: " << Type.size() << std::endl;
		std::cout << "The OutN in MRDout class: " << OutN << std::endl;
	}

	//for(int i=0; i<Type.size(); i++) std::cout << "-> " << Type[i] << " " << std::endl;

	uint64_t timestamp = toUTC(TimeStamp);
 
	// Let's deal with the TDC
	bool cosmic_loopback = false;
	bool beam_loopback = false;
	int cosmic_tdc = -1;
	int beam_tdc = -1;
	std::vector<int> CrateSlotChannel_Beam{7,11,15};
	std::vector<int> CrateSlotChannel_Cosmic{7,11,14};
	
  	for (unsigned int i_data = 0; i_data < Crate.size(); i_data++){
	    int crate = Crate.at(i_data);
	    int slot = Slot.at(i_data);
	    int channel = Channel.at(i_data);
	    int hittimevalue = Value.at(i_data);
	    std::vector<int> CrateSlotChannel{crate,slot,channel};
	    unsigned long chankey = 999;
	    //if (MRDCrateSpaceToChannelNumMap.find(CrateSlotChannel) != MRDCrateSpaceToChannelNumMap.end()){
	    //  chankey = MRDCrateSpaceToChannelNumMap[CrateSlotChannel];
	    //}
	    //std::cout <<"crate: "<<crate<<", slot: "<<slot<<", channel: "<<channel<<", chankey: "<<chankey<<std::endl;
	    std::cout <<"crate: "<<crate<<", slot: "<<slot<<", channel: "<<channel<<", hittimevalue: "<<hittimevalue<<std::endl;
	    if (CrateSlotChannel != CrateSlotChannel_Beam && CrateSlotChannel != CrateSlotChannel_Cosmic && chankey != 999){
	      std::pair <unsigned long,int> keytimepair(chankey,hittimevalue);  //chankey will be 0 when looking at loopback channels that don't have an entry in the mapping-->skip
	      //MRDEvents[timestamp].push_back(keytimepair);
	    }
	    if (crate == 7 && slot == 11 && channel == 14) {cosmic_loopback=true; cosmic_tdc = hittimevalue;}   //FIXME: don't hard-code the trigger channels?
	    if (crate == 7 && slot == 11 && channel == 15) {beam_loopback=true; beam_tdc = hittimevalue;}     //FIXME: don't hard-code the trigger channels?
 	 }
}

int main(int argc, char* argv[]){
    if(argc < 2){
       std::cerr << "Usage: " << argv[0] << " <RAWData file (example RAWDataR5454S0p131)>" << std::endl;
       return 1;
    }

    std::string rawdata_file = argv[1];

    BoostStore *RawData = new BoostStore(false,0);
    RawData->Initialise(rawdata_file);
    RawData->Print(false);

    std::cout << " ------ Accesing to MRD Data BoostStore ---------\n";
	BoostStore *MRDData = new BoostStore(false,2);
	RawData->Get("CCData",*MRDData);
	MRDData->Print(false);

	long trigtotalentriesMRD;
	MRDData->Header->Get("TotalEntries",trigtotalentriesMRD); // From the MRDData BoostStore
	std::cout << "MRD total entries: " << trigtotalentriesMRD << std::endl;

	// What else we can get from this binary?
	// Fuck we need to define the MRD class 'MRDOut'
	//MRDOut* Mdata = new MRDOut;
	//MRDData->Get("Data",*Mdata);
    // int MRDEntryNum = 1; // we will need a for loop from 0 to trigtotalentriesMRD to get all data in this rawfile

	// Print out the values
	for(int i=0; i<trigtotalentriesMRD; i++) printMRDInfo(MRDData,i);

	// Saving in rootfile
	#ifdef HAVE_ROOT	
	saveToRootfile(MRDData);	
	#endif

	return 0;
}
