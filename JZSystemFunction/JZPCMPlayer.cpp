#include "JZPCMPlayer.h"

// 静态变量初始化
int JZPCMPlayer::_isPlayerRunning = NO;


JZPCMPlayer::JZPCMPlayer()
{
	tag = 0;
	delegate = NULL;
}


JZPCMPlayer::~JZPCMPlayer()
{
}



int JZPCMPlayer::CreatePlayer(int channels, int samplesPerSec, int bitsPerSample)
{
	plChannels = channels;
	plSamplesPerSec = samplesPerSec;
	plBitsPerSample = bitsPerSample;
	dwRepetitions = 1;

	//open waveform audo for input     
	pyWaveform.wFormatTag = WAVE_FORMAT_PCM;
	pyWaveform.nChannels = plChannels;
	pyWaveform.nSamplesPerSec = plSamplesPerSec;
	pyWaveform.wBitsPerSample = plBitsPerSample;
	pyWaveform.nAvgBytesPerSec = (pyWaveform.nChannels * pyWaveform.nSamplesPerSec * pyWaveform.wBitsPerSample / 8);// 176400;    
	pyWaveform.nBlockAlign = (pyWaveform.nChannels *pyWaveform.wBitsPerSample / 8); // 4;
	pyWaveform.cbSize = 0;

	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &pyWaveform, (DWORD_PTR)AudioPlay_Callback, (DWORD_PTR)this, CALLBACK_FUNCTION)) //打开输出设备，开始回放
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return 1;
	}
	return 0;
}

int JZPCMPlayer::StartPlay(LPSTR pBuffer, DWORD dwDataLength)
{
	// Set up header
	PWAVEHDR WaveHdr = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
	WaveHdr->lpData = pBuffer;
	WaveHdr->dwBufferLength = dwDataLength;
	WaveHdr->dwBytesRecorded = 0;
	WaveHdr->dwUser = 0;
	WaveHdr->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	WaveHdr->dwLoops = dwRepetitions;
	WaveHdr->lpNext = NULL;
	WaveHdr->reserved = 0;

	StartPlay(WaveHdr);

	return 0;
}

int JZPCMPlayer::StartPlay(PWAVEHDR pWaveHdr)
{
	waveOutPrepareHeader(hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	dwRepetitions = 1;

	return 0;
}

int JZPCMPlayer::StopPlay()
{
	waveOutReset(hWaveOut);  //停止回放，关闭输出设备
	_isPlayerRunning = NO;
	dwRepetitions = 1;
	return 0;
}

int JZPCMPlayer::TeardownPlayer()
{
	if (NULL != hWaveOut)
	{
		waveOutClose(hWaveOut);
		hWaveOut = NULL;
	}
	return 0;
}





/******************************************************************/
/*                             回调函数                             */
/******************************************************************/

void JZPCMPlayer::AudioPlay_Callback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	JZPCMPlayer* player = (JZPCMPlayer*)dwInstance;

	switch (uMsg)
	{
	case WOM_OPEN:
	{
		_isPlayerRunning = 1;
		if (NULL != player->delegate)
		{
			player->delegate->OnJZPCMPlayerOpen(player);
		}
	}
	break;
	
	case WOM_DONE:
	{
		PWAVEHDR pWaveHdr = (PWAVEHDR)dwParam1;

		waveOutUnprepareHeader(hWaveOut, pWaveHdr, sizeof(WAVEHDR));

		// 播放完自己释放了播放buffer的内存
		destroyWaveHearder(pWaveHdr);

		if (NULL != player->delegate)
		{
			player->delegate->OnJZPCMPlayerPlayDone(player);
		}
	}
		break;

	case WOM_CLOSE:  // bug: hWaveOut() 没有触发这个消息
	{
		if (NULL != player->delegate)
		{
			player->delegate->OnJZPCMPlayerClose(player);
		}

	}
		break;

	default:
		break;

	}
}



PWAVEHDR JZPCMPlayer::CopyWaveHearder(PWAVEHDR orgWavHeader)
{
	PWAVEHDR retHeader = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));

	retHeader->lpData = (LPSTR)malloc(orgWavHeader->dwBufferLength);
	memcpy(retHeader->lpData, orgWavHeader->lpData, orgWavHeader->dwBufferLength);
	retHeader->dwBufferLength = orgWavHeader->dwBufferLength;
	retHeader->dwBytesRecorded = orgWavHeader->dwBytesRecorded;
	retHeader->dwUser = orgWavHeader->dwUser;
	retHeader->dwFlags = orgWavHeader->dwFlags;
	retHeader->dwLoops = orgWavHeader->dwLoops;
	retHeader->lpNext = orgWavHeader->lpNext;
	retHeader->reserved = orgWavHeader->reserved;


	return retHeader;
}

void JZPCMPlayer::destroyWaveHearder(PWAVEHDR wavHeader)
{
	if (NULL != wavHeader)
	{
		if (NULL != wavHeader->lpData)
		{
			free(wavHeader->lpData);
			wavHeader->lpData = NULL;
		}
		free(wavHeader);
	}
}

