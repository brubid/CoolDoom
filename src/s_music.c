struct mus_info
{
	char header[4];
	unsigned short length, start, primaryChannelsNum, secondaryChannelsNum, numInstruments, gap;
	unsigned short* instrumentList;

};