#include "JZPCMPlayer.h"

// ��̬������ʼ��
int JZPCMPlayer::_isPlayerRunning = NO;


JZPCMPlayer::JZPCMPlayer()
{
	tag = 0;
	delegate = NULL;
}


JZPCMPlayer::~JZPCMPlayer()
{
}




int JZPCMPlayer::waveOutputDeviceCapacity(int& devNum, WAVEOUTCAPS** pWavOutDevCaqs)
{
	devNum = 0;
	*pWavOutDevCaqs = NULL;
	devNum = waveOutGetNumDevs();
	if (devNum > 0)
	{
		*pWavOutDevCaqs = (WAVEOUTCAPS*)malloc(devNum * sizeof(WAVEOUTCAPS));
		for (int i = 0; i < devNum; i++)
		{
			MMRESULT mmResult = waveOutGetDevCaps(i, ((WAVEOUTCAPS*)(*pWavOutDevCaqs) + i), sizeof(WAVEOUTCAPS));
		}

		return 0;
	}

	return 1;
}



int JZPCMPlayer::CreatePlayer(int channels, int samplesPerSec, int bitsPerSample, int outputDevID)
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

	if (waveOutOpen(&hWaveOut, outputDevID, &pyWaveform, (DWORD_PTR)AudioPlay_Callback, (DWORD_PTR)this, CALLBACK_FUNCTION)) //������豸����ʼ�ط�
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return 1;
	}
	return 0;
}

int JZPCMPlayer::StartPlay(LPSTR pBuffer, DWORD dwDataLength, float volumeRate)
{
	if ((NULL != pBuffer) && (dwDataLength > 0))
	{
		// Set up header
		PWAVEHDR WaveHdr = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR)));
		WaveHdr->lpData = (LPSTR)malloc(dwDataLength);
		if (NULL != WaveHdr->lpData)
		{
			memcpy(WaveHdr->lpData, pBuffer, dwDataLength);
			WaveHdr->dwBufferLength = dwDataLength;
			WaveHdr->dwBytesRecorded = 0;
			WaveHdr->dwUser = 0;
			WaveHdr->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
			WaveHdr->dwLoops = 1;
			WaveHdr->lpNext = NULL;
			WaveHdr->reserved = 0;

			// ��������
			adjustVolume((char*)WaveHdr->lpData, dwDataLength, volumeRate);

			StartPlay(WaveHdr);

			return 0;
		}
		return -3;
	}
	return -1;
}

int JZPCMPlayer::StartPlay(PWAVEHDR pWaveHdr)
{
	_isPlayerRunning = YES;
	waveOutPrepareHeader(hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, pWaveHdr, sizeof(WAVEHDR));
	dwRepetitions = 1;

	return 0;
}

int JZPCMPlayer::StopPlay()
{
	_isPlayerRunning = NO;
	waveOutReset(hWaveOut);  //ֹͣ�طţ��ر�����豸
	
	dwRepetitions = 1;
	return 0;
}

int JZPCMPlayer::TeardownPlayer()
{
	if (NULL != hWaveOut)
	{
		_isPlayerRunning = NO;
		waveOutClose(hWaveOut);
		hWaveOut = NULL;
	}
	return 0;
}





/******************************************************************/
/*                             �ص�����                             */
/******************************************************************/

void JZPCMPlayer::AudioPlay_Callback(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	JZPCMPlayer* player = (JZPCMPlayer*)dwInstance;

	switch (uMsg)
	{
	case WOM_OPEN:
	{
		if (NULL != player->delegate)
		{
			player->delegate->OnJZPCMPlayerOpen(player);
		}
	}
	break;
	
	case WOM_DONE:
	{
		PWAVEHDR pWaveHdr = (PWAVEHDR)dwParam1;
		if (YES == _isPlayerRunning)
		{
			waveOutUnprepareHeader(hWaveOut, pWaveHdr, sizeof(WAVEHDR));
			// �������Լ��ͷ��˲���buffer���ڴ�
			destroyWaveHearder(pWaveHdr);

			if (NULL != player->delegate)
			{
				player->delegate->OnJZPCMPlayerPlayDone(player);
			}
		}
		else
		{
			destroyWaveHearder(pWaveHdr);
		}
		
	}
		break;

	case WOM_CLOSE:  // bug: hWaveOut() û�д��������Ϣ
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


//----- volume --- 

void JZPCMPlayer::adjustVolume(char* pPcmData, int nDataLength, float volumeRate)
{
	if (volumeRate >= 1.0)
	{
	}
	else if (volumeRate <= 0)
	{
		memset(pPcmData, 0, nDataLength);
	}
	else
	{
		// printf("change volume: %f for datalen: %d\n", volumeRate, nDataLength);
		if (8 == plBitsPerSample)
		{
			adjust8BitSampleForVolume((char*)pPcmData, nDataLength, volumeRate);
		}
		else if (16 == plBitsPerSample)
		{
			adjust16BitSampleForVolume((char*)pPcmData, nDataLength, volumeRate);
		}
	}

}


void JZPCMPlayer::adjust8BitSampleForVolume(char* pcmDataBuff, int pcmDataLen, float volumeRate)
{
	if (((NULL == pcmDataBuff) || (pcmDataLen <= 0))
		|| (volumeRate <= 0) || (volumeRate >= 1.0))
	{
		return;
	}
	char* pPcmBuff = pcmDataBuff;
	for (int i=0; i<pcmDataLen; i++)
	{
		// ��volumeRate ���� 1ʱ����ʵ���������Ͳ����ģ�������ʱûд��һ����
		*pPcmBuff = (*pPcmBuff) * volumeRate;
		pPcmBuff++;
	}
}

void JZPCMPlayer::adjust16BitSampleForVolume(char* pcmDataBuff, int pcmDataLen, float volumeRate)
{
	if (((NULL == pcmDataBuff) || (pcmDataLen <= 1))
		|| (volumeRate <= 0) || (volumeRate >= 1.0))
	{
		return;
	}
	short* pPcmBuff = (short*)pcmDataBuff;
	for (int i = 0; i < (pcmDataLen)/2; i++)
	{
		// ��volumeRate ���� 1ʱ����ʵ���������Ͳ����ģ�������ʱûд��һ����
		*pPcmBuff = (*pPcmBuff) * volumeRate;
		pPcmBuff++;
	}
}

