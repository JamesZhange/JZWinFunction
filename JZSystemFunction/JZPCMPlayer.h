#pragma once

#include <MMSystem.h>
#include "JZGlobal.h"


class JZPCMPlayer
{
public:
	JZPCMPlayer();
	~JZPCMPlayer();

	int CreatePlayer(int channels, int samplesPerSec, int bitsPerSample);
	int StartPlay(LPSTR pBuffer, DWORD dwDataLength);
	int StartPlay(PWAVEHDR pWaveHdr);
	int StopPlay();
	int TeardownPlayer();

	GETPROP(int, isPlayerRunning)
	GET(int, isPlayerRunning) {
		return _isPlayerRunning;
	}


private:
	int plChannels;
	int plSamplesPerSec;
	int plBitsPerSample;

	WAVEFORMATEX pyWaveform;    //WAV�ļ�ͷ������Ƶ��ʽ

	HWAVEOUT hWaveOut;     //����豸���

	DWORD dwRepetitions; //�ظ�����

	static int _isPlayerRunning; // ��ʾ�Ƿ����ڻط�

	static void(CALLBACK AudioPlay_Callback)(
		HWAVEOUT m_hWO, 
		UINT uMsg, 
		DWORD dwInstance,
		DWORD dwParam1,
		DWORD dwParam2);

	static PWAVEHDR JZPCMPlayer::CopyWaveHearder(PWAVEHDR orgWavHeader);
	static void JZPCMPlayer::destroyWaveHearder(PWAVEHDR wavHeader);
};

