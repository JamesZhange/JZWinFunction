#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "JZGlobal.h"

class JZMemory
{
public:
	typedef enum {
		AlignSize_Random = 0,
		AlignSize_2byte = 1,
		AlignSize_4byte = 2,
		AlignSize_8byte = 3,
		// AlignSize_16byte = 4,
	} eumJZMAlignSize;

	JZMemory(unsigned int size, eumJZMAlignSize align = AlignSize_Random);
	JZMemory(const JZMemory & m);
	~JZMemory();

	// 等号重载，不属于构造函数，等号左右两边的对象必须已经被创建
	JZMemory & operator= (const JZMemory & m);
	bool operator== (const JZMemory & m);
	// JZMemory & operator+ (const JZMemory & m);
	// friend JZMemory & operator+ (const JZMemory & m1, const JZMemory & m2);

	int memoryCopy(void* pBuff, unsigned int buffLen);
	int memoryCopy(JZMemory & m);

	static void freeJZMemory(void* jzmallocptr);


	// ------ 属性 ----------

	// 用户使用这个指针所指向内侧
	GETPROP(void*, memory)
	GET(void*, memory) {
		return _pUserMemory;
	}

	// 这个len是用户申请使用的长度，不是内部实际malloc的长度
	GETPROP(unsigned int, memoryLen)
	GET(unsigned int, memoryLen) {
		return _userMemoryLen;
	}

	GETPROP(eumJZMAlignSize, alignType)
	GET(eumJZMAlignSize, alignType) {
		return _memoryAlignType;
	}



private:
	void * _pUserMemory; // 用户指针
	unsigned int _userMemoryLen; // 用户内存长度
	eumJZMAlignSize _memoryAlignType;

	void *_allocAddr; // 实际申请指针

	int mallocMemory(unsigned int size, eumJZMAlignSize align);
};

