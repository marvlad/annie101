// Taking from LAPPD Decoder 

#include <string>
#include <iostream>
#include <map>
#include <bitset>
#include <fstream>
#include "PsecData.h"

#define NUM_CH 30
#define NUM_PSEC 5
#define NUM_SAMP 256
#define NUM_VECTOR_DATA 7795
#define NUM_VECTOR_PPS 16

// This tool, control variables (only used in this tool, every thing that is not an data object)
// Variables that you get from the config file
int ReadStore;         // read data from a StoreFile (tool chain start with this tool rather than LoadANNIEEvent)
int Nboards;           // total number of boards to load pedestal file, LAPPD number * 2
string PedFileName;    // store format pedestal file name
string PedFileNameTXT; // txt format pedestal file name
int DoPedSubtract;     // 1: do pedestal subtraction, 0: don't do pedestal subtraction
int LAPPDStoreReadInVerbosity;
int num_vector_data;
int num_vector_pps;
bool SelectSingleLAPPD;
int SelectedLAPPD;
bool mergingModeReadIn;
bool ReadStorePdeFile;
bool MultiLAPPDMap; // loading map of multiple LAPPDs from ANNIEEvent
bool loadOffsets;
bool LoadBuiltPPSInfo;
bool loadFromStoreDirectly;
// Variables that you need in the tool
int retval; // track the data parsing and meta parsing status
int eventNo;
double CLOCK_to_NSEC;
int errorEventsNumber;
bool runInfoLoaded;

// LAPPD tool chain, control variables (Will be shared in multiple LAPPD tools to show the state of the tool chain in each loop)
// Variables that you get from the config file
int stopEntries;        // stop tool chain after loading this number of PSEC data events
int NonEmptyEvents;     // count how many non empty data events were loaded
int NonEmptyDataEvents; // count how many non empty data events were loaded
bool PsecReceiveMode;   // Get PSEC data from CStore or Stores["ANNIEEvent"]. 1: CStore, 0: Stores["ANNIEEvent"]
string OutputWavLabel;
string InputWavLabel;
int NChannels;
int Nsamples;
int TrigChannel;
double SampleSize;
int LAPPDchannelOffset;
int PPSnumber;
bool loadPPS;
bool loadPSEC;
bool mergedEvent; // in some merged Event, the LAPPD events was merged to ANNIEEvent, but the data stream was not changed to be true. use this = 1 to read it
bool LAPPDana;    // run other LAPPD tools
// Variables that you need in the tool
bool isCFD;
bool isBLsub;
bool isFiltered;
int EventType; // 0: PSEC, 1: PPS
// LAPPD tool chain, data variables. (Will be used in multiple LAPPD tools)
// everything you get or set to Store, which means it may be used in other tools or it's from other tools
int LAPPD_ID;
vector<int> LAPPD_IDs;
vector<uint64_t> LAPPDLoadedTimeStamps;
vector<uint64_t> LAPPDLoadedTimeStampsRaw;
vector<uint64_t> LAPPDLoadedBeamgatesRaw;
vector<uint64_t> LAPPDLoadedOffsets;
vector<int> LAPPDLoadedTSCorrections;
vector<int> LAPPDLoadedBGCorrections;
vector<int> LAPPDLoadedOSInMinusPS;
vector<uint64_t> LAPPDLoadedBG_PPSBefore;
vector<uint64_t> LAPPDLoadedBG_PPSAfter;
vector<uint64_t> LAPPDLoadedBG_PPSDiff;
vector<int> LAPPDLoadedBG_PPSMissing;
vector<uint64_t> LAPPDLoadedTS_PPSBefore;
vector<uint64_t> LAPPDLoadedTS_PPSAfter;
vector<uint64_t> LAPPDLoadedTS_PPSDiff;
vector<int> LAPPDLoadedTS_PPSMissing;

vector<int> ParaBoards; // save the board index for this PsecData
//std::map<unsigned long, vector<Waveform<double>>> LAPPDWaveforms;

// This tool, data variables (only used in this tool, every thing that is an data object)
ifstream PedFile;   // stream for reading in the Pedestal Files
string NewFileName; // name of the new Data File
std::map<unsigned long, vector<int>> *PedestalValues;
std::vector<unsigned short> Raw_buffer;
std::vector<unsigned short> Parse_buffer;
vector<unsigned short> Parse_Buffer;
std::vector<int> ReadBoards;
std::map<int, vector<unsigned short>> data;
vector<unsigned short> meta;
vector<unsigned short> pps;
vector<int> LAPPD_ID_Channel;     // for each LAPPD, how many channels on it's each board
vector<int> LAPPD_ID_BoardNumber; // for each LAPPD, how many boards with it
std::map<uint64_t, PsecData> LAPPDDataMap;
std::map<uint64_t, uint64_t> LAPPDBeamgate_ns;
std::map<uint64_t, uint64_t> LAPPDTimeStamps_ns; // data and key are the same
std::map<uint64_t, uint64_t> LAPPDTimeStampsRaw;
std::map<uint64_t, uint64_t> LAPPDBeamgatesRaw;
std::map<uint64_t, uint64_t> LAPPDOffsets;
std::map<uint64_t, int> LAPPDTSCorrection;
std::map<uint64_t, int> LAPPDBGCorrection;
std::map<uint64_t, int> LAPPDOSInMinusPS;
std::map<std::string, bool> DataStreams;
std::vector<int> LAPPDEventIndex_ID; // for each LAPPD ID, count the index of current loaded event
// For example, this part file may have a ID=0 event, b ID=1 event, while loading data object c and loaded 3 ID=0 and 4 ID=1, I may have 
// LAPPDEventIndex_ID = {3, 4}

// save PPS info for the second order correction
std::map<uint64_t, uint64_t> LAPPDBG_PPSBefore;
std::map<uint64_t, uint64_t> LAPPDBG_PPSAfter;
std::map<uint64_t, uint64_t> LAPPDBG_PPSDiff;
std::map<uint64_t, int> LAPPDBG_PPSMissing;
std::map<uint64_t, uint64_t> LAPPDTS_PPSBefore;
std::map<uint64_t, uint64_t> LAPPDTS_PPSAfter;
std::map<uint64_t, uint64_t> LAPPDTS_PPSDiff;
std::map<uint64_t, int> LAPPDTS_PPSMissing;

// data variables don't need to be cleared in each loop
// these are loaded offset for event building
std::map<string, vector<uint64_t>> Offsets;             // Loaded offset, use string = run number + sub run number + partfile number as key.
std::map<string, vector<int>> Offsets_minus_ps;         // offset in ps, use offset - this/1e3 as the real offset
std::map<string, vector<int>> BGCorrections;            // Loaded BGcorrections, same key as Offsets, but offset saved on event by event basis in that part file, in unit of ticks
std::map<string, vector<int>> TSCorrections;            // TS corrections, in unit of ticks
std::map<string, vector<uint64_t>> BG_PPSBefore_loaded; // BG PPS before, in unit of ticks
std::map<string, vector<uint64_t>> BG_PPSAfter_loaded;  // BG PPS after, in unit of ticks
std::map<string, vector<uint64_t>> BG_PPSDiff_loaded;   // BG PPS Diff
std::map<string, vector<int>> BG_PPSMissing_loaded;     // BG PPS Missing
std::map<string, vector<uint64_t>> TS_PPSBefore_loaded; // TS PPS before, in unit of ticks
std::map<string, vector<uint64_t>> TS_PPSAfter_loaded;  // TS PPS after, in unit of ticks
std::map<string, vector<uint64_t>> TS_PPSDiff_loaded;   // TS PPS Diff
std::map<string, vector<int>> TS_PPSMissing_loaded;     // TS PPS Missing

int runNumber;
int subRunNumber;
int partFileNumber;
int eventNumberInPF;
std::ofstream debugStoreReadIn;

int verbosity = 2;

