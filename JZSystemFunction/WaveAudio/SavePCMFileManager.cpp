#include "SavePCMFileManager.h"




SavePCMFileManager::SavePCMFileManager()
{
	pPCMFile = NULL;
	_isAvailable = NO;
	saveWaveHeader = NO;
	_savePcmDataLen = 0;
}

SavePCMFileManager::SavePCMFileManager(CString filepath, int willSaveWaveHeader)
{
	pPCMFile = NULL;
	int ret = openFileWithPath(filepath, willSaveWaveHeader);

}


SavePCMFileManager::~SavePCMFileManager()
{
	closeCurrentFile();
}




int SavePCMFileManager::openFileWithPath(CString filepath, int willSaveWaveHeader)
{
	closeCurrentFile();

	pPCMFile = _wfopen(filepath.GetBuffer(), _T("wb"));
	if (NULL != pPCMFile)
	{
		if (YES == willSaveWaveHeader)
		{
			WAVEDEC_HEADER emptHeader = { 0 };
			fwrite(&emptHeader, 1, sizeof(WAVEDEC_HEADER), pPCMFile);
		}

		_savePcmDataLen = 0;
		_isAvailable = YES;
		return 0;
	}
	return -3;
}

int SavePCMFileManager::closeCurrentFile()
{
	if (NULL != pPCMFile)
	{
		fflush(pPCMFile);
		fclose(pPCMFile);
		pPCMFile = NULL;
	}
	_isAvailable = NO;
	return 0;
}

int SavePCMFileManager::savePcmData(char * pPcmBuff, unsigned int nPcmLen)
{
	if ((NULL != pPCMFile) && (NULL != pPcmBuff) && (nPcmLen > 0))
	{
		fwrite(pPcmBuff, 1, nPcmLen, pPCMFile);
		fflush(pPCMFile);
		_savePcmDataLen += nPcmLen;
	}
	return 0;
}


// 生成 Wav 头文件
void SavePCMFileManager::writeWavHeaderInFile(FILE* fp,
	int channels,
	int samplesPerSec,
	int bitsPerSample,
	int pcmDataSize)
{
	WAVEDEC_HEADER waveheader = { 0 };
	waveheader.riff_header.szRiffID[0] = 'R';
	waveheader.riff_header.szRiffID[1] = 'I';
	waveheader.riff_header.szRiffID[2] = 'F';
	waveheader.riff_header.szRiffID[3] = 'F';

	waveheader.riff_header.dwRiffSize = (sizeof(waveheader) - 8) + pcmDataSize;

	waveheader.riff_header.szRiffFormat[0] = 'W';
	waveheader.riff_header.szRiffFormat[1] = 'A';
	waveheader.riff_header.szRiffFormat[2] = 'V';
	waveheader.riff_header.szRiffFormat[3] = 'E';

	//
	waveheader.fmt_block.szFmtID[0] = 'f';
	waveheader.fmt_block.szFmtID[1] = 'm';
	waveheader.fmt_block.szFmtID[2] = 't';
	waveheader.fmt_block.szFmtID[3] = ' ';

	waveheader.fmt_block.dwFmtSize = sizeof(WAVE_FORMAT);

	waveheader.fmt_block.wavFormat.wFormatTag = 1;         //音频数据编码方式 1PCM  
	waveheader.fmt_block.wavFormat.wChannels = channels;           //声道数 1,2  
	waveheader.fmt_block.wavFormat.dwSamplesPerSec = samplesPerSec;    //采样率
	waveheader.fmt_block.wavFormat.wBitsPerSample = bitsPerSample;     //样本数据位数  
	waveheader.fmt_block.wavFormat.dwAvgBytesPerSec = (channels * samplesPerSec * bitsPerSample / 8);;   //波形数据传输速率（每秒平均字节数）  
	waveheader.fmt_block.wavFormat.wBlockAlign = (channels * bitsPerSample / 8);        //数据的调整数（按字节计算）  
	
// 	waveheader.fact_block.szFactID[0] = 'f';
// 	waveheader.fact_block.szFactID[1] = 'a';
// 	waveheader.fact_block.szFactID[2] = 'c';
// 	waveheader.fact_block.szFactID[3] = 't';
// 	waveheader.fact_block.dwFactSize = 4;

	waveheader.data_block.szDataID[0] = 'd';
	waveheader.data_block.szDataID[1] = 'a';
	waveheader.data_block.szDataID[2] = 't';
	waveheader.data_block.szDataID[3] = 'a';

	waveheader.data_block.dwDataSize = pcmDataSize;

	if (NULL != fp)
	{
		fseek(fp, 0, SEEK_SET);
		fwrite(&waveheader, sizeof(waveheader), 1, fp);
		fflush(fp);
	}
}


