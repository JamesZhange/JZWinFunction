#pragma once

#include <MMSystem.h>
#include "JZGlobal.h"


#define JZAUDIORECORDERBUFFERNUM		10		// 录音缓冲buffer个数


class JZPCMRecorder
{
public:
	JZPCMRecorder();
	~JZPCMRecorder();

public:
	int CreateRecorder(unsigned int channels,
		unsigned int samplesPerSec, 
		unsigned int bitsPerSample,
		unsigned int cacheBufferNum = JZAUDIORECORDERBUFFERNUM);
	int StartRecorder();
	int StopRecorder();
	int TeardownRecorder();

	GETPROP(int, isRecorderRunning)
	GET(int, isRecorderRunning) {
		return _isRecorderRunning;
	}

	GETPROP(int, RecordeChunksCount)
	GET(int, RecordeChunksCount) {
		return _RecordeChunksCount;
	}

private:

	// 录音
	int rcChannels;
	int rcSamplesPerSec;
	int rcBitsPerSample;

	bool m_record;	// m_record表示是否正在录音


	WAVEFORMATEX waveform;    //WAV文件头包含音频格式
	DWORD dwDataLength, dwRepetitions; //dwDataLength已有的数据长度，dwRepetitions重复次数
	HWAVEIN hWaveIn;     //输入设备句柄

	unsigned int _CacheBufferNum;
	PWAVEHDR * _WaveHdrArray;//声音文件头
	PBYTE * _RecordeBufferArray;  // 录音输入缓冲区

	static int _isRecorderRunning;
	static int _RecordeChunksCount;

	//---
	// WaveXAPI回调函数
	static DWORD(CALLBACK AudioRecorder_Callback)(	
		HWAVEIN hwavein,							//	.输入设备
		UINT uMsg,									//	.消息
		DWORD dwInstance,							//	.保留
		DWORD dwParam1,								//	.刚填充好的缓冲区指针
		DWORD dwParam2);							//	.保留
};