bool ReadPedestals(int boardNo)
{

    if (LAPPDStoreReadInVerbosity > 0)
        cout << "Getting Pedestals " << boardNo << endl;

    std::string LoadName = PedFileNameTXT;
    string nextLine; // temp line to parse
    double finalsampleNo;
    std::string ext = std::to_string(boardNo);
    ext += ".txt";
    LoadName += ext;
    PedFile.open(LoadName); // final name: PedFileNameTXT + boardNo + .txt
    if (!PedFile.is_open())
    {
        cout << "Failed to open " << LoadName << "!" << endl;
        return false;
    }
    if (LAPPDStoreReadInVerbosity > 0)
        cout << "Opened file: " << LoadName << endl;

    int sampleNo = 0; // sample number
    while (getline(PedFile, nextLine))
    {
        istringstream iss(nextLine);            // copies the current line in the file
        int location = -1;                      // counts the current perameter in the line
        string stempValue;                      // current string in the line
        int tempValue;                          // current int in the line
        unsigned long channelNo = boardNo * 30; // channel number
        // cout<<"NEW BOARD "<<channelNo<<" "<<sampleNo<<endl;
        // starts the loop at the beginning of the line
        while (iss >> stempValue)
        {
            location++;
            int tempValue = stoi(stempValue, 0, 10);
            if (sampleNo == 0)
            {
                vector<int> tempPed;
                tempPed.push_back(tempValue);
                // cout<<"First time: "<<channelNo<<" "<<tempValue<<endl;
                PedestalValues->insert(pair<unsigned long, vector<int>>(channelNo, tempPed));
                if (LAPPDStoreReadInVerbosity > 0)
                    cout << "Inserting pedestal at channelNo " << channelNo << endl;
                // newboard=false;
            }
            else
            {
                // cout<<"Following time: "<<channelNo<<" "<<tempValue<<"    F    "<<PedestalValues->count(channelNo)<<endl;
                (((PedestalValues->find(channelNo))->second)).push_back(tempValue);
            }

            channelNo++;
        }
        sampleNo++;
    }
    if (LAPPDStoreReadInVerbosity > 0)
        cout << "FINAL SAMPLE NUMBER: " << PedestalValues->size() << " " << (((PedestalValues->find(0))->second)).size() << endl;
    PedFile.close();
    return true;
}

bool MakePedestals()
{

    // Empty for now...
    // should be moved to ASCII readin?

    return true;
}

int getParsedMeta(std::vector<unsigned short> buffer, int BoardId)
{
    // Catch empty buffers
    if (buffer.size() == 0)
    {
        std::cout << "You tried to parse ACDC data without pulling/setting an ACDC buffer" << std::endl;
        return -1;
    }

    // Prepare the Metadata vector
    // meta.clear();

    // Helpers
    int chip_count = 0;

    // Indicator words for the start/end of the metadata
    const unsigned short startword = 0xBA11;
    unsigned short endword = 0xFACE;
    unsigned short endoffile = 0x4321;

    // Empty metadata map for each Psec chip <PSEC #, vector with information>
    map<int, vector<unsigned short>> PsecInfo;

    // Empty trigger metadata map for each Psec chip <PSEC #, vector with trigger>
    map<int, vector<unsigned short>> PsecTriggerInfo;
    unsigned short CombinedTriggerRateCount;

    // Empty vector with positions of aboves startword
    vector<int> start_indices =
        {
            1539, 3091, 4643, 6195, 7747};

    // Fill the psec info map
    vector<unsigned short>::iterator bit;
    for (int i : start_indices)
    {
        // Write the first word after the startword
        bit = buffer.begin() + (i + 1);

        // As long as the endword isn't reached copy metadata words into a vector and add to map
        vector<unsigned short> InfoWord;
        while (*bit != endword && *bit != endoffile && InfoWord.size() < 14)
        {
            InfoWord.push_back(*bit);
            ++bit;
        }
        PsecInfo.insert(pair<int, vector<unsigned short>>(chip_count, InfoWord));
        chip_count++;
    }

    // Fill the psec trigger info map
    for (int chip = 0; chip < NUM_PSEC; chip++)
    {
        for (int ch = 0; ch < NUM_CH / NUM_PSEC; ch++)
        {
            if (LAPPDStoreReadInVerbosity > 10)
                cout << "parsing meta step1-1" << endl;
            // Find the trigger data at begin + last_metadata_start + 13_info_words + 1_end_word + 1
            bit = buffer.begin() + start_indices[4] + 13 + 1 + 1 + ch + (chip * (NUM_CH / NUM_PSEC));
            if (LAPPDStoreReadInVerbosity > 10)
                cout << "parsing meta step1-2" << endl;
            PsecTriggerInfo[chip].push_back(*bit);
        }
    }

    if (LAPPDStoreReadInVerbosity > 10)
        cout << "parsing meta step1.5" << endl;
    // Fill the combined trigger
    CombinedTriggerRateCount = buffer[7792];

    //----------------------------------------------------------
    // Start the metadata parsing

    meta.push_back(BoardId);
    for (int CHIP = 0; CHIP < NUM_PSEC; CHIP++)
    {
        meta.push_back((0xDCB0 | CHIP));
        // cout<<"size of info word is "<<PsecInfo[CHIP].size()<<endl;
        // cout<<"size of trigger word is "<<PsecTriggerInfo[CHIP].size()<<endl;
        for (int INFOWORD = 0; INFOWORD < 13; INFOWORD++)
        {
            if (LAPPDStoreReadInVerbosity > 10)
                cout << "parsing meta step2-1 infoword " << INFOWORD << endl;
            if (PsecInfo[CHIP].size() < 13)
            {
                NonEmptyEvents = NonEmptyEvents - 1;
                NonEmptyDataEvents = NonEmptyDataEvents - 1;
                cout << "meta data parsing wrong! PsecInfo[CHIP].size() < 13" << endl;
                //m_data->CStore.Set("LAPPDana", false);
                return 1;
            }

            try
            {
                meta.push_back(PsecInfo[CHIP][INFOWORD]);
            }
            catch (...)
            {
                NonEmptyEvents = NonEmptyEvents - 1;
                NonEmptyDataEvents = NonEmptyDataEvents - 1;
                cout << "meta data parsing wrong! meta.push_back(PsecInfo[CHIP][INFOWORD]);" << endl;
                //m_data->CStore.Set("LAPPDana", false);
                return 1;
            }
        }
        for (int TRIGGERWORD = 0; TRIGGERWORD < 6; TRIGGERWORD++)
        {
            if (LAPPDStoreReadInVerbosity > 10)
                cout << "parsing meta step2-2 trigger word" << endl;

            if (PsecTriggerInfo[CHIP].size() < 6)
            {
                NonEmptyEvents = NonEmptyEvents - 1;
                NonEmptyDataEvents = NonEmptyDataEvents - 1;
                cout << "meta data parsing wrong! PsecTriggerInfo[CHIP].size() < 6" << endl;
                //m_data->CStore.Set("LAPPDana", false);
                return 1;
            }

            try
            {
                meta.push_back(PsecTriggerInfo[CHIP][TRIGGERWORD]);
            }
            catch (...)
            {
                NonEmptyEvents = NonEmptyEvents - 1;
                NonEmptyDataEvents = NonEmptyDataEvents - 1;
                cout << "meta data parsing wrong! meta.push_back(PsecTriggerInfo[CHIP][TRIGGERWORD]);" << endl;
                //m_data->CStore.Set("LAPPDana", false);
                return 1;
            }
        }
    }

    meta.push_back(CombinedTriggerRateCount);
    meta.push_back(0xeeee);
    return 0;
}

