#pragma once
#include "JZScopeLock.h"

typedef struct _ANODE
{
	void* ptr;
}ANODE;
typedef long APOSITION;

#define MAX_PTR_LEN 64
class JZPtrList
{
public:
	JZPtrList(void);
	~JZPtrList(void);

	APOSITION GetHeadPosition();
	void* GetNext(APOSITION& pos);
	void* GetAt(int pos);

	void AddTail(void* ptr);
	void AddHead(void* ptr);

	void* RemoveAt(APOSITION pos);

	void* RemoveTail();
	void* RemoveHead();

	int GetSize();
	int IsNew();
	void ClearNew();
private:
	ANODE m_Element[MAX_PTR_LEN];
	int m_Size;
	int m_new;
	JZLock m_cs;
};
