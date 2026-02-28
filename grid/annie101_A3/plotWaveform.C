void plotWaveform() {
    // Open the ROOT file
    TFile *f = new TFile("PMTData.root", "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Cannot open PMTData.root" << std::endl;
        return;
    }

    // Get the TTree
    TTree *PMTTree = (TTree*)f->Get("PMTTree");
    if (!PMTTree) {
        std::cerr << "Error: Cannot find PMTTree in PMTData.root" << std::endl;
        f->Close();
        delete f;
        return;
    }

    // Define variables for branches
    int Event;
    unsigned int CrateNum, SlotNum, ChannelID;
    std::vector<double> *Amplitude = nullptr;

    // Set branch addresses
    PMTTree->SetBranchAddress("TriggerEntry", &Event);
    PMTTree->SetBranchAddress("Crate", &CrateNum);
    PMTTree->SetBranchAddress("Slot", &SlotNum);
    PMTTree->SetBranchAddress("Channel", &ChannelID);
    PMTTree->SetBranchAddress("Amplitude", &Amplitude);

    // Loop through entries to find the matching event
    bool found = false;
    Long64_t nEntries = PMTTree->GetEntries();
    std::cout << " nEntries = " << nEntries << std::endl;
    for (Long64_t i = 0; i < nEntries; i++) {
        PMTTree->GetEntry(i);
        //std::cout << "Event " << Event << std::endl;
        if (Event == 1 && CrateNum == 1 && SlotNum == 4 && ChannelID == 1) {
        //if (CrateNum == 1 && SlotNum == 4 && ChannelID == 1) {
            found = true;

            // Create histogram for the waveform
            int nBins = Amplitude->size(); // Number of bins = size of Amplitude vector
            float binWidth = 2.0; // 2 ns per bin
            TH1F *hWaveform = new TH1F("hWaveform", "PMT Waveform;Time (ns);Amplitude (V)", nBins, 0, nBins * binWidth);

            // Fill histogram with Amplitude values
            for (size_t j = 0; j < Amplitude->size(); j++) {
                hWaveform->SetBinContent(j + 1, (*Amplitude)[j]);
            }

            // Create a canvas and draw the histogram
            TCanvas *c1 = new TCanvas("c1", "PMT Waveform", 720, 600);
            hWaveform->Draw("HIST");
            c1->Update();

            // Keep the canvas open until user closes it
            c1->WaitPrimitive();
            break; // Exit loop after plotting
        }
    }

    if (!found) {
        std::cerr << "Error: No entry found for Event==2, Crate==1, Slot==4, Channel==1" << std::endl;
    }

    f->Close();
    delete f;
}
