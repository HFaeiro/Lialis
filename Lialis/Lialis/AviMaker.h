#pragma once

class AviMaker
{
public:	
	AviMaker()
	{}
	//AviMaker(const AviMaker& obj)
	//{}
	void addFrames(std::string&,unsigned int);
	void saveFile(std::string);




private:
	struct FRAMES {
		unsigned int size;
		std::string frame;
	};
	BOOL USING = FALSE;
	struct AVISTREAMHEADER {
		char FOURCC[5] = "strh";
		char tFOURCC[4];
		int  size = 56;
		std::string fccType = "vids";
		char fccHandler[5] = "GPJM";
		int  dwFlags = 0;
		WORD   wPriority;
		WORD   wLanguage;
		int  dwInitialFrames;
		int  dwScale = 1000;
		int  dwRate = 15912;
		int  dwStart = 0;
		int  dwLength;
		int  dwSuggestedBufferSize;
		int  dwQuality = 1;
		int  dwSampleSize = 0;
		RECT rcFrame = { 0 };
	};

	struct  MainAVIHeader
	{
		std::string FOURCC = "avih";
		int size = 56;
		int dwMicroSecPerFrame = 0; // frame display rate (or 0)
		int dwMaxBytesPerSec = 0; // max. transfer rate
		int dwPaddingGranularity = 0; // pad to multiples of this size;
		int dwFlags = 2320; // the ever-present flags
		int dwTotalFrames = 0; // # frames in file
		int dwInitialFrames = 0;
		int dwStreams = 1;
		int dwSuggestedBufferSize;
		int dwWidth = 1920;
		int dwHeight = 1080;
		int dwReserved[4] = { 0 };
	};

	struct HEADER {
		char FOURCC[5] = "RIFF";
		unsigned int FileSize = 300;
		char FileType[5] = "AVI ";

	};
	struct LIST {
		std::string FOURCC = "LIST";
		unsigned int listSize;
		std::string listType;
		std::string listData;

	};
	struct CHUNK {
		std::string FOURCC = "00dc";
		unsigned int ckSize;
		std::string ckData;

	};

	HEADER hRIFF;
	AVISTREAMHEADER AVISH;
	MainAVIHeader mAVIH;
	//std::vector<AVISTREAMHEADER> vAVISH;
	BITMAPINFOHEADER bih;
	WAVEFORMATEX WAV;
	std::vector<CHUNK> Chunk;
	std::vector<FRAMES> Frames;
	//std::vector<LIST> vList;


};