int getParsedData(std::vector<unsigned short> buffer, int ch_start)
{
    // Catch empty buffers
    if (buffer.size() == 0)
    {
        std::cout << "You tried to parse ACDC data without pulling/setting an ACDC buffer" << std::endl;
        return -1;
    }

    // Helpers
    int DistanceFromZero;
    int channel_count = 0;

    // Indicator words for the start/end of the metadata
    const unsigned short startword = 0xF005;
    unsigned short endword = 0xBA11;
    unsigned short endoffile = 0x4321;

    // Empty vector with positions of aboves startword
    vector<int> start_indices =
        {
            2, 1554, 3106, 4658, 6210};

    // Fill data map
    vector<unsigned short>::iterator bit;
    for (int i : start_indices)
    {
        // Write the first word after the startword
        bit = buffer.begin() + (i + 1);

        // As long as the endword isn't reached copy metadata words into a vector and add to map
        vector<unsigned short> InfoWord;
        while (*bit != endword && *bit != endoffile)
        {
            InfoWord.push_back((unsigned short)*bit);
            if (InfoWord.size() == NUM_SAMP)
            {
                data.insert(pair<int, vector<unsigned short>>(ch_start + channel_count, InfoWord));
                if (LAPPDStoreReadInVerbosity > 5)
                    cout << "inserted data to channel " << ch_start + channel_count << endl;
                InfoWord.clear();
                channel_count++;
            }
            ++bit;
        }
    }

    return 0;
}

bool LoadData()
{
    // TODO: when looping in Stores["ANNIEEvent"], the multiple PSEC data will be saved in std::map<double,PsecData> LAPPDDatas;
    // so we need to loop the map to get all data and waveforms, using the LAPPD_ID, board number, channel number to form a global channel-waveform map
    // then loop all waveforms based on channel number

    if (loadFromStoreDirectly)
        //m_data->Stores["ANNIEEvent"]->GetEntry(eventNo);
    if (LAPPDStoreReadInVerbosity > 2)
        cout << "Got eventNo " << eventNo << endl;

    // if loaded enough events, stop the loop and return false
    if (NonEmptyEvents == stopEntries || NonEmptyEvents > stopEntries || NonEmptyDataEvents == stopEntries || NonEmptyDataEvents > stopEntries)
    {
        if (LAPPDStoreReadInVerbosity > 0)
            cout << "LAPPDStoreReadIn: NonEmptyEvents is " << NonEmptyEvents << ", NonEmptyDataEvents is " << NonEmptyDataEvents << ", stopEntries is " << stopEntries << ", stop the loop" << endl;
        //m_data->vars.Set("StopLoop", 1);
        return false;
    }

    // if (mergedEvent)
    //     DataStreams["LAPPD"] = true;

    // print the load information: DataStreams["LAPPD"] value, PsecReceiveMode, MultiLAPPDMap
    if (LAPPDStoreReadInVerbosity > 0)
    {
        cout << "LAPPDStoreReadIn: DataStreams[LAPPD] is " << DataStreams["LAPPD"] << ", PsecReceiveMode is " << PsecReceiveMode << ", MultiLAPPDMap is " << MultiLAPPDMap << endl;
    }

    if (loadPSEC || loadPPS)
    { // if load any kind of data
        // if there is no LAPPD data in event store, and not getting data from CStore, return false, don't load
        if (!DataStreams["LAPPD"] && PsecReceiveMode == 0 && !MultiLAPPDMap) // if doesn't have datastream, not reveive data from raw data store (PsecReceiveMode), not loading multiple LAPPD map from processed data
        {
            return false;
        }
        else if (PsecReceiveMode == 1 && !MultiLAPPDMap) // no LAPPD data in event store, but load from CStore (for merging LAPPD to ANNIEEvent)
        {                                                // only get PSEC object from CStore
            // if loading from raw data, and the loading was set to pause, return false
            bool LAPPDRawLoadingPaused = false;
            //m_data->CStore.Get("PauseLAPPDDecoding", LAPPDRawLoadingPaused);
            if (LAPPDRawLoadingPaused)
            {
                //m_data->CStore.Set("NewLAPPDDataAvailable", false);
                return false;
            }

            PsecData dat;
            bool getData = true;//marvin//m_data->CStore.Get("LAPPDData", dat);
            if (getData)
            {
                // m_data->CStore.Set("StoreLoadedLAPPDData", dat);
            }
            if (LAPPDStoreReadInVerbosity > 0)
                cout << "LAPPDStoreReadIn: getting LAPPDData from CStore" << endl;
            bool mergingLoad;
            // if in merging mode, but no LAPPD data in CStore, return false, don't load
            // m_data->CStore.Get("LAPPDanaData", mergingLoad);
            if (!mergingLoad && mergingModeReadIn)
            {
                if (LAPPDStoreReadInVerbosity > 0)
                    cout << "LAPPDStoreReadIn: mergingMode is true but LAPPDanaData is false, set LAPPDana to false" << endl;
                return false;
            }
            if (getData)
            {
                vector<unsigned int> errorcodes = dat.errorcodes;
                if (errorcodes.size() == 1 && errorcodes[0] == 0x00000000)
                {
                    if (LAPPDStoreReadInVerbosity > 1)
                        printf("No errorcodes found all good: 0x%08x\n", errorcodes[0]);
                }
                else
                {
                    printf("When Loading PPS: Errorcodes found: %li\n", errorcodes.size());
                    for (unsigned int k = 0; k < errorcodes.size(); k++)
                    {
                        printf("Errorcode: 0x%08x\n", errorcodes[k]);
                    }
                    errorEventsNumber++;
                    return false;
                }
                ReadBoards = dat.BoardIndex;
                Raw_buffer = dat.RawWaveform;
                if (Raw_buffer.size() == 0 || ReadBoards.size() == 0)
                {
                    cout << "LAPPD Load Store, find Raw buffer size == 0 or ReadBoards size == 0" << endl;
                    return false;
                }
                LAPPD_ID = dat.LAPPD_ID;
                if (LAPPD_ID != SelectedLAPPD && SelectSingleLAPPD)
                    return false;
                //m_data->CStore.Set("PsecTimestamp", dat.Timestamp);
                if (LAPPDStoreReadInVerbosity > 2)
                {
                    cout << " Got Data " << endl;
                    dat.Print();
                }
                int frameType = static_cast<int>(Raw_buffer.size() / ReadBoards.size());
                if (LAPPDStoreReadInVerbosity > 0)
                    cout << "LAPPDStoreReadIn: got Data from CStore, frame type is " << frameType << endl;
                if (loadPSEC)
                {
                    if (frameType == num_vector_data)
                    {
                        //m_data->CStore.Set("LoadingPPS", false);
                        return true;
                    }
                }
                if (loadPPS)
                {
                    if (frameType == num_vector_pps)
                    {
                        //m_data->CStore.Set("LoadingPPS", true);
                        return true;
                    }
                }
                return false;
            }
            else
            {
                return false;
            }
        }
        else if (DataStreams["LAPPD"] && PsecReceiveMode == 0 && !MultiLAPPDMap) // if load single lappd data at ID 0, require Datastream, not receive from cstore, not in multiLAPPDMap mode
        {
            PsecData dat;
            //m_data->Stores["ANNIEEvent"]->Get("LAPPDData", dat);
            ReadBoards = dat.BoardIndex;
            Raw_buffer = dat.RawWaveform;
            LAPPD_ID = dat.LAPPD_ID;
            if (LAPPD_ID != SelectedLAPPD && SelectSingleLAPPD)
                return false;
            //m_data->CStore.Set("PsecTimestamp", dat.Timestamp);

            if (Raw_buffer.size() != 0 || ReadBoards.size() != 0)
            {
                if (LAPPDStoreReadInVerbosity > 0)
                {
                    cout << "Getting data length format" << static_cast<int>(Raw_buffer.size() / ReadBoards.size()) << ", psec timestamp is " << dat.Timestamp << endl;
                    cout << "ReadBoards size " << ReadBoards.size() << " Raw_buffer size " << Raw_buffer.size() << " LAPPD_ID " << LAPPD_ID << endl;
                }
            }
            else
            {
                cout << "LAPPDStoreReadIn: loading data with raw buffer size 0 or ReadBoards size 0, skip loading" << endl;
                cout << "ReadBoards size " << ReadBoards.size() << " Raw_buffer size " << Raw_buffer.size() << " LAPPD_ID " << LAPPD_ID << endl;

                return false;
            }
            return true;
        }
        else if (DataStreams["LAPPD"] && PsecReceiveMode == 0 && MultiLAPPDMap) // if not receive from cstore, and load multi lappd map
        {
            if (LAPPDStoreReadInVerbosity > 0)
                cout << "LAPPDLoadStore: Loading multiple LAPPD data from ANNIEEvent" << "Inside, size of LAPPDDatamap = " << LAPPDDataMap.size() << endl;

            if (LAPPDDataMap.size() == 0)
            {
                cout << "what happened?" << endl;
                return false;
            }

            return true;
        }
    }
    return false; // if not any of the above, return false
}

