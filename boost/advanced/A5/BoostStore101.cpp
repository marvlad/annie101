#include <iostream>
#include "PsecData.h"
#include "BoostStore.h"
#include <vector>
#include <bitset>
#include <math.h>
#include "lappd_helper.h"
//using namespace std;

int main(){
	BoostStore *RawData = new BoostStore(false,0);
	RawData->Initialise("../data/RAWDataR5452S0p60");
	RawData->Print(false);
	
        std::cout << " ------> LAPPDData <---------\n";
        BoostStore *LAPPDData = new BoostStore(false,2);
        RawData->Get("LAPPDData",*LAPPDData);
        LAPPDData->Print(false);

	int lappdtotalentries = 0;
        int LAPPDEntryNum = 0;

        LAPPDData->Header->Print(false);
	LAPPDData->Header->Get("TotalEntries",lappdtotalentries);
	LAPPDData->GetEntry(LAPPDEntryNum);

	std::cout << "LAPPD TotalEntries " << lappdtotalentries << std::endl;
	std::cout << "LAPPD LAPPDEntryNum " << LAPPDEntryNum << std::endl;
 
	PsecData* Ldata = new PsecData;
        LAPPDData->Get("LAPPDData", *Ldata);

        std::cout << "BoardIndex size in PsecData " << Ldata->BoardIndex.size() << std::endl;
	
	std::vector<unsigned short> Raw_Buffer = Ldata->RawWaveform;
	std::vector<int> BoardId_Buffer = Ldata->BoardIndex;
	
	if (Raw_Buffer.size() == 0) {
    	   std::cout <<"LAPPDDataDecoder: Encountered Raw Buffer size of 0! Abort!"<<std::endl;
  	}
	
	verbosity = 3;

        if (verbosity > 2) std::cout <<"LAPPDDataDecoder: Raw_Buffer.size(): "<<Raw_Buffer.size()<<", BoardId_Buffer.size(): "<<BoardId_Buffer.size()<<std::endl;
        int frametype = Raw_Buffer.size()/BoardId_Buffer.size();
        if (verbosity > 2) std::cout <<"LAPPDDataDecoder: Got frametype = "<<frametype<<std::endl;

        if(frametype!=NUM_VECTOR_DATA && frametype!=NUM_VECTOR_PPS)
        {   
            cout << "Problem identifying the frametype, size of raw vector was " << Raw_Buffer.size() << endl;
            cout << "It was expected to be either " << NUM_VECTOR_DATA*BoardId_Buffer.size() << " or " <<  NUM_VECTOR_PPS*BoardId_Buffer.size() << endl;
            cout << "Please check manually!" << endl;
        }

        if(frametype==NUM_VECTOR_PPS)
        {
         if (verbosity > 2) std::cout <<"LAPPDDataDecoder: Encountered PPS frame"<<std::endl;
            pps = Raw_Buffer;
            int thepps = LoopThroughPPSData(pps);
            //LAPPDDataBuilt = true;
        } 
        else {
            if (verbosity > 2) std::cout <<"LAPPDDataDecoder: Encountered data frame"<<std::endl;
    
            //Create a vector of paraphrased board indices
            int nbi = BoardId_Buffer.size();
            vector<int> ParaBoards;
            if(nbi%2!=0)
            {
              cout << "Why is there an uneven number of boards! this is wrong!" << endl;
               if(nbi==1)
               { 
                  ParaBoards.push_back(1);
               } else
               { 
                 //return false;
               }
            } else
            { 
              for(int cbi=0; cbi<nbi; cbi++)
              { 
                  ParaBoards.push_back(cbi);
              }
            }

            for(int bi: ParaBoards)
	    {
		Parse_Buffer.clear();
		if(verbosity>1) std::cout << "Starting with board " << BoardId_Buffer[bi] << std::endl;
		//Go over all ACDC board data frames by seperating them
		for(int c=bi*frametype; c<(bi+1)*frametype; c++)
		{   
		    Parse_Buffer.push_back(Raw_Buffer[c]);
		}
		if(verbosity>1) std::cout << "Data for board " << BoardId_Buffer[bi] << " was grabbed!" << std::endl;

		//Meta data parsing

		int retval = getParsedMeta(Parse_Buffer,BoardId_Buffer[bi]);
		if(retval!=0)
		{   
		  std::cout << "Meta parsing went wrong! " << retval << endl;
		} else
		{   
		  if(verbosity>1) std::cout << "Meta for board " << BoardId_Buffer[bi] << " was parsed!" << std::endl;
		}
	    }

            //LoopThroughMetaData();
            //meta.clear();

            //LAPPDPulses->push_back(*Ldata);
            //LAPPDDataBuilt = true;
        }	
	//std::cout << "LAPPDData vector size: " << Ldata->size() << endl;
        //Ldata->Print();
        
	/*
	for (int evnt = 0; evnt <  Ldata->size(); evnt++){
	    std::cout << "----> event " << evnt << "<----" << std::endl;
	    //std::cout << "RawWaveform size in PsecData " << Ldata[evnt]->RawWaveform.size() << std::endl;
	    //std::cout << "AccInfoFrame size in PsecData " << Ldata[evnt]->AccInfoFrame.size() << std::endl;
	    //std::cout << "BoardIndex size in PsecData " << Ldata[evnt]->BoardIndex.size() << std::endl;
	    //std::cout << "ReceiveData size in PsecData " << Ldata[evnt]->ReceiveData.size() << std::endl;
	    //std::cout << "Timestamp of PsecData " << Ldata[evnt]->Timestamp << std::endl;
	}
        */

	return 0;
}
