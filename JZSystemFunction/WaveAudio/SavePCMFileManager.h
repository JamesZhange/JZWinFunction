#pragma once
#include <afxwin.h>
#include <afxcmn.h>
#include "..\JZGlobal.h"
#include "WaveHeader.h"




class SavePCMFileManager
{
public:
	SavePCMFileManager();
	SavePCMFileManager(CString filepath, int willSaveWaveHeader=YES);
	~SavePCMFileManager();

	// --- 属性 ----
	FILE* pPCMFile;

	int saveWaveHeader;

	GETPROP(int, isAvailable)
	GET(int, isAvailable) {
		return _isAvailable;
	}

	GETPROP(int, savePcmDataLen)
	GET(int, savePcmDataLen) {
		return _savePcmDataLen;
	}


	/************************************/
	/*       输出 PCM/Wav 文件           */
	/************************************/

	int openFileWithPath(CString filepath, int willSaveWaveHeader=YES);
	int closeCurrentFile();
	
	int savePcmData(char* pPcmBuff, unsigned int nPcmLen);

	static void writeWavHeaderInFile(FILE * fp, int channels, int samplesPerSec, int bitsPerSample, int pcmDataSize);
	


	

private:
	int _isAvailable;
	int _savePcmDataLen;
};




