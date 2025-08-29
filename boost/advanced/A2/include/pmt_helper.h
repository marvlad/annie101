// Multiple code was cp from the ToolAnalysis (ANNIE), PMT data decoder tools. This helper
// in mainly to understand how to process annie raw data out of the context of tool chains.

#include <iostream>
#include "BoostStore.h"
#include "CardData.h"
#include <vector>
#include <bitset>
#include <math.h>
#include <cstdint>

// declaring global variables
int RECORD_HEADER_LABELPART1 = 0x000;
int RECORD_HEADER_LABELPART2 = 0xFFF;
int SYNCFRAME_HEADERID = 10;
std::map<std::vector<int>, std::vector<uint16_t>> WaveBank;
std::map<std::vector<int>, uint64_t> TriggerTimeBank;
bool OffsetVME03;
bool OffsetVME01;
bool OffsetPositive;
std::map<uint64_t,std::map<std::vector<int>,uint64_t>>* TimestampsFromTheFuture = nullptr;
uint64_t LastGoodTimestamp;
int ADCCountsToBuild;
bool NewWavesBuilt;
std::map<uint64_t, std::map<std::vector<int>, std::vector<uint16_t> > >* FinishedPMTWaves;
std::map<uint64_t, std::map<std::vector<int>, int> >* FIFOPMTWaves = nullptr;
unsigned int SAMPLES_RIGHTOF_000 = 7;
std::map<int,std::vector<uint64_t>> SyncCounters;
const double ADC_TO_VOLT = 2.415 / pow(2., 12);
bool verbosity = false;

#ifndef be32toh
inline uint32_t be32toh(uint32_t big_endian) {
    return ((big_endian >> 24) & 0xFF) |
           ((big_endian >> 8) & 0xFF00) |
           ((big_endian << 8) & 0xFF0000) |
           ((big_endian << 24) & 0xFF000000);
}
#endif

// Defining struc DecodedFrame
struct DecodedFrame{
  bool has_recordheader;
  uint32_t frameheader;
  std::vector<uint16_t> samples;
  std::vector<int> recordheader_starts; //Holds indices where a record header starts in samples
  ~DecodedFrame(){
  }
};

