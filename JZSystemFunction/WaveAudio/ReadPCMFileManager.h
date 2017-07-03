#pragma once
#include <afxwin.h>
#include <afxcmn.h>
#include "..\JZGlobal.h"
#include "WaveHeader.h"

/*
typedef enum _enumpcmbitpersample
{
	BitPerSample_8bit = 1,
	BitPerSample_16bit = 2,

}eumPCMBitPerSample;

typedef enum _enumpcmchannel
{
	PCMChannel_1Channel = 1,
	PCMChannel_2Channel = 2,

}eumPCMChannel;

typedef enum _enumpcmsamplerate
{
	PCMSamplerate_8000 = 8000,
	PCMSamplerate_16000 = 16000,
	PCMSamplerate_32000 = 32000,
	PCMSamplerate_44100 = 44100,
	PCMSamplerate_48000 = 48000,
	PCMSamplerate_96000 = 96000,
	PCMSamplerate_192000 = 192000

}eumPCMSampleRate;
*/


// ����
typedef struct _ancplayersourcepcm
{
	char* pPCMBuffer;					// pcm����buffer
	unsigned int nBufferLen;			// pcm buffer����
	unsigned int nDataLen;				// pcm ���ݳ���

// 	eumPCMBitPerSample ePCMBit;			// pcm ����������bit
// 	eumPCMChannel ePCMChannel;			// pcm ������
// 	eumPCMSampleRate ePCMSamplerate;	// ������

	int nPCMBitPerSample;			// pcm ����������bit
	int nPCMChannels;			// pcm ������
	int nPCMSamplerate;	// ������

}strcPCMData;




class ReadPCMFileManager
{
public:
	ReadPCMFileManager();
	ReadPCMFileManager(CString filepath);
	~ReadPCMFileManager();

	FILE* pPCMFile;
	// strcPCMData pcmSource;

	// ����
	GETPROP(int, isAvailable)
	GET(int, isAvailable) {
		return _isAvailable;
	}

	GETPROP(int, isPcmHaveHeader)
	GET(int, isPcmHaveHeader) {
		return _isPcmHaveHeader;
	}

	GETPROP(CString, currentFileFullPath)
	GET(CString, currentFileFullPath) {
		return _currentFileFullPath;
	}

	GETPROP(CString, currentFilePath)
		GET(CString, currentFilePath) {
		return _currentFilePath;
	}

	GETPROP(CString, currentFileName)
		GET(CString, currentFileName) {
		return _currentFileName;
	}

	
	/************************************/
	/*       ���� PCM/Wav �ļ�           */
	/************************************/
	int openFileWithPath(CString filepath);
	int closeCurrentFile();
	int getWaveHeaderOffset();

	int loadNextPCMDataLen(unsigned int needLen, strcPCMData& outData, int loopload);
	int resetFilePosition();

	int getWaveFormat(int& isFileHaveHeader, WAVE_FORMAT& fileFormat);
	
	static int getWaveHeader(char *input_buf, int inputlen, WAVEDEC_HEADER* wave_header, int *headerlen);
	static int getWaveHeaderLen(char *input_buf, int inputlen, int *headerlen);


	

private:
	int _isAvailable;
	int WaveHeaderOffset;
	
	int _isPcmHaveHeader;
	WAVE_FORMAT _FileFormat;

	CString _currentFileFullPath;
	CString _currentFilePath;
	CString _currentFileName;

	void saveCurrentPath(CString filepath);
	void cleanCurrentPath();
};