void ParsePPSData()
{
    if (LAPPDStoreReadInVerbosity > 0)
        cout << "Loading PPS frame size " << pps.size() << endl;
    std::vector<unsigned short> pps = Raw_buffer;
    std::vector<unsigned long> pps_vector;
    std::vector<unsigned long> pps_count_vector;

    unsigned long pps_timestamp = 0;
    unsigned long ppscount = 0;
    for (int s = 0; s < ReadBoards.size(); s++)
    {
        unsigned short pps_63_48 = pps.at(2 + 16 * s);
        unsigned short pps_47_32 = pps.at(3 + 16 * s);
        unsigned short pps_31_16 = pps.at(4 + 16 * s);
        unsigned short pps_15_0 = pps.at(5 + 16 * s);
        std::bitset<16> bits_pps_63_48(pps_63_48);
        std::bitset<16> bits_pps_47_32(pps_47_32);
        std::bitset<16> bits_pps_31_16(pps_31_16);
        std::bitset<16> bits_pps_15_0(pps_15_0);
        unsigned long pps_63_0 = (static_cast<unsigned long>(pps_63_48) << 48) + (static_cast<unsigned long>(pps_47_32) << 32) + (static_cast<unsigned long>(pps_31_16) << 16) + (static_cast<unsigned long>(pps_15_0));
        if (LAPPDStoreReadInVerbosity > 0)
            std::cout << "pps combined: " << pps_63_0 << std::endl;
        std::bitset<64> bits_pps_63_0(pps_63_0);
        // pps_timestamp = pps_63_0 * (CLOCK_to_NSEC); // NOTE: Don't do convert to ns because of the precision, do this in later tools
        pps_timestamp = pps_63_0;
        // LAPPDPPS->push_back(pps_timestamp);
        if (LAPPDStoreReadInVerbosity > 0)
            std::cout << "Adding timestamp " << pps_timestamp << " to LAPPDPPS" << std::endl;
        pps_vector.push_back(pps_timestamp);

        unsigned short ppscount_31_16 = pps.at(8 + 16 * s);
        unsigned short ppscount_15_0 = pps.at(9 + 16 * s);
        std::bitset<16> bits_ppscount_31_16(ppscount_31_16);
        std::bitset<16> bits_ppscount_15_0(ppscount_15_0);
        unsigned long ppscount_31_0 = (static_cast<unsigned long>(ppscount_31_16) << 16) + (static_cast<unsigned long>(ppscount_15_0));
        if (LAPPDStoreReadInVerbosity > 0)
            std::cout << "pps count combined: " << ppscount_31_0 << std::endl;
        std::bitset<32> bits_ppscount_31_0(ppscount_31_0);
        ppscount = ppscount_31_0;
        pps_count_vector.push_back(ppscount);

        if (LAPPDStoreReadInVerbosity > 8)
        {
            // Print the bitsets
            cout << "******************************" << endl;
            std::cout << "printing ACDC " << s << ": " << endl;
            std::cout << "bits_pps_63_48: " << bits_pps_63_48 << std::endl;
            std::cout << "bits_pps_47_32: " << bits_pps_47_32 << std::endl;
            std::cout << "bits_pps_31_16: " << bits_pps_31_16 << std::endl;
            std::cout << "bits_pps_15_0: " << bits_pps_15_0 << std::endl;
            // Print the unsigned shorts
            std::cout << "pps_63_48: " << pps_63_48 << std::endl;
            std::cout << "pps_47_32: " << pps_47_32 << std::endl;
            std::cout << "pps_31_16: " << pps_31_16 << std::endl;
            std::cout << "pps_15_0: " << pps_15_0 << std::endl;
            std::cout << "pps_63_0: " << pps_63_0 << std::endl;
            std::cout << "pps_63_0 after conversion in double: " << pps_timestamp << endl;

            for (int x = 0; x < 16; x++)
            {
                std::bitset<16> bit_pps_here(pps.at(x + 16 * s));
                cout << "unsigned short at " << x << " : " << pps.at(x + 16 * s) << ", bit at " << x << " is: " << bit_pps_here << endl;
                ;
            }
        }
    }

    // double ppsDiff = static_cast<double>(pps_vector.at(0)) - static_cast<double>(pps_vector.at(1));
    unsigned long ppsDiff = pps_vector.at(0) - pps_vector.at(1);
    /*
    m_data->CStore.Set("LAPPDPPScount0", pps_count_vector.at(0));
    m_data->CStore.Set("LAPPDPPScount1", pps_count_vector.at(1));
    m_data->CStore.Set("LAPPDPPScount", pps_count_vector);
    m_data->CStore.Set("LAPPDPPSDiff0to1", ppsDiff);
    m_data->CStore.Set("LAPPDPPSVector", pps_vector);

    m_data->CStore.Set("LAPPDPPStimestamp0", pps_vector.at(0));
    m_data->CStore.Set("LAPPDPPStimestamp1", pps_vector.at(1));
    m_data->CStore.Set("LAPPDPPShere", true);
    m_data->CStore.Set("LAPPD_ID", LAPPD_ID);
    */

    PPSnumber++;
}

bool ParsePSECData()
{
    if (LAPPDStoreReadInVerbosity > 0)
        std::cout << "PSEC Data Frame was read! Starting the parsing!" << std::endl;

    // while loading single PsecData Object, parse the data by LAPPDID and number of boards on each LAPPD and channel on each board
    //  Create a vector of paraphrased board indices
    //  the board indices goes with LAPPD ID. For example, LAPPD ID = 2, we will have board = 4,5
    //  this need to be converted to 0,1
    int nbi = ReadBoards.size();
    if (LAPPDStoreReadInVerbosity > 0 && nbi != 2)
        cout << "Number of board is " << nbi << endl;
    if (nbi == 0)
    {
        cout << "LAPPDStoreReadIn: error here! number of board is 0" << endl;
        errorEventsNumber++;
        return false;
    }
    if (nbi % 2 != 0)
    {
        errorEventsNumber++;
        cout << "LAPPDStoreReadIn: uneven number of boards in this event" << endl;
        if (nbi == 1)
        {
            ParaBoards.push_back(ReadBoards[0]);
        }
        else
        {
            return false;
        }
    }
    else
    {
        for (int cbi = 0; cbi < nbi; cbi++)
        {
            ParaBoards.push_back(cbi);
            if (LAPPDStoreReadInVerbosity > 2)
                cout << "Board " << cbi << " is added to the list of boards to be parsed!" << endl;
        }
    }
    // loop all boards, 0, 1
    if (LAPPDStoreReadInVerbosity > 2)
    {
        cout << "ParaBoards size is " << ParaBoards.size() << endl;
        for (int i = 0; i < ParaBoards.size(); i++)
        {
            cout << "ParaBoards " << i << " is " << ParaBoards[i] << endl;
        }
    }
    for (int i = 0; i < ParaBoards.size(); i++)
    {
        int bi = ParaBoards.at(i) % 2;
        Parse_buffer.clear();
        if (LAPPDStoreReadInVerbosity > 2)
            std::cout << "Parsing board with ReadBoards ID" << ReadBoards[bi] << std::endl;
        // Go over all ACDC board data frames by seperating them
        int frametype = static_cast<int>(Raw_buffer.size() / ReadBoards.size());
        for (int c = bi * frametype; c < (bi + 1) * frametype; c++)
        {
            Parse_buffer.push_back(Raw_buffer[c]);
        }
        if (LAPPDStoreReadInVerbosity > 2)
            std::cout << "Data for " << i << "_th board with board number = " << ReadBoards[bi] << " was grabbed!" << std::endl;

        // Grab the parsed data and give it to a global variable 'data'
        // insert the data start with channel number 30*ReadBoards[bi]
        // for instance, when bi=0 , LAPPD ID = 2, ReadBoards[bi] = 4, insert to channel number start with 120, to 150
        int channelShift = bi * NUM_CH + LAPPD_ID * NUM_CH * 2;
        retval = getParsedData(Parse_buffer, channelShift); //(because there are only 2 boards, so it's 0*30 or 1*30). Inserting the channel number start from this then ++ to 30
        if (retval == 0)
        {
            if (LAPPDStoreReadInVerbosity > 2)
                std::cout << "Data for board with number = " << ReadBoards[bi] << " was parsed with channel shift " << channelShift << endl;
            // Grab the parsed metadata and give it to a global variable 'meta'
            retval = getParsedMeta(Parse_buffer, bi + LAPPD_ID * 2);
            if (retval != 0)
            {
                std::cout << "Meta parsing went wrong! " << retval << endl;
                return false;
            }
            else
            {
                if (LAPPDStoreReadInVerbosity > 2)
                    std::cout << "Meta for board " << ReadBoards[bi] << " was parsed!" << std::endl;
            }
        }
        else
        {
            std::cout << "Parsing went wrong! " << retval << endl;
            return false;
        }
    }


    LAPPDEventIndex_ID[LAPPD_ID] += 1;
    if (LAPPDStoreReadInVerbosity > 1)
    {
        cout << "Adding one new event with LAPPD_ID = " << LAPPD_ID << " to the LAPPDEventIndex_ID, now it is: " << endl;
        for (int i = 0; i < LAPPDEventIndex_ID.size(); i++)
        {
            cout <<  LAPPDEventIndex_ID[i] << ", " << endl;
        }
        cout << endl;
    }

    if (LAPPDStoreReadInVerbosity > 2)
        cout << "Parsed all boards for this event finished" << endl;
    return true;
}

