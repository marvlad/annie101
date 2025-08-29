#include<iostream>
#include "BoostStore.h"

int main(){
	// BoostStore(bool typechecking=true, int format=0), format 0=binary, 1=text, 2=multievent

	// Create a holder BoostStore
	BoostStore *RawData = new BoostStore(false,0);

	// Get the information from the file
	RawData->Initialise("../data/RAWDataR5452S0p60");

	// Show the content
	RawData->Print(false);
	
	// From the printed in the RawData
	// CCData =>  
	// LAPPDData =>  
	// PMTData =>  
	// TrigData =>  
	// m_format =>  
	// m_typechecking => 

	// testing initializing by reference and pointer?
	BoostStore Trigger(false,2);
	BoostStore *PMTData = new BoostStore(false,2);
	BoostStore *LAPPDData = new BoostStore(false,2);
	BoostStore *CCData = new BoostStore(false,2);

	// In order to get the sub-boostStores we use Get
	RawData->Get("TrigData",Trigger);
	RawData->Get("PMTData",*PMTData);
	RawData->Get("LAPPDData",*LAPPDData);
	RawData->Get("CCData",*CCData);

	// Print the values
	Trigger.Print(false);
	PMTData->Print(false);
	LAPPDData->Print(false);
	CCData->Print(false);

	// Accesing further given by the previus print
	// TrigData =>  
	// CardData =>  
	// LAPPDData => 	
	
	// DataData?
	long trigtotalentriesData;
	CCData->Header->Get("TotalEntries",trigtotalentriesData);
	
	int TrigEntryNumData;
	CCData->GetEntry(TrigEntryNumData);

	std::cout << "CCData: " << std::endl;
	std::cout << "  TotalEntries: " << trigtotalentriesData << std::endl;
	std::cout << "  TrigEntryNum: " << TrigEntryNumData << std::endl;

	// TriggerData 
	long trigtotalentries;
	Trigger.Header->Get("TotalEntries",trigtotalentries);
	
	int TrigEntryNum;
	Trigger.GetEntry(TrigEntryNum);

	std::cout << "TriggerData: " << std::endl;
	std::cout << "  TotalEntries: " << trigtotalentries << std::endl;
	std::cout << "  TrigEntryNum: " << TrigEntryNum << std::endl;

	// PMTData
	long trigtotalentriesPMT;
	PMTData->Header->Get("TotalEntries",trigtotalentriesPMT);

	int TrigEntryNumPMT;
	PMTData->GetEntry(TrigEntryNumPMT);

	std::cout << "PMTData: " << std::endl;
	std::cout << "  TotalEntries: " << trigtotalentriesPMT << std::endl;
	std::cout << "  TrigEntryNum: " << TrigEntryNumPMT << std::endl;

	// LAPPDData
	long trigtotalentriesLAPPD;
	LAPPDData->Header->Get("TotalEntries",trigtotalentriesLAPPD);

	int TrigEntryNumLAPPD;
	LAPPDData->GetEntry(TrigEntryNumLAPPD);

	std::cout << "LAPPDData: " << std::endl;
	std::cout << "  TotalEntries: " << trigtotalentriesLAPPD << std::endl;
	std::cout << "  TrigEntryNum: " << TrigEntryNumLAPPD << std::endl;

	return 0;
}
