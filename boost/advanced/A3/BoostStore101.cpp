#include <iostream>
#include <map>
#include <vector>
#include <cstdint>

#include "pmt_helper.h"
#include "TFile.h"
#include "TTree.h"

// Merge helper: merges one WaveBank-like map into another
static void MergeWaveBank(
    std::map<std::vector<int>, std::vector<uint16_t>>& dest,
    const std::map<std::vector<int>, std::vector<uint16_t>>& src)
{
    for (const auto& kv : src) {
        // If the same (CardID,ChannelID) key appears twice, last one wins.
        dest[kv.first] = kv.second;
    }
}

void saveToRoot(
    const std::vector<std::map<std::vector<int>, std::vector<uint16_t>>>& Fulldata,
    double ADC_TO_VOLT,
    int verb)
{
    std::cout << " ------------ Fulldata 2 ROOTFILE ---------------" << std::endl;
    TFile* f = new TFile("PMTData.root", "RECREATE");
    TTree* TriggerTree = new TTree("TriggerTree", "One entry per trigger/PMTData entry");

    int TriggerEntry = -1;
    int NWaveformsInTrigger = 0;

    TriggerTree->Branch("TriggerEntry", &TriggerEntry, "TriggerEntry/I");
    TriggerTree->Branch("NWaveforms", &NWaveformsInTrigger, "NWaveforms/I");

    TTree* WaveformTree = new TTree("PMTTree", "One entry per waveform");

    // Branch variables
    Long64_t WaveformEntry = 0;       // global waveform index: 0..Nwaveforms-1
    int WaveformInTrigger = 0;        // local waveform index inside this trigger

    unsigned int CrateNum = 0, SlotNum = 0, ChannelID = 0;
    std::vector<double> Amplitude;

    WaveformTree->Branch("TriggerEntry", &TriggerEntry, "TriggerEntry/I");
    WaveformTree->Branch("WaveformEntry", &WaveformEntry, "WaveformEntry/L");
    WaveformTree->Branch("WaveformInTrigger", &WaveformInTrigger, "WaveformInTrigger/I");
    WaveformTree->Branch("Crate", &CrateNum, "Crate/i");
    WaveformTree->Branch("Slot", &SlotNum, "Slot/i");
    WaveformTree->Branch("Channel", &ChannelID, "Channel/i");
    WaveformTree->Branch("Amplitude", &Amplitude);

    Long64_t totalWaveforms = 0;

    // Loop over triggers (Fulldata index == TriggerEntry)
    for (int trig = 0; trig < (int)Fulldata.size(); ++trig) {

        TriggerEntry = trig;
        NWaveformsInTrigger = (int)Fulldata[trig].size();

        // fill trigger-level tree ONCE per trigger
        TriggerTree->Fill();

        // fill waveform-level tree: one entry per waveform in this trigger
        WaveformInTrigger = 0;

        for (const auto& pair : Fulldata[trig]) {

            // IDs
            const int CardIDf    = pair.first.at(0);
            const int ChannelIDf = pair.first.at(1);

            const int SlotNumf  = CardIDf % 100;
            const int CrateNumf = CardIDf / 1000;

            CrateNum  = (unsigned int)CrateNumf;
            SlotNum   = (unsigned int)SlotNumf;
            ChannelID = (unsigned int)ChannelIDf;

            // waveform conversion
            Amplitude.clear();
            Amplitude.reserve(pair.second.size());

            for (const auto& amp : pair.second) {
                Amplitude.push_back((double)amp * ADC_TO_VOLT);
            }

            // bookkeeping
            WaveformEntry = totalWaveforms;
            totalWaveforms++;

            // Fill waveform Tree
            WaveformTree->Fill();
            WaveformInTrigger++;
        }
    }

    std::cout << "Triggers written (TriggerTree entries): " << TriggerTree->GetEntries() << std::endl;
    std::cout << "Waveforms written (PMTTree entries): " << WaveformTree->GetEntries() << std::endl;

    f->Write();
    f->Close();
    delete f;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <RAWData file>" << std::endl;
        return 1;
    }

    std::string rawdata_file = argv[1];

    BoostStore* RawData = new BoostStore(false, 0);
    RawData->Initialise(rawdata_file);
    RawData->Print(false);

    std::cout << " ------ Accesing to PMTData BoostStore ---------\n";
    BoostStore PMTData(false, 2);
    RawData->Get("PMTData", PMTData);
    PMTData.Print(false);

    long trigtotalentriesPMT = 0;
    PMTData.Header->Get("TotalEntries", trigtotalentriesPMT);

    std::cout << "trigtotalentriesPMT " << trigtotalentriesPMT << std::endl;

    // Fulldata[trigger] = map( [CardID,Channel] -> waveform )
    std::vector<std::map<std::vector<int>, std::vector<uint16_t>>> Fulldata;
    Fulldata.reserve((size_t)trigtotalentriesPMT);

    int verb = 1;

    // LOOP OVER ALL TRIGGERs
    for (int TrigEntryNumPMT = 0; TrigEntryNumPMT < (int)trigtotalentriesPMT; TrigEntryNumPMT++) {

        PMTData.GetEntry(TrigEntryNumPMT);

        std::vector<CardData> Cdata;
        PMTData.Get("CardData", Cdata);

        if (verb > 2) {
            std::cout << "Entry " << TrigEntryNumPMT
                      << " : Cdata.size() = " << Cdata.size() << std::endl;
        }

        // Build one bank for this trigger
        std::map<std::vector<int>, std::vector<uint16_t>> eventBank;

        for (unsigned int CardDataIndex = 0; CardDataIndex < Cdata.size(); CardDataIndex++) {
            const CardData& aCardData = Cdata.at(CardDataIndex);
            std::vector<DecodedFrame> ThisCardDFs = DecodeFrames(aCardData.Data);
            WaveBank.clear();

            for (unsigned int i = 0; i < ThisCardDFs.size(); i++) {
                ParseFrame(aCardData.CardID, ThisCardDFs.at(i));
            }
            MergeWaveBank(eventBank, WaveBank);
        }
        Fulldata.push_back(std::move(eventBank));
    }
    saveToRoot(Fulldata, ADC_TO_VOLT, 0);

    delete RawData;
    return 0;
}