bool DoPedestalSubtract()
{
    if (LAPPDStoreReadInVerbosity > 0)
        cout << "DoPedestalSubtract()" << endl;
    if (DoPedSubtract == 0)
        return true;
    // Waveform<double> tmpWave;
    // vector<Waveform<double>> VecTmpWave;
    int pedval, val;
    if (LAPPDStoreReadInVerbosity > 3)
    {
        // print the size of data and all keys, and the size of PedestalValues and all keys
        cout << "Size of data is " << data.size() << endl;
        for (std::map<int, vector<unsigned short>>::iterator it = data.begin(); it != data.end(); ++it) // looping over the data map by channel number, from 0 to 60
        {
            cout << it->first << ", ";
        }
        cout << endl;
        cout << "Size of PedestalValues is " << PedestalValues->size() << endl;
        for (auto it = PedestalValues->begin(); it != PedestalValues->end(); ++it) // looping over the data map by channel number, from 0 to 60
        {
            cout << it->first << ", ";
        }
        cout << endl;
    }
    // Loop over data stream
    for (std::map<int, vector<unsigned short>>::iterator it = data.begin(); it != data.end(); ++it) // looping over the data map by channel number, from 0 to 60
    {
        int wrongPedChannel = 0;
        if (LAPPDStoreReadInVerbosity > 5)
            cout << "Do Pedestal sub at Channel " << it->first;

        for (int kvec = 0; kvec < it->second.size(); kvec++)
        { // loop all data point in this channel
            if (DoPedSubtract == 1)
            {
                auto iter = PedestalValues->find((it->first));
                if (kvec == 0 && LAPPDStoreReadInVerbosity > 5)
                    cout << std::fixed << ", found PedestalValues for channel " << it->first << " with value = " << iter->second.at(0);
                if (iter != PedestalValues->end() && iter->second.size() > kvec)
                {
                    pedval = iter->second.at(kvec);
                }
                else
                {
                    pedval = 0;
                    wrongPedChannel = (it->first);
                }
            }
            else
            {
                pedval = 0;
            }
            val = it->second.at(kvec);
            //tmpWave.PushSample(0.3 * (double)(val - pedval));
            if (LAPPDStoreReadInVerbosity > 5 && kvec < 10)
                cout << ", " << val << "-" << pedval << "=" << 0.3 * (double)(val - pedval);
        }
        if (wrongPedChannel != 0)
            cout << "Pedestal value not found for channel " << wrongPedChannel << "with it->first channel" << it->first << ", LAPPD channel shift " << LAPPD_ID * 60 << endl;

        //VecTmpWave.push_back(tmpWave);

        unsigned long pushChannelNo = (unsigned long)it->first;
        //LAPPDWaveforms.insert(pair<unsigned long, vector<Waveform<double>>>(pushChannelNo, VecTmpWave));
        // cout<<", Pushed to LAPPDWaveforms with channel number "<<pushChannelNo<<endl;

        //tmpWave.ClearSamples();
        //VecTmpWave.clear();
    }
    return true;
}

void SaveTimeStamps()
{
    unsigned short beamgate_63_48 = meta.at(7);
    unsigned short beamgate_47_32 = meta.at(27);
    unsigned short beamgate_31_16 = meta.at(47);
    unsigned short beamgate_15_0 = meta.at(67);
    std::bitset<16> bits_beamgate_63_48(beamgate_63_48);
    std::bitset<16> bits_beamgate_47_32(beamgate_47_32);
    std::bitset<16> bits_beamgate_31_16(beamgate_31_16);
    std::bitset<16> bits_beamgate_15_0(beamgate_15_0);
    unsigned long beamgate_63_0 = (static_cast<unsigned long>(beamgate_63_48) << 48) + (static_cast<unsigned long>(beamgate_47_32) << 32) + (static_cast<unsigned long>(beamgate_31_16) << 16) + (static_cast<unsigned long>(beamgate_15_0));
    std::bitset<64> bits_beamgate_63_0(beamgate_63_0);
    unsigned long beamgate_timestamp = beamgate_63_0 * (CLOCK_to_NSEC);
    //m_data->CStore.Set("LAPPDbeamgate", beamgate_timestamp);
    //m_data->CStore.Set("LAPPDBeamgate_Raw", beamgate_63_0);

    unsigned long BGTruncation = beamgate_63_0 % 8;
    unsigned long BGTruncated = beamgate_63_0 - BGTruncation;
    unsigned long BGInt = BGTruncated / 8 * 25;
    unsigned long BGIntTruncation = BGTruncation * 3;
    // save these two to CStore
    double BGFloat = BGTruncation * 0.125;
    unsigned long BGIntCombined = BGInt + BGIntTruncation;
    //m_data->CStore.Set("LAPPDBGIntCombined", BGIntCombined);
    //m_data->CStore.Set("LAPPDBGFloat", BGFloat);

    unsigned short timestamp_63_48 = meta.at(70);
    unsigned short timestamp_47_32 = meta.at(50);
    unsigned short timestamp_31_16 = meta.at(30);
    unsigned short timestamp_15_0 = meta.at(10);
    std::bitset<16> bits_timestamp_63_48(timestamp_63_48);
    std::bitset<16> bits_timestamp_47_32(timestamp_47_32);
    std::bitset<16> bits_timestamp_31_16(timestamp_31_16);
    std::bitset<16> bits_timestamp_15_0(timestamp_15_0);
    unsigned long timestamp_63_0 = (static_cast<unsigned long>(timestamp_63_48) << 48) + (static_cast<unsigned long>(timestamp_47_32) << 32) + (static_cast<unsigned long>(timestamp_31_16) << 16) + (static_cast<unsigned long>(timestamp_15_0));
    unsigned long lappd_timestamp = timestamp_63_0 * (CLOCK_to_NSEC);
    //m_data->CStore.Set("LAPPDtimestamp", lappd_timestamp);
    //m_data->CStore.Set("LAPPDTimestamp_Raw", timestamp_63_0);

    unsigned long TSTruncation = timestamp_63_0 % 8;
    unsigned long TSTruncated = timestamp_63_0 - TSTruncation;
    unsigned long TSInt = TSTruncated / 8 * 25;
    unsigned long TSIntTruncation = TSTruncation * 3;
    // save these two to CStore
    double TSFloat = TSTruncation * 0.125;
    unsigned long TSIntCombined = TSInt + TSIntTruncation;
    /*
    m_data->CStore.Set("LAPPDTSIntCombined", TSIntCombined);
    m_data->CStore.Set("LAPPDTSFloat", TSFloat);

    m_data->Stores["ANNIEEvent"]->Set("LAPPDbeamgate", beamgate_timestamp); // in ns
    m_data->Stores["ANNIEEvent"]->Set("LAPPDtimestamp", lappd_timestamp);   // in ns
    m_data->Stores["ANNIEEvent"]->Set("LAPPDBeamgate_Raw", beamgate_63_0);
    m_data->Stores["ANNIEEvent"]->Set("LAPPDTimestamp_Raw", timestamp_63_0);
    */
    if (LAPPDStoreReadInVerbosity > 11)
        debugStoreReadIn << eventNo << " LAPPDStoreReadIn, Saving timestamps, beamgate_timestamp: " << beamgate_63_0 << ", lappd_timestamp: " << timestamp_63_0 << endl;

    if (loadOffsets && runInfoLoaded)
    {
        // run number + sub run number + partfile number + LAPPD_ID to make a string key
        // TODO: need to add a reset number here after got everything work
        // search it in the maps, then load
        // marvin // SaveOffsets();
    }
    //m_data->CStore.Set("LAPPD_new_event", true);
}