// Defining a vector of the struc DecodedFrame
std::vector<DecodedFrame> DecodeFrames(std::vector<uint32_t> bank) // the reason og std::vector<uint32_t> is because the the CardData has a member of that type
{
  uint64_t tempword=0;
  std::vector<DecodedFrame> frames;  // What we will return
  std::vector<uint16_t> samples;
  samples.resize(40); // Well, if there's 480 bits per frame of samples max, this fits it

  if(verbosity) {
    std::cout << "DECODING A CARDDATA'S DATA BANK (Size of vector<DecodedFrame> ).  SIZE OF BANK: " << bank.size() << std::endl;
    std::cout << "THIS SHOULD HOLD AN INTEGER NUMBER OF FRAMES. EACH FRAME HAS" << std::endl;
    std::cout << "512 BITs, split into 16 32-bit INTEGERS. THIS SHOUDL BE DIVISIBLE BY 16" << std::endl;
  }

  for (unsigned int frame = 0; frame<bank.size()/16; ++frame) {  // if each frame has 16 32-bit ints, nframes = bank_size/16
    struct DecodedFrame thisframe;
    int sampleindex          = 0;
    int wordindex            = 16*frame;  // Index of first 32-bit int for this frame
    int bitsleft             = 0;  // # bits to shift to the left in the temp word
    bool haverecheader_part1 = false;

    while (sampleindex < 40) {  // Parse out this whole frame
      if (bitsleft < 12) {

	if(verbosity) {
	  std::cout << "DATA STREAM STEP AT SAMPLE INDEX" << sampleindex << std::endl;
	  std::cout << " BANK[WORDINDEX="<<wordindex<<"]: " << bank[wordindex] << std::endl;
	}

        tempword += ((uint64_t)be32toh(bank[wordindex]))<<bitsleft;

        if(verbosity) std::cout << "DATA STREAM SNAPSHOT WITH NEXT 32-bit WORD FROM FRAME " << std::bitset<64>(tempword) << std::endl;

        bitsleft += 32;
        wordindex += 1;
      }

      //Logic to search for record headers
      if((int)(tempword&0xfff)==RECORD_HEADER_LABELPART1) haverecheader_part1 = true;
      else if (haverecheader_part1 && ((int)(tempword&0xfff)==RECORD_HEADER_LABELPART2)){
        if(verbosity) std::cout << "FOUND A RECORD HEADER. AT INDEX " << sampleindex << std::endl;
        thisframe.has_recordheader=true;
        thisframe.recordheader_starts.push_back(sampleindex-1);
        haverecheader_part1 = false;
      } else haverecheader_part1 = false;

      //Takie the first 12 bits of the tempword at each loop, and shift tempword
      samples[sampleindex] = tempword&0xfff;

      if(verbosity) std::cout << "FIRST 12 BITS IN THIS SNAPSHOT: " << std::bitset<16>(tempword&0xfff) << std::endl;

      tempword = tempword>>12;
      bitsleft -= 12;
      sampleindex += 1;
    } //END parse out this frame

    thisframe.frameheader = be32toh(bank[16*frame+15]);  //Frameid is held in the frame's last 32-bit word
    if(verbosity) std::cout << "FRAMEHEADER last 8 bits: " << std::bitset<32>(thisframe.frameheader>>24) << std::endl;

    thisframe.samples = samples;
    if(verbosity) std::cout << "LENGTH OF SAMPLES AFTER DECODING A FRAME: "  << thisframe.samples.size() << std::endl;

    frames.push_back(thisframe);
  }
  return frames;
}

void AddSamplesToWaveBank(int CardID, int ChannelID, 
        std::vector<uint16_t> WaveSlice)
{
  std::vector<int> wave_key{CardID,ChannelID};
  if(WaveBank.count(wave_key)==0){
    return;
  } else {
  WaveBank.at(wave_key).insert(WaveBank.at(wave_key).end(),
                               WaveSlice.begin(),
                               WaveSlice.end());
  }
  return;
}

