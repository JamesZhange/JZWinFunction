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

	// �Ⱥ����أ������ڹ��캯�����Ⱥ��������ߵĶ�������Ѿ�������
	JZMemory & operator= (const JZMemory & m);
	bool operator== (const JZMemory & m);
	// JZMemory & operator+ (const JZMemory & m);
	// friend JZMemory & operator+ (const JZMemory & m1, const JZMemory & m2);

	int memoryCopy(void* pBuff, unsigned int buffLen);
	int memoryCopy(JZMemory & m);

	static void freeJZMemory(void* jzmallocptr);


	// ------ ���� ----------

	// �û�ʹ�����ָ����ָ���ڲ�
	GETPROP(void*, memory)
	GET(void*, memory) {
		return _pUserMemory;
	}

	// ���len���û�����ʹ�õĳ��ȣ������ڲ�ʵ��malloc�ĳ���
	GETPROP(unsigned int, memoryLen)
	GET(unsigned int, memoryLen) {
		return _userMemoryLen;
	}

	GETPROP(eumJZMAlignSize, alignType)
	GET(eumJZMAlignSize, alignType) {
		return _memoryAlignType;
	}



private:
	void * _pUserMemory; // �û�ָ��
	unsigned int _userMemoryLen; // �û��ڴ泤��
	eumJZMAlignSize _memoryAlignType;

	void *_allocAddr; // ʵ������ָ��

	int mallocMemory(unsigned int size, eumJZMAlignSize align);
};

