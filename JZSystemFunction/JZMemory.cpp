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
		_pUserMemory = NULL; // 用户指针
		_userMemoryLen = 0; // 用户内存长度
		_allocAddr = 0; // 实际申请指针
		_memoryAlignType = AlignSize_Random;
		return -3;
	}
	else
	{
		_pUserMemory = NULL; // 用户指针
		_userMemoryLen = 0; // 用户内存长度

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
			_pUserMemory = NULL; // 用户指针
			_userMemoryLen = 0; // 用户内存长度
			_allocAddr = 0; // 实际申请指针
			_memoryAlignType = AlignSize_Random;
			return -2;
		}
#else
		// debug 我现在想在用户使用的内存前放一个指针，为方便起见全部用8字节对齐的内存
		unsigned int totallen = size + 2*(1 << AlignSize_8byte); // 一个8字节为对齐裁剪用，一个8字节存放this指针
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
			_pUserMemory = NULL; // 用户指针
			_userMemoryLen = 0; // 用户内存长度
			_allocAddr = 0; // 实际申请指针
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

// 实际使用时，经常是只有指针记录所需的内存指针，而不记录JZMemory类的，
// 在内里用该内存前的指针记录JZMemory类实例指针
// 所以如果入参不是JZMemory申请的空间，这个函数就会crash
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