void StoreFinishedWaveform(int CardID, int ChannelID)
{
  //Get the full waveform from the Wave Bank
  std::vector<int> wave_key{CardID,ChannelID};
  //Check there's a wave in the map
  if(WaveBank.count(wave_key)==0){
    return;
  }
  std::vector<uint16_t> FinishedWave = WaveBank.at(wave_key);
  uint64_t FinishedWaveTrigTime = TriggerTimeBank.at(wave_key);  //Conversion from counter ticks to ns
  if (CardID > 3000 && OffsetVME03) {
    if (OffsetPositive) FinishedWaveTrigTime += 8;
    else FinishedWaveTrigTime -= 8;	//Offset for VME03
  }
  if (CardID < 2000 && OffsetVME01) {
    if (OffsetPositive) FinishedWaveTrigTime += 8; //Offset for VME01
    else FinishedWaveTrigTime -= 8;
  }
  if (FinishedWaveTrigTime > 2000000000000000000) {
    WaveBank.erase(wave_key);
    TriggerTimeBank.erase(wave_key);
    std::map<std::vector<int>,uint64_t> MapFutureTimestamps;
    MapFutureTimestamps.emplace(wave_key,FinishedWaveTrigTime);
    if (TimestampsFromTheFuture->count(LastGoodTimestamp)==0){
      TimestampsFromTheFuture->emplace(LastGoodTimestamp,MapFutureTimestamps);
    } else {
      TimestampsFromTheFuture->at(LastGoodTimestamp).emplace(wave_key,FinishedWaveTrigTime);
    }
    return;		//Don't include times that are far off in the future (what is going on there?) [exclude everything beyond 18th of May 2033, ANNIE will probably not run that long...)
  }
  LastGoodTimestamp = FinishedWaveTrigTime;

  if((int)FinishedWave.size()>ADCCountsToBuild){
    NewWavesBuilt = true;
    if(FinishedPMTWaves->count(FinishedWaveTrigTime) == 0) {
      std::map<std::vector<int>, std::vector<uint16_t> > WaveMap;
      WaveMap.emplace(wave_key,FinishedWave);
      FinishedPMTWaves->emplace(FinishedWaveTrigTime,WaveMap);
      int FIFOstate = 0;
      if (FIFOstate == 1 || FIFOstate ==2){
        std::map<std::vector<int>,int> FIFOInfo;
        FIFOInfo.emplace(wave_key,FIFOstate);
        FIFOPMTWaves->emplace(FinishedWaveTrigTime,FIFOInfo);
      }
    } else {
      FinishedPMTWaves->at(FinishedWaveTrigTime).emplace(wave_key,FinishedWave);
      int FIFOstate = 0;
      if (FIFOstate == 1 || FIFOstate == 2) {
	if (FIFOPMTWaves->count(FinishedWaveTrigTime)==0){
          std::map<std::vector<int>,int> FIFOInfo;
          FIFOInfo.emplace(wave_key,FIFOstate);
          FIFOPMTWaves->emplace(FinishedWaveTrigTime,FIFOInfo);
        } else {
          FIFOPMTWaves->at(FinishedWaveTrigTime).emplace(wave_key,FIFOstate);
        }
      }
    }
  }
  //Clear the finished wave from WaveBank and TriggerTimeBank for the new wave
  //to start being put together
  WaveBank.erase(wave_key);
  TriggerTimeBank.erase(wave_key);
  return;
}

void ParseRecordHeader(int CardID, int ChannelID, std::vector<uint16_t> RH)
{
  //We need to get the MTC count and make a new entry in TriggerTimeBank and WaveBank
  //First 4 samples; Just get the bits from 24 to 37 (is counter (61 downto 48)
  //Last 4 samples; All the first 48 bits of the MTC count.
  if(verbosity) {
    std::cout << "BIT WORDS IN RECORD HEADER: " << std::endl;
    for (unsigned int j=0; j<RH.size(); j++){
      std::cout << std::bitset<16>(RH.at(j)) << std::dec << std::endl;
    }
  }
  std::vector<uint16_t> CounterEnd(RH.begin()+2, RH.begin()+4);
  std::vector<uint16_t> CounterBegin(RH.begin()+4, RH.begin()+8);
  uint64_t ClockCount=0;
  int samplewidth=12;  //each uint16 really only holds 12 bits of info. (see DecodeFrame)
  //std::reverse(CounterBegin.begin(),CounterBegin.end());
  for (unsigned int j=0; j<CounterBegin.size(); j++){
    ClockCount += ((uint64_t)CounterBegin.at(j) << j*samplewidth);
  }
  for (unsigned int j=0; j<CounterEnd.size(); j++){
    ClockCount += ((uint64_t)CounterEnd.at(j) << ((CounterBegin.size() + j)*samplewidth));
  }
  std::vector<int> wave_key{CardID,ChannelID};
  std::vector<uint16_t> Waveform;

  //Update the TriggerTimeBank and WaveBank with new entries, since this channel's
  //Wave data is coming up next
  TriggerTimeBank.emplace(wave_key,ClockCount*8);
  WaveBank.emplace(wave_key,Waveform);
  return;
}
void ParseSyncFrame(int CardID, DecodedFrame DF)
{
  if(verbosity) std::cout << "PRINTING ALL DATA IN A SYNC FRAME FOR CARD" << CardID << std::endl;
  uint64_t SyncCounter = 0;
  for (int i=0; i < 6; i++){
    if(verbosity) std::cout << "SYNC FRAME DATA AT INDEX " << i << ": " << DF.samples.at(i) << std::endl;
    SyncCounter += ((uint64_t)DF.samples.at(i)) << (12*i);
    if(verbosity) std::cout << "SYNC COUNTER WITH CURRENT SAMPLE PUT AT LEFT: " << SyncCounter << std::endl;
  }
  std::map<int, std::vector<uint64_t>>::iterator it = SyncCounters.find(CardID);
  if(it != SyncCounters.end()) SyncCounters.at(CardID).push_back(SyncCounter);
  else {
    std::vector<uint64_t> SyncVec{SyncCounter};
    SyncCounters.emplace(CardID,SyncVec);
  }
  return;
}

