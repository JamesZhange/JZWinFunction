#include "stdafx.h"
#include "JZGlobal.h"
#include "JZMemory.h"


int JZMemory:: mallocMemory(unsigned int size, eumJZMAlignSize align)
{
	if ((size <= 0) || ((align != AlignSize_Random)
		&& (align != AlignSize_2byte)
		&& (align != AlignSize_4byte)
		&& (align != AlignSize_8byte)
		// && (align != AlignSize_16byte)
		))
	{
		_pUserMemory = NULL; // �û�ָ��
		_userMemoryLen = 0; // �û��ڴ泤��
		_allocAddr = 0; // ʵ������ָ��
		_memoryAlignType = AlignSize_Random;
		return -3;
	}
	else
	{
		_pUserMemory = NULL; // �û�ָ��
		_userMemoryLen = 0; // �û��ڴ泤��

#if 0
		unsigned int totallen = size + (1 << AlignSize_8byte);
		if (AlignSize_Random != align)
		{
			totallen += (1 << align);
		}
		_allocAddr = malloc(totallen);
		if (NULL != _allocAddr) {
			unsigned int mask = ~0;
			mask <<= (align + AlignSize_8byte);
			_pUserMemory = (void *)(((unsigned int)(_allocAddr)) & mask);
			_userMemoryLen = size;
			_memoryAlignType = align;
			return 0;
		}
		else {
			_pUserMemory = NULL; // �û�ָ��
			_userMemoryLen = 0; // �û��ڴ泤��
			_allocAddr = 0; // ʵ������ָ��
			_memoryAlignType = AlignSize_Random;
			return -2;
		}
#else
		// debug �����������û�ʹ�õ��ڴ�ǰ��һ��ָ�룬Ϊ�������ȫ����8�ֽڶ�����ڴ�
		unsigned int totallen = size + 2*(1 << AlignSize_8byte); // һ��8�ֽ�Ϊ����ü��ã�һ��8�ֽڴ��thisָ��
		_allocAddr = malloc(totallen);
		if (NULL != _allocAddr) {
			unsigned int mask = ~0;
			mask <<= AlignSize_8byte;
			void* tAlignPtr = (void *)(((PlatformPtrType)(_allocAddr)) & mask);

			_pUserMemory = (((char*)tAlignPtr) + (1 << AlignSize_8byte));

			PlatformPtrType* alignPtrAddr = ((PlatformPtrType*)_pUserMemory) - 1;
			*alignPtrAddr = (PlatformPtrType)this;
			
			_userMemoryLen = size;
			_memoryAlignType = align;
			return 0;
		}
		else {
			_pUserMemory = NULL; // �û�ָ��
			_userMemoryLen = 0; // �û��ڴ泤��
			_allocAddr = 0; // ʵ������ָ��
			_memoryAlignType = AlignSize_Random;
			return -2;
		}
#endif
		
	}
}

int JZMemory::memoryCopy(void* pBuff, unsigned int buffLen) {
	if ((NULL != pBuff) && (buffLen > 0) 
		&& (NULL != _pUserMemory) && (_userMemoryLen >= buffLen))
	{
		memcpy(_pUserMemory, pBuff, buffLen);
		return 0;
	}
	return -1;
}

int JZMemory::memoryCopy(JZMemory & m) {
	void* pbuffer = ((JZMemory&)m).memory;
	unsigned int bufflen = ((JZMemory&)m).memoryLen;
	return this->memoryCopy(pbuffer, bufflen);
}

// ʵ��ʹ��ʱ��������ֻ��ָ���¼������ڴ�ָ�룬������¼JZMemory��ģ�
// �������ø��ڴ�ǰ��ָ���¼JZMemory��ʵ��ָ��
// ���������β���JZMemory����Ŀռ䣬��������ͻ�crash
void JZMemory::freeJZMemory(void* jzmallocptr)
{
	if (NULL != jzmallocptr)
	{
		JZMemory* jzmptr = ((JZMemory*)jzmallocptr) - 1;
		if (NULL != jzmptr)
		{
			delete jzmptr;
		}
	}
}


//--------------- class ----------------------

JZMemory::JZMemory(unsigned int size, eumJZMAlignSize align)
{
	mallocMemory(size, align);
}

JZMemory::JZMemory(const JZMemory & m)
{
	void* pbuffer = ((JZMemory&)m).memory;
	unsigned int size = ((JZMemory&)m).memoryLen;
	eumJZMAlignSize align = ((JZMemory&)m).alignType;
	mallocMemory(size, align);
	memoryCopy(pbuffer, size);
}


JZMemory & JZMemory::operator= (const JZMemory & m)
{
	if (*this == m)
		return *this;

	if (NULL != _allocAddr) {
		free(_allocAddr);
		_allocAddr = NULL;
		_pUserMemory = NULL;
		_userMemoryLen = 0;
	}

	void* pbuffer = ((JZMemory&)m).memory;
	unsigned int size = ((JZMemory&)m).memoryLen;
	eumJZMAlignSize align = ((JZMemory&)m).alignType;
	mallocMemory(size, align);
	memoryCopy(pbuffer, size);
	
	return *this;
}

bool JZMemory::operator== (const JZMemory & m)
{
	PlatformPtrType thisAddr = (PlatformPtrType)this;
	PlatformPtrType aimAddr = (PlatformPtrType)&m;
	if (thisAddr == aimAddr) {
		return true;
	} else {
		return false;
	}
}

JZMemory::~JZMemory()
{
	if (NULL != _allocAddr) {
		free(_allocAddr);
		_allocAddr = NULL;
		_pUserMemory = NULL;
		_userMemoryLen = 0;
	}
}



