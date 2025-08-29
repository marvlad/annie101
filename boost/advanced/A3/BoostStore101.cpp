#include<iostream>
#include"pmt_helper.h"
#include"TFile.h"
#include"TTree.h"

//using namespace std;

void saveToRoot(const std::vector<std::map<std::vector<int>, std::vector<uint16_t>>>& Fulldata, double ADC_TO_VOLT, int verb) {
    std::cout << " ------------ Fulldata 2 ROOTFILE ---------------" << std::endl;
    
    // Create ROOT file and tree
    TFile *f = new TFile("PMTData.root", "RECREATE");
    TTree *PMTTree = new TTree("PMTTree", "PMT Data Tree");

    // Define branch variables
    int Event;
    unsigned int CrateNum, SlotNum, ChannelID;
    std::vector<double> Amplitude;

    // Set up branches
    PMTTree->Branch("Event", &Event, "Event/I");
    PMTTree->Branch("Crate", &CrateNum, "Crate/i");
    PMTTree->Branch("Slot", &SlotNum, "Slot/i");
    PMTTree->Branch("Channel", &ChannelID, "Channel/i");
    PMTTree->Branch("Amplitude", &Amplitude);

    int counts = 0;
    
    // Loop through Fulldata
    for (int evnt = 0; evnt < Fulldata.size(); evnt++) {
        std::cout << "Event " << evnt << std::endl;
        for (const auto& pair : Fulldata[evnt]) {
            counts++;

            // Extract IDs
            int CardIDf = pair.first.at(0);
            int ChannelIDf = pair.first.at(1);
            int SlotNumf = CardIDf % 100;
            int CrateNumf = CardIDf / 1000;

            // Convert to unsigned int
            unsigned int uCrateNum = (unsigned int)CrateNumf;
            unsigned int uSlotNum = (unsigned int)SlotNumf;
            unsigned int uChannelID = (unsigned int)ChannelIDf;

            // Set branch values
            Event = evnt;
            CrateNum = uCrateNum;
            SlotNum = uSlotNum;
            ChannelID = uChannelID;

            // Convert uint16_t waveform amplitudes to double with ADC_TO_VOLT
            Amplitude.clear();
            for (const auto& amp : pair.second) {
                Amplitude.push_back(amp * ADC_TO_VOLT);
            }

            // Fill the tree with waveform data
            PMTTree->Fill();
        }
    }

    std::cout << "Total counts: " << counts << std::endl;

    // Write and close the file
    PMTTree->Write();
    f->Close();
    delete f; // Clean up
}
int main(){
	BoostStore *RawData = new BoostStore(false,0);
	RawData->Initialise("../data/RAWDataR5452S0p60");
	RawData->Print(false);
	
	std::cout << " ------ Accesing to PMTData BoostStore ---------\n"; 
	BoostStore *PMTData = new BoostStore(false,2);
	RawData->Get("PMTData",*PMTData);
	PMTData->Print(false);

	long trigtotalentriesPMT;
	int TrigEntryNumPMT;
	
	// Using the class CardData
	std::vector<CardData>* Cdata = new std::vector<CardData>;

	// Get the CardData to the class CardData
	PMTData->Get("CardData",*Cdata);	
	
	// Check the size of the CardData vector
	std::cout << "Cdata->size() " << Cdata->size() << std::endl;	

	int FIFOstate;
	std::vector<std::map<std::vector<int>, std::vector<uint16_t>>> Fulldata;
	
	// Accessing to the class
	
	int verb = 1;
	for (unsigned int CardDataIndex=0; CardDataIndex<Cdata->size(); CardDataIndex++){

		CardData aCardData = Cdata->at(CardDataIndex);

		if(verb > 2){
		  std::cout<<"PMTDataDecoder Tool: Loading next CardData from entry's index " << CardDataIndex <<std::endl;
	          std::cout<<"PMTDataDecoder Tool: CardData's CardID = " << aCardData.CardID<<std::endl;
        	  std::cout<<"PMTDataDecoder Tool: CardData's data vector size = " << aCardData.Data.size()<<std::endl;
		}

		int FIFOstate =0;
		FIFOstate = aCardData.FIFOstate;
		if(verb > 2) printf("The FIFOstate is %d \n",FIFOstate);

		std::vector<DecodedFrame> ThisCardDFs;
		ThisCardDFs = DecodeFrames(aCardData.Data);
		if(verb > 2) std::cout << "ThisCardDFs size: " << ThisCardDFs.size() << std::endl;
		
		for (unsigned int i=0; i < ThisCardDFs.size(); i++){
			ParseFrame(aCardData.CardID,ThisCardDFs.at(i));
		}

		Fulldata.push_back(WaveBank);
	}

	// EXAMPLE:
	// Access to more comprensible PMT 2 ROOTFILEs
	
	saveToRoot(Fulldata,ADC_TO_VOLT,0);

	return 0;
}