void ParseFrame(int CardID, DecodedFrame DF)
{
  //Decoded frame infomration is moved to the
  //TriggerTimeBank and WaveBank.
  //Get the ID in the frame header.  Need to know if a channel, or sync signal
  unsigned channel_mask;
  int ChannelID = DF.frameheader >> 24; //TODO: Use something more intricate?
                                  //Bitrange defined by Jonathan (511 downto 504)
  if(verbosity) std::cout << "Parsing frame with CardID and ChannelID-" <<
      CardID << "," << ChannelID << std::endl;
  if(!DF.has_recordheader && (ChannelID != SYNCFRAME_HEADERID)){
    //All samples are waveforms for channel record that already exists in the WaveBank.
    AddSamplesToWaveBank(CardID, ChannelID, DF.samples);
  } else if (ChannelID != SYNCFRAME_HEADERID){
    int WaveSecBegin = 0;
    //We need to get the rest of a wave from WaveSecBegin to where the header starts
    //FIXME: this works if there's already a wave being built.  You need to parse
    //a record header in the wavebank first if it's the first thing in the frame though
    if(verbosity) {
      for (unsigned int j = 0; j<DF.recordheader_starts.size(); j++){
          std::cout << DF.recordheader_starts.at(j) << std::endl;
      }
    }
    for (unsigned int j = 0; j<DF.recordheader_starts.size(); j++){
      //TODO: More graceful way to handle this?  It's already happened once
      if(WaveSecBegin>DF.recordheader_starts.at(j)){
        if(verbosity) std::cout << "" <<
            "This is likely due a 000FFF in the counter.  Skipping record header and " <<
            "continuing" << std::endl;
        continue;
      }
      if(verbosity) std::cout << "RECORD HEADER INDEX" << DF.recordheader_starts.at(j) << std::endl;
      if(verbosity) std::cout << "WAVESECBEGIN IS " << WaveSecBegin << std::endl;
      std::vector<uint16_t> WaveSlice(DF.samples.begin()+WaveSecBegin,
              DF.samples.begin()+DF.recordheader_starts.at(j));
      //Add this WaveSlice to the wave bank
      AddSamplesToWaveBank(CardID, ChannelID, WaveSlice);
      //Since we have acquired the wave up to the next record header, the wave is done.
      //Store it in the FinishedWaves map.
      StoreFinishedWaveform(CardID, ChannelID);
      //Now, we have the header coming next.  Get it and parse it, starting whatever
      //Entries in maps are needed.
      std::vector<uint16_t> RecordHeader(DF.samples.begin()+
              DF.recordheader_starts.at(j), DF.samples.begin()+
              DF.recordheader_starts.at(j)+SAMPLES_RIGHTOF_000+1);
      ParseRecordHeader(CardID, ChannelID, RecordHeader);
      WaveSecBegin = DF.recordheader_starts.at(j)+SAMPLES_RIGHTOF_000+1;
    }
    // No more record headers from here; just parse the rest of whatever
    // waveform is being looked at
    int WaveSecEnd = DF.samples.size()-1;
    std::vector<uint16_t> WaveSlice(DF.samples.begin()+WaveSecBegin,
            DF.samples.end());
    AddSamplesToWaveBank(CardID, ChannelID, WaveSlice);
  }
  else {
    ParseSyncFrame(CardID, DF);
  }
  return;
}

template<typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

