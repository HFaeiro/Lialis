#include "stdafx.h"
#include "AviMaker.h"
#include <fstream>

void AviMaker::addFrames(std::string& Frame, unsigned int size)
{
	CHUNK chunk;
		Chunk.push_back({ chunk.FOURCC, size, Frame });

}



void AviMaker::saveFile(std::string filename)
{

	if (!Chunk.size())
	{
		MessageBox(NULL, L"No Frames", L"Failed To Save File", 0);
		return ;
	}
	mAVIH.dwTotalFrames = Chunk.size();
	
	mAVIH.dwSuggestedBufferSize = Chunk[0].ckSize * 2;
	AVISH.dwLength = Chunk.size();
	std::fstream newAvi(filename.c_str(), std::ios::out | std::ios::binary);
	if (newAvi.is_open()) {
		
		for (int j = 0; j < Chunk.size(); j++) {
			if (Chunk[j].ckSize % 2) {
				Chunk[j].ckSize += 1;
			}
			hRIFF.FileSize += Chunk[j].ckSize;
		}
		
		newAvi.write(hRIFF.FOURCC, 4);
		newAvi.write((char*)&hRIFF.FileSize, 4);
		newAvi.write(hRIFF.FileType, 4);

		LIST list;
		char padd[22];
		memset(padd, 0, 22);

		list.listType = "hdrl";
		list.listSize = 320;
		newAvi.write((char*)&list.FOURCC[0], 4);
		newAvi.write((char*)&list.listSize, 4);
		newAvi.write((char*)list.listType.c_str(), 4);
		newAvi.write(&mAVIH.FOURCC[0], 4);
		newAvi.write((char*)&mAVIH.size, 4); //make sure size is right
		//newAvi.write((char*)&mAVIH.dwMicroSecPerFrame, 4);
		newAvi.write(padd, 4);
		newAvi.write((char*)&mAVIH.dwMaxBytesPerSec, 4); //can be NULL;
		newAvi.write((char*)&mAVIH.dwPaddingGranularity, 4); //can be NULL;
		newAvi.write((char*)&mAVIH.dwFlags, 4);
		newAvi.write((char*)&mAVIH.dwTotalFrames, 4);
		newAvi.write((char*)&mAVIH.dwInitialFrames, 4); //can be NULL;
		newAvi.write((char*)&mAVIH.dwStreams, 4);
		newAvi.write(padd, 4);
		//newAvi.write((char*)&mAVIH.dwSuggestedBufferSize, 4);
		mAVIH.dwWidth = 1920;
		mAVIH.dwHeight = 1080;
		newAvi.write((char*)&mAVIH.dwWidth, 4);
		newAvi.write((char*)&mAVIH.dwHeight, 4);
		newAvi.write(padd, 16);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		list.listType = "strl";
		list.listSize = 94;
		newAvi.write((char*)&list.FOURCC[0], 4);
		newAvi.write((char*)&list.listSize, 4);
		newAvi.write((char*)list.listType.c_str(), 4);
		newAvi.write(AVISH.FOURCC, 4);
		newAvi.write((char*)&AVISH.size, 4);
		newAvi.write(&AVISH.fccType[0], 4);
		newAvi.write(AVISH.fccHandler, 4);

		newAvi.write(padd, 12);


		newAvi.write((char*)&AVISH.dwScale, 4);
		newAvi.write((char*)&AVISH.dwRate, 4);
		newAvi.write((char*)&AVISH.dwStart, 4); //should be NULL
		newAvi.write((char*)&AVISH.dwLength, 4);
		newAvi.write((char*)&AVISH.dwSuggestedBufferSize, 4);
		newAvi.write((char*)&AVISH.dwQuality, 4); // usually -1
		newAvi.write((char*)&AVISH.dwSampleSize, 4); //can be NULL;

		newAvi.write((char*)&AVISH.rcFrame, sizeof(RECT));

		bih.biCompression = 1196444237;
		bih.biPlanes = (WORD)1;
		bih.biWidth = 1920;
		bih.biHeight = 1080;
		bih.biBitCount = (WORD)24;
		bih.biSize = 40;

		std::string strf = "strf";
		newAvi.write(&strf[0], 4);
		newAvi.write((char*)&bih.biSize, sizeof(int));
		newAvi.write((char*)&bih.biSize, sizeof(int));
		newAvi.write((char*)&bih.biWidth, sizeof(LONG));
		newAvi.write((char*)&bih.biHeight, sizeof(LONG));
		newAvi.write((char*)&bih.biPlanes, 2);
		newAvi.write((char*)&bih.biBitCount, sizeof(WORD));
		newAvi.write((char*)&bih.biCompression, 4);
		newAvi.write(padd, 20);


		list.listType = "movi";
		list.listSize = hRIFF.FileSize - 300;

		newAvi.write((char*)&list.FOURCC[0], 4);
		newAvi.write((char*)&list.listSize, 4);
		newAvi.write((char*)list.listType.c_str(), 4);
		size_t filesize = newAvi.tellp();
		for (int j = 0; j < Chunk.size(); j++) {
			
			newAvi.write(&Chunk[j].FOURCC[0], 4);
			newAvi.write((char*)&Chunk[j].ckSize, 4);

			newAvi.write(&Chunk[j].ckData[0], Chunk[j].ckSize);
			size_t filesize = newAvi.tellp();
			if (Chunk[j].ckSize % 2) {
				Chunk[j].ckSize += 1;
				newAvi.write(padd, 1);
			}
			
		}

		filesize = newAvi.tellp();
		filesize -= 8;
		newAvi.seekg(4);
		newAvi.write((char*)&filesize, 4);
		newAvi.close();
	}


	Chunk.clear();

	return ;
}
