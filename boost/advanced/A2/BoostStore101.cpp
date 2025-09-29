#include<iostream>
#include"pmt_helper.h"

int main(int argc, char* argv[]){
    if(argc < 2){ 
      std::cerr << "Usage: " << argv[0] << " <RAWData file>" << std::endl;
      return 1;
    }   

    std::string rawdata_file = argv[1];

    BoostStore *RawData = new BoostStore(false,0);
    RawData->Initialise(rawdata_file);
    RawData->Print(false);
	
	std::cout << " ------ Accesing to PMTData BoostStore ---------\n"; 
	BoostStore *PMTData = new BoostStore(false,2);
	RawData->Get("PMTData",*PMTData);
	PMTData->Print(false);

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
	// Access to more comprensible PMT
	
	std::cout << " ------------ Accessing to vector<map<std::vector<int>, std::vector<uint16_t>>> Fulldata  ---------------" << std::endl;
	int counts = 0;

	for (int evnt=0; evnt < Fulldata.size(); evnt++){
            std::cout << "Event " << evnt << std::endl;
	    // We iwll use pair on the content of the Fulldata vector, because is a map
	    for (const auto& pair : Fulldata[evnt]) {
	         counts++;
		 
		 // The first part of the map is std::vector<int>, key, Basically is like 'CardID, ChannelID', example  [1004, 1]
	         std::cout << "--> Key: ";
	         printVector(pair.first); 

		 // The second part of the map is std::vector<uint16_t>
		 std::cout << "--> Value: ";
		 printVector(pair.second);
	     }
	}

	std::cout << "Total counts: " << counts << std::endl;
	
	return 0;
}
