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

	// 是否初始化
	GETPROP(unsigned int, isBufferInited)
		GET(unsigned int, isBufferInited) {
		return mIsBufferInited;
	}

	// T类型字长
	GETPROP(unsigned int, bytePerSample)
		GET(unsigned int, bytePerSample) {
		return mBytePerSample;
	}

	// buffer总长（单位为采样点个数为）
	GETPROP(unsigned int, bufferSampleLen)
		GET(unsigned int, bufferSampleLen) {
		return mBufferSampleLen;
	}

	// buffer有效数据长度
	GETPROP(unsigned int, dataSampleLen)
		GET(unsigned int, dataSampleLen) {
		return mDataSampleLen;
	}

	// buffer指针
	GETPROP(T*, pBuffer)
		GET(T*, pBuffer) {
		return mpBuffer;
	}

	int InitBufferWithSampleLen(unsigned int samplelen);
	int TeardownBuffer();

	int Copy(JZBufferBlock* src);

protected:
	int mIsBufferInited;

	unsigned int mBytePerSample;		// T类型字长
	unsigned int mBufferSampleLen;	// buffer总长（单位为采样点个数为）
	unsigned int mDataSampleLen;		// buffer有效数据长度
	T* mpBuffer;
};


template <class T>
JZBufferBlock<T>::JZBufferBlock()
{
	mIsBufferInited = NO;
	mBytePerSample = sizeof(T);		// T类型字长
	mBufferSampleLen = 0;	// buffer总长（单位为采样点个数为）
	mDataSampleLen = 0;		// buffer有效数据长度
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