/*
void SaveOffsets()
{
    int LoadingOffsetID = LAPPD_ID;
    if(LoadingOffsetID+1 > LAPPDEventIndex_ID.size())
    {
        LAPPDEventIndex_ID.resize(LoadingOffsetID+1);
    }
    int GetOffsetIndex_byID = LAPPDEventIndex_ID[LoadingOffsetID];
    if(LAPPDStoreReadInVerbosity>0)
        cout << "LAPPDStoreReadIn, SavingOffsets, LoadingOffset for LAPPD_ID: " << LoadingOffsetID << ", OffsetIndex of this event: " << GetOffsetIndex_byID << endl;

    std::string key = std::to_string(runNumber) + "_" + std::to_string(subRunNumber) + "_" + std::to_string(partFileNumber) + "_" + std::to_string(LAPPD_ID);

    int LAPPDBGCorrection = 0;
    int LAPPDTSCorrection = 0;
    int LAPPDOffset_minus_ps = 0;
    uint64_t LAPPDOffset = 0;

    uint64_t BG_PPSBefore = 0;
    uint64_t BG_PPSAfter = 0;
    uint64_t BG_PPSDiff = 0;
    uint64_t TS_PPSBefore = 0;
    uint64_t TS_PPSAfter = 0;
    uint64_t TS_PPSDiff = 0;
    int BG_PPSMissing = 0;
    int TS_PPSMissing = 0;

    // Check if the key exists and the index is within range for BGCorrections
    if (BGCorrections.find(key) != BGCorrections.end() && GetOffsetIndex_byID < BGCorrections[key].size())
    {
        LAPPDBGCorrection = BGCorrections[key][GetOffsetIndex_byID];
    }
    else
    {
        if (BGCorrections.find(key) == BGCorrections.end())
        {
            std::cerr << "Error: Key not found in BGCorrections: " << key << std::endl;
        }
        else
        {
            std::cerr << "Error: GetOffsetIndex_byID out of range for BGCorrections with key: " << key << std::endl;
        }
    }

    // Repeat the checks for TSCorrections, Offsets_minus_ps, and Offsets
    if (TSCorrections.find(key) != TSCorrections.end() && GetOffsetIndex_byID < TSCorrections[key].size())
    {
        LAPPDTSCorrection = TSCorrections[key][GetOffsetIndex_byID];
    }
    else
    {
        if (TSCorrections.find(key) == TSCorrections.end())
        {
            std::cerr << "Error: Key not found in TSCorrections: " << key << std::endl;
        }
        else
        {
            std::cerr << "Error: GetOffsetIndex_byID out of range for TSCorrections with key: " << key << std::endl;
        }
    }

    if (Offsets_minus_ps.find(key) != Offsets_minus_ps.end() && GetOffsetIndex_byID < Offsets_minus_ps[key].size())
    {
        LAPPDOffset_minus_ps = Offsets_minus_ps[key][GetOffsetIndex_byID];
    }
    else
    {
        if (Offsets_minus_ps.find(key) == Offsets_minus_ps.end())
        {
            std::cerr << "Error: Key not found in Offsets_minus_ps: " << key << std::endl;
        }
        else
        {
            std::cerr << "Error: GetOffsetIndex_byID out of range for Offsets_minus_ps with key: " << key << std::endl;
        }
    }

    if (Offsets.find(key) != Offsets.end() && GetOffsetIndex_byID < Offsets[key].size())
    {
        LAPPDOffset = Offsets[key][GetOffsetIndex_byID];
    }
    else
    {
        if (Offsets.find(key) == Offsets.end())
        {
            std::cerr << "Error: Key not found in Offsets: " << key << std::endl;
        }
        else
        {
            std::cerr << "Error: GetOffsetIndex_byID out of range for Offsets with key: " << key << std::endl;
        }
    }

    if (BG_PPSBefore_loaded.find(key) != BG_PPSBefore_loaded.end() && GetOffsetIndex_byID < BG_PPSBefore_loaded[key].size())
    {
        BG_PPSBefore = BG_PPSBefore_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (BG_PPSBefore_loaded.find(key) == BG_PPSBefore_loaded.end())
            std::cerr << "Error: Key not found in BG_PPSBefore_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for BG_PPSBefore_loaded with key: " << key << std::endl;
    }

    if (BG_PPSAfter_loaded.find(key) != BG_PPSAfter_loaded.end() && GetOffsetIndex_byID < BG_PPSAfter_loaded[key].size())
    {
        BG_PPSAfter = BG_PPSAfter_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (BG_PPSAfter_loaded.find(key) == BG_PPSAfter_loaded.end())
            std::cerr << "Error: Key not found in BG_PPSAfter_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for BG_PPSAfter_loaded with key: " << key << std::endl;
    }

    if (BG_PPSDiff_loaded.find(key) != BG_PPSDiff_loaded.end() && GetOffsetIndex_byID < BG_PPSDiff_loaded[key].size())
    {
        BG_PPSDiff = BG_PPSDiff_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (BG_PPSDiff_loaded.find(key) == BG_PPSDiff_loaded.end())
            std::cerr << "Error: Key not found in BG_PPSDiff_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for BG_PPSDiff_loaded with key: " << key << std::endl;
    }

    if (BG_PPSMissing_loaded.find(key) != BG_PPSMissing_loaded.end() && GetOffsetIndex_byID < BG_PPSMissing_loaded[key].size())
    {
        BG_PPSMissing = BG_PPSMissing_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (BG_PPSMissing_loaded.find(key) == BG_PPSMissing_loaded.end())
            std::cerr << "Error: Key not found in BG_PPSMissing_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for BG_PPSMissing_loaded with key: " << key << std::endl;
    }

    if (TS_PPSBefore_loaded.find(key) != TS_PPSBefore_loaded.end() && GetOffsetIndex_byID < TS_PPSBefore_loaded[key].size())
    {
        TS_PPSBefore = TS_PPSBefore_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (TS_PPSBefore_loaded.find(key) == TS_PPSBefore_loaded.end())
            std::cerr << "Error: Key not found in TS_PPSBefore_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for TS_PPSBefore_loaded with key: " << key << std::endl;
    }

    if (TS_PPSAfter_loaded.find(key) != TS_PPSAfter_loaded.end() && GetOffsetIndex_byID < TS_PPSAfter_loaded[key].size())
    {
        TS_PPSAfter = TS_PPSAfter_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (TS_PPSAfter_loaded.find(key) == TS_PPSAfter_loaded.end())
            std::cerr << "Error: Key not found in TS_PPSAfter_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for TS_PPSAfter_loaded with key: " << key << std::endl;
    }

    if (TS_PPSDiff_loaded.find(key) != TS_PPSDiff_loaded.end() && GetOffsetIndex_byID < TS_PPSDiff_loaded[key].size())
    {
        TS_PPSDiff = TS_PPSDiff_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (TS_PPSDiff_loaded.find(key) == TS_PPSDiff_loaded.end())
            std::cerr << "Error: Key not found in TS_PPSDiff_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for TS_PPSDiff_loaded with key: " << key << std::endl;
    }

    if (TS_PPSMissing_loaded.find(key) != TS_PPSMissing_loaded.end() && GetOffsetIndex_byID < TS_PPSMissing_loaded[key].size())
    {
        TS_PPSMissing = TS_PPSMissing_loaded[key][GetOffsetIndex_byID];
    }
    else
    {
        if (TS_PPSMissing_loaded.find(key) == TS_PPSMissing_loaded.end())
            std::cerr << "Error: Key not found in TS_PPSMissing_loaded: " << key << std::endl;
        else
            std::cerr << "Error: GetOffsetIndex_byID out of range for TS_PPSMissing_loaded with key: " << key << std::endl;
    }

    // start to fill data
    
    m_data->CStore.Set("LAPPDBGCorrection", LAPPDBGCorrection);
    m_data->CStore.Set("LAPPDTSCorrection", LAPPDTSCorrection);
    m_data->CStore.Set("LAPPDOffset", LAPPDOffset);
    m_data->CStore.Set("LAPPDOffset_minus_ps", LAPPDOffset_minus_ps);

    m_data->CStore.Set("BG_PPSBefore", BG_PPSBefore);
    m_data->CStore.Set("BG_PPSAfter", BG_PPSAfter);
    m_data->CStore.Set("BG_PPSDiff", BG_PPSDiff);
    m_data->CStore.Set("BG_PPSMissing", BG_PPSMissing);
    m_data->CStore.Set("TS_PPSBefore", TS_PPSBefore);
    m_data->CStore.Set("TS_PPSAfter", TS_PPSAfter);
    m_data->CStore.Set("TS_PPSDiff", TS_PPSDiff);
    m_data->CStore.Set("TS_PPSMissing", TS_PPSMissing);
    
    if (TS_PPSMissing != BG_PPSMissing)
    {
        cout << "LAPPDLoadStore: BG_PPSMissing != TS_PPSMissing, BG_PPSMissing: " << BG_PPSMissing << ", TS_PPSMissing: " << TS_PPSMissing << endl;
    }

    // cout << "LAPPDStoreReadIn, Saving offsets and corrections, key: " << key << ", LAPPDOffset: " << LAPPDOffset << ", LAPPDOffset_minus_ps: " << LAPPDOffset_minus_ps << ", LAPPDBGCorrection: " << LAPPDBGCorrection << ", LAPPDTSCorrection: " << LAPPDTSCorrection << ", BG_PPSBefore: " << BG_PPSBefore << ", BG_PPSAfter: " << BG_PPSAfter << ", BG_PPSDiff: " << BG_PPSDiff << ", BG_PPSMissing: " << BG_PPSMissing << ", TS_PPSBefore: " << TS_PPSBefore << ", TS_PPSAfter: " << TS_PPSAfter << ", TS_PPSDiff: " << TS_PPSDiff << ", TS_PPSMissing: " << TS_PPSMissing << endl;

    if (LAPPDStoreReadInVerbosity > 11)
        debugStoreReadIn << eventNo << "+LAPPDStoreReadIn, Saving offsets and corrections, key: " << key << ", LAPPDOffset: " << LAPPDOffset << ", LAPPDOffset_minus_ps: " << LAPPDOffset_minus_ps << ", LAPPDBGCorrection: " << LAPPDBGCorrection << ", LAPPDTSCorrection: " << LAPPDTSCorrection << endl;
}
*/
/*
void LoadOffsetsAndCorrections()
{
    // load here from the root tree to:
    
    std::map<string, vector<uint64_t>> Offsets; //Loaded offset, use string = run number + sub run number + partfile number as key.
    std::map<string, vector<int>> Offsets_minus_ps; //offset in ps, use offset - this/1e3 as the real offset
    std::map<string, vector<int>> BGCorrections; //Loaded BGcorrections, same key as Offsets, but offset saved on event by event basis in that part file, in unit of ticks
    std::map<string, vector<int>> TSCorrections; //TS corrections, in unit of ticks
    

    TFile *file = new TFile("offsetFitResult.root", "READ");
    TTree *tree;
    file->GetObject("Events", tree);

    if (!tree)
    {
        std::cerr << "LAPPDStoreReadIn Loading offsets, Tree not found!" << std::endl;
        return;
    }

    int runNumber, subRunNumber, partFileNumber, LAPPD_ID;
    ULong64_t final_offset_ns_0, final_offset_ps_negative_0, EventIndex;
    ULong64_t BGCorrection_tick, TSCorrection_tick;

    ULong64_t BG_PPSBefore_tick;
    ULong64_t BG_PPSAfter_tick;
    ULong64_t BG_PPSDiff_tick;
    ULong64_t BG_PPSMissing_tick;
    ULong64_t TS_PPSBefore_tick;
    ULong64_t TS_PPSAfter_tick;
    ULong64_t TS_PPSDiff_tick;
    ULong64_t TS_PPSMissing_tick;
    ULong64_t TS_driftCorrection_ns;
    ULong64_t BG_driftCorrection_ns;

    tree->SetBranchAddress("runNumber", &runNumber);
    tree->SetBranchAddress("subRunNumber", &subRunNumber);
    tree->SetBranchAddress("partFileNumber", &partFileNumber);
    tree->SetBranchAddress("LAPPD_ID", &LAPPD_ID);
    tree->SetBranchAddress("EventIndex", &EventIndex);
    tree->SetBranchAddress("final_offset_ns_0", &final_offset_ns_0);
    tree->SetBranchAddress("final_offset_ps_negative_0", &final_offset_ps_negative_0);
    tree->SetBranchAddress("BGCorrection_tick", &BGCorrection_tick);
    tree->SetBranchAddress("TSCorrection_tick", &TSCorrection_tick);
    tree->SetBranchAddress("BG_PPSBefore_tick", &BG_PPSBefore_tick);
    tree->SetBranchAddress("BG_PPSAfter_tick", &BG_PPSAfter_tick);
    tree->SetBranchAddress("BG_PPSDiff_tick", &BG_PPSDiff_tick);
    tree->SetBranchAddress("BG_PPSMissing_tick", &BG_PPSMissing_tick);
    tree->SetBranchAddress("TS_PPSBefore_tick", &TS_PPSBefore_tick);
    tree->SetBranchAddress("TS_PPSAfter_tick", &TS_PPSAfter_tick);
    tree->SetBranchAddress("TS_PPSDiff_tick", &TS_PPSDiff_tick);
    tree->SetBranchAddress("TS_PPSMissing_tick", &TS_PPSMissing_tick);
    tree->SetBranchAddress("TS_driftCorrection_ns", &TS_driftCorrection_ns);
    tree->SetBranchAddress("BG_driftCorrection_ns", &BG_driftCorrection_ns);

    Long64_t nentries = tree->GetEntries();
    cout << "LAPPDStoreReadIn Loading offsets and corrections, total entries: " << nentries << endl;
    for (Long64_t i = 0; i < nentries; ++i)
    {
        tree->GetEntry(i);

        std::string key = std::to_string(runNumber) + "_" + std::to_string(subRunNumber) + "_" + std::to_string(partFileNumber) + "_" + std::to_string(LAPPD_ID);

        // Prepare the vector sizes for each map
        if (Offsets[key].size() <= EventIndex)
        {
            Offsets[key].resize(EventIndex + 1);
            Offsets_minus_ps[key].resize(EventIndex + 1);
            BGCorrections[key].resize(EventIndex + 1);
            TSCorrections[key].resize(EventIndex + 1);
            BG_PPSBefore_loaded[key].resize(EventIndex + 1);
            BG_PPSAfter_loaded[key].resize(EventIndex + 1);
            BG_PPSDiff_loaded[key].resize(EventIndex + 1);
            BG_PPSMissing_loaded[key].resize(EventIndex + 1);
            TS_PPSBefore_loaded[key].resize(EventIndex + 1);
            TS_PPSAfter_loaded[key].resize(EventIndex + 1);
            TS_PPSDiff_loaded[key].resize(EventIndex + 1);
            TS_PPSMissing_loaded[key].resize(EventIndex + 1);
        }

        // Now using EventIndex to place each event correctly
        Offsets[key][EventIndex] = final_offset_ns_0 + TS_driftCorrection_ns;
        Offsets_minus_ps[key][EventIndex] = static_cast<int>(final_offset_ps_negative_0);
        BGCorrections[key][EventIndex] = static_cast<int>(BGCorrection_tick) - 1000;
        TSCorrections[key][EventIndex] = static_cast<int>(TSCorrection_tick) - 1000;

        BG_PPSBefore_loaded[key][EventIndex] = BG_PPSBefore_tick;
        BG_PPSAfter_loaded[key][EventIndex] = BG_PPSAfter_tick;
        BG_PPSDiff_loaded[key][EventIndex] = BG_PPSDiff_tick;
        BG_PPSMissing_loaded[key][EventIndex] = static_cast<int>(BG_PPSMissing_tick) - 1000;
        TS_PPSBefore_loaded[key][EventIndex] = TS_PPSBefore_tick;
        TS_PPSAfter_loaded[key][EventIndex] = TS_PPSAfter_tick;
        TS_PPSDiff_loaded[key][EventIndex] = TS_PPSDiff_tick;
        TS_PPSMissing_loaded[key][EventIndex] = static_cast<int>(TS_PPSMissing_tick) - 1000;

        if (nentries > 10 && i % (static_cast<int>(nentries / 10)) == 0)
        {
            cout << "LAPPDStoreReadIn Loading offsets and corrections, " << i << " entries loaded" << endl;
            cout << "Printing key: " << key << ", EventIndex: " << EventIndex << ", final_offset_ns_0: " << final_offset_ns_0 << ", final_offset_ps_negative_0: " << final_offset_ps_negative_0 << ", BGCorrection_tick: " << BGCorrection_tick << ", TSCorrection_tick: " << TSCorrection_tick << ", BG_PPSMissing_tick: " << BG_PPSMissing_tick << ", TS_PPSMissing_tick: " << TS_PPSMissing_tick << ", TS_driftCorrection_ns: " << TS_driftCorrection_ns << ", BG_driftCorrection_ns: " << BG_driftCorrection_ns << endl;
        }
    }

    file->Close();
    delete file;

    // The data structures are now correctly filled and can be used as needed.
}
*/
void LoadRunInfo()
{
    if (LAPPDStoreReadInVerbosity > 0)
        cout << "LAPPDStoreReadIn, Loading run info" << endl;
    int PFNumberBeforeGet = partFileNumber;
    //m_data->CStore.Get("rawFileNumber", partFileNumber);
    //m_data->CStore.Get("runNumber", runNumber);
    //m_data->CStore.Get("subrunNumber", subRunNumber);

    if (partFileNumber != PFNumberBeforeGet)
    {
        eventNumberInPF = 0;
        // also set all value of LAPPDEventIndex_ID to be 0
        for (int i = 0; i < LAPPDEventIndex_ID.size(); i++)
        {
            LAPPDEventIndex_ID[i] = 0;
        }
    }
    else
    {
        eventNumberInPF++;
    }
    if (LAPPDStoreReadInVerbosity > 0)
        cout << "LAPPDStoreReadIn, Loaded run info, runNumber: " << runNumber << ", subRunNumber: " << subRunNumber << ", partFileNumber: " << partFileNumber << ", eventNumberInPF: " << eventNumberInPF << endl;
}
int LoopThroughPPSData(vector<unsigned short> pps){
  if (pps.size()==16){
    unsigned short pps_63_48 = pps.at(2);
    unsigned short pps_47_32 = pps.at(3);
    unsigned short pps_31_16 = pps.at(4);
    unsigned short pps_15_0 = pps.at(5);
    std::bitset < 16 > bits_pps_63_48(pps_63_48);
    std::bitset < 16 > bits_pps_47_32(pps_47_32);
    std::bitset < 16 > bits_pps_31_16(pps_31_16);
    std::bitset < 16 > bits_pps_15_0(pps_15_0);
    unsigned long pps_63_0 = (static_cast<unsigned long>(pps_63_48) << 48) + (static_cast<unsigned long>(pps_47_32) << 32) + (static_cast<unsigned long>(pps_31_16) << 16) + (static_cast<unsigned long>(pps_15_0));
    if (verbosity > 2) std::cout << "It is pps.size()==16, pps combined: " << pps_63_0 << std::endl;
    std::bitset < 64 > bits_pps_63_0(pps_63_0);
    unsigned long pps_timestamp = pps_63_0 * (CLOCK_to_NSEC);
    //LAPPDPPS->push_back(pps_timestamp);
    if (verbosity > 2) std::cout << "pps_timestamp " << pps_timestamp << std::endl;
    if (verbosity > 2) std::cout <<"Adding timestamp "<<pps_timestamp<<" to LAPPDPPS"<<std::endl;
  } else if (pps.size() == 32){
    unsigned short pps_63_48 = pps.at(2);
    unsigned short pps_47_32 = pps.at(3);
    unsigned short pps_31_16 = pps.at(4);
    unsigned short pps_15_0 = pps.at(5);
    std::bitset < 16 > bits_pps_63_48(pps_63_48);
    std::bitset < 16 > bits_pps_47_32(pps_47_32);
    std::bitset < 16 > bits_pps_31_16(pps_31_16);
    std::bitset < 16 > bits_pps_15_0(pps_15_0);
    unsigned long pps_63_0 = (static_cast<unsigned long>(pps_63_48) << 48) + (static_cast<unsigned long>(pps_47_32) << 32) + (static_cast<unsigned long>(pps_31_16) << 16) + (static_cast<unsigned long>(pps_15_0));
    if (verbosity > 2) std::cout << "It is pps.size()==32, pps combined: " << pps_63_0 << std::endl;
    std::bitset < 64 > bits_pps_63_0(pps_63_0);
    unsigned long pps_timestamp = pps_63_0 * (CLOCK_to_NSEC);
    if (verbosity > 2) std::cout << "pps_timestamp?..  " << pps_timestamp << std::endl;
    //LAPPDPPS->push_back(pps_timestamp);
    if (verbosity > 2) std::cout <<"Adding timestamp "<<pps_timestamp<<" to LAPPDPPS"<<std::endl;
    pps_63_48 = pps.at(18);
    pps_47_32 = pps.at(19);
    pps_31_16 = pps.at(20);
    pps_15_0 = pps.at(21);
    std::bitset < 16 > Bits_pps_63_48(pps_63_48);
    std::bitset < 16 > Bits_pps_47_32(pps_47_32);
    std::bitset < 16 > Bits_pps_31_16(pps_31_16);
    std::bitset < 16 > Bits_pps_15_0(pps_15_0);
    pps_63_0 = (static_cast<unsigned long>(pps_63_48) << 48) + (static_cast<unsigned long>(pps_47_32) << 32) + (static_cast<unsigned long>(pps_31_16) << 16) + (static_cast<unsigned long>(pps_15_0));
    if (verbosity > 2) std::cout << "pps combined: " << pps_63_0 << std::endl;
    std::bitset < 64 > Bits_pps_63_0(pps_63_0);
    unsigned long pps_timestamp_2 = pps_63_0 * (CLOCK_to_NSEC);
  } else {
    //Some error because the size of the pps data frame does not match 16 or 32
    std::cout <<"LAPPDDataDecoder: PPS vector does not have size 16 or 32! Abort"<<std::endl;
    std::cout <<"Size: "<<pps.size()<<std::endl;
    for (int i_pps=0; i_pps < pps.size(); i_pps++){std::cout << pps.at(i_pps)<<std::endl;}
    return 1;
  }
  return 0;
}
