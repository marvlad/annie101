// Example to check trigger data
// ....

#include<iostream>
#include "BoostStore.h"
#include "TriggerData.h"

void printall(BoostStore Trigger, int events){

        Trigger.GetEntry(events);

        //std::cout << "TriggerData: " << std::endl;
        //std::cout << "  TotalEntries: " << trigtotalentries << std::endl;
        //std::cout << "  TrigEntryNum: " << TrigEntryNum << std::endl;

        TriggerData *TData = new TriggerData;
        Trigger.Get("TrigData", TData);

        std::cout << "Firmware version: " << TData->FirmwareVersion << std::endl;

        std::vector<uint16_t> EventIDs;
        std::vector<uint64_t> EventTimes;

        EventIDs = TData->EventIDs;
        EventTimes = TData->EventTimes;

        for(int i=0; i < EventIDs.size(); i++){
                std::cout << i << " " << EventIDs[i] << std::endl;
        } 

        for(int i=0; i < EventTimes.size(); i++){
                std::cout << i << " " << EventTimes[i] << std::endl;
        } 


}
int main(){
        // BoostStore(bool typechecking=true, int format=0), format 0=binary, 1=text, 2=multievent

        // Create a holder BoostStore
        BoostStore *RawData = new BoostStore(false,0);

        // Get the information from the file
        RawData->Initialise("../data/RAWDataR5452S0p60");

        // Show the content
        RawData->Print(false);

        // testing initializing by reference and pointer?
        BoostStore Trigger(false,2);

        // In order to get the sub-boostStores we use Get
        RawData->Get("TrigData",Trigger);

        // Print the values
        Trigger.Print(false);

        // TriggerData 
        long trigtotalentries;
        Trigger.Header->Get("TotalEntries",trigtotalentries);

        for (int i=0; i<trigtotalentries; i++){
                printall(Trigger,i);
        }

        int TrigEntryNum = 18;
        Trigger.GetEntry(TrigEntryNum);

        std::cout << "TriggerData: " << std::endl;
        std::cout << "  TotalEntries: " << trigtotalentries << std::endl;
        std::cout << "  TrigEntryNum: " << TrigEntryNum << std::endl;

        TriggerData *TData = new TriggerData;
        Trigger.Get("TrigData", TData);

        std::cout << "Firmware version: " << TData->FirmwareVersion << std::endl;

        std::vector<uint16_t> EventIDs;
        std::vector<uint64_t> EventTimes;

        EventIDs = TData->EventIDs;
        EventTimes = TData->EventTimes;

        for(int i=0; i < EventIDs.size(); i++){
                std::cout << i << " " << EventIDs[i] << std::endl;
        } 

        for(int i=0; i < EventTimes.size(); i++){
                std::cout << i << " " << EventTimes[i] << std::endl;
        } 

        return 0;
}
