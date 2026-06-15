#include <PsecData.h>

PsecData::PsecData()
{
	VersionNumber = 0x0003;
	LAPPD_ID = 0;
    SetDefaults();
}

PsecData::PsecData(unsigned int id)
{
	VersionNumber = 0x0003;
	LAPPD_ID = id;
    SetDefaults();
}

PsecData::~PsecData()
{}

bool PsecData::SetDefaults()
{
	Timestamp="";
	FailedReadCounter=0;
	readRetval=0;
    
	return true;
}


bool PsecData::Print(){
	std::cout << "----------------------Sent data---------------------------" << std::endl;
	printf("Version number: 0x%04x\n", VersionNumber);
	printf("LAPPD ID: %i\n", LAPPD_ID);
	for(int i=0; i<BoardIndex.size(); i++)
	{
		printf("Board number: %i\n", BoardIndex[i]);
	}
	printf("Failed read attempts: %i\n", FailedReadCounter);
	printf("Waveform size: %li\n", RawWaveform.size());
	printf("ACC Infoframe size: %li\n", AccInfoFrame.size());
	if(errorcodes.size()==1 && errorcodes[0]==0x00000000)
	{
		printf("No errorcodes found all good: 0x%08x\n", errorcodes[0]);
	}else
	{
		printf("Errorcodes found: %li\n", errorcodes.size());
		for(unsigned int k=0; k<errorcodes.size(); k++)
		{
			printf("Errorcode: 0x%08x\n", errorcodes[k]);
	
		}
	}
	std::cout << "----------------------------------------------------------" << std::endl;
	
	return true;
}
