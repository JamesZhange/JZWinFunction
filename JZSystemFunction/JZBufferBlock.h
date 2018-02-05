#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>





template <class T>
class JZBufferBlock
{
public:
	JZBufferBlock();
	~JZBufferBlock();

public:

	// �Ƿ��ʼ��
	GETPROP(unsigned int, isBufferInited)
		GET(unsigned int, isBufferInited) {
		return mIsBufferInited;
	}

	// T�����ֳ�
	GETPROP(unsigned int, bytePerSample)
		GET(unsigned int, bytePerSample) {
		return mBytePerSample;
	}

	// buffer�ܳ�����λΪ���������Ϊ��
	GETPROP(unsigned int, bufferSampleLen)
		GET(unsigned int, bufferSampleLen) {
		return mBufferSampleLen;
	}

	// buffer��Ч���ݳ���
	GETPROP(unsigned int, dataSampleLen)
		GET(unsigned int, dataSampleLen) {
		return mDataSampleLen;
	}

	// bufferָ��
	GETPROP(T*, pBuffer)
		GET(T*, pBuffer) {
		return mpBuffer;
	}

	int InitBufferWithSampleLen(unsigned int samplelen);
	int TeardownBuffer();

	int Copy(JZBufferBlock* src);

protected:
	int mIsBufferInited;

	unsigned int mBytePerSample;		// T�����ֳ�
	unsigned int mBufferSampleLen;	// buffer�ܳ�����λΪ���������Ϊ��
	unsigned int mDataSampleLen;		// buffer��Ч���ݳ���
	T* mpBuffer;
};


template <class T>
JZBufferBlock<T>::JZBufferBlock()
{
	mIsBufferInited = NO;
	mBytePerSample = sizeof(T);		// T�����ֳ�
	mBufferSampleLen = 0;	// buffer�ܳ�����λΪ���������Ϊ��
	mDataSampleLen = 0;		// buffer��Ч���ݳ���
	mpBuffer = (T*)NULL;
}


template <class T>
JZBufferBlock<T>::~JZBufferBlock()
{
	TeardownBuffer();
}

template <class T>
int JZBufferBlock<T>::InitBufferWithSampleLen(unsigned int samplelen)
{
	if (0 == samplelen)
	{
		return -2;
	}
	mpBuffer = new T[samplelen];
	if ((T*)NULL == mpBuffer)
	{
		return -3;
	}
	mBufferSampleLen = samplelen;
	mDataSampleLen = 0;
	mIsBufferInited = YES;
	return 0;
}

template <class T>
int JZBufferBlock<T>::TeardownBuffer()
{
	if (NULL != mpBuffer)
	{
		delete[] mpBuffer;
		mpBuffer = NULL;
	}
	return 0;
}

template<class T>
inline int JZBufferBlock<T>::Copy(JZBufferBlock * src)
{
	if ((NULL == src) || ((T*)NULL == src->pBuffer))
	{
		return -2;
	}
	TeardownBuffer();

	int ret = InitBufferWithSampleLen(src->bufferSampleLen);
	if (0 != ret)
	{
		return ret;
	}

	memcpy((void*)mpBuffer, (void*)(src->mpBuffer), src->dataSampleLen * mBytePerSample);
	mDataSampleLen = src->dataSampleLen;

	return 0;
}
