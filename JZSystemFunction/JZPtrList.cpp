#include "JZPtrList.h"


JZPtrList::JZPtrList(void)
{
	m_Size = 0;
	for(int i=0; i<MAX_PTR_LEN; i++)
	{
		m_Element[i].ptr = 0;
	}
	m_new = 0;
}

JZPtrList::~JZPtrList(void)
{
	//assert(!m_Size);
}

void* JZPtrList::GetAt(int pos)
{
	if(pos < m_Size)
		return m_Element[pos].ptr;
	else
		return 0;
}

//APOSITION begin from the index 1
APOSITION JZPtrList::GetHeadPosition()
{
	JZScopeLock cs(&m_cs);
	if(m_Size)
		return 1;
	else
		return 0;
}

void* JZPtrList::GetNext(APOSITION& pos)
{
	JZScopeLock cs(&m_cs);
	if(pos && pos <= m_Size)
	{
		void* ptr = m_Element[pos-1].ptr;			
		pos++;
		if(pos > m_Size)
			pos = 0;
		return ptr;
	}

	pos = 0;
	return 0;
}

void JZPtrList::AddTail(void* ptr)
{
	JZScopeLock cs(&m_cs);
	//assert(m_Size < MAX_PTR_LEN);
	m_Element[m_Size].ptr = ptr;
	m_Size++;
	m_new=1;
}

void JZPtrList::AddHead(void* ptr)
{
	JZScopeLock cs(&m_cs);
	//assert(m_Size < MAX_PTR_LEN);
	for(int i=m_Size; i>0; i--)
	{
		m_Element[i].ptr = m_Element[i-1].ptr;
	}
	m_Element[0].ptr = ptr;
	m_Size++;
	m_new=1;
}

void* JZPtrList::RemoveAt(APOSITION pos)
{
	JZScopeLock cs(&m_cs);
	void* ptr = 0;
	if(pos && m_Size)
	{
		ptr = m_Element[pos-1].ptr;
		for(int i=pos-1; i<m_Size-1; i++)
		{
			m_Element[i].ptr = m_Element[i+1].ptr;
		}	
		m_Element[m_Size-1].ptr = 0;
		m_Size--;
	}
	return ptr;
}

void* JZPtrList::RemoveTail()
{
	JZScopeLock cs(&m_cs);
	void* ptr = 0;
	if(m_Size)
	{
		ptr = m_Element[m_Size-1].ptr;
		m_Element[m_Size-1].ptr = 0;
		m_Size--;
	}
	return ptr;
}

void* JZPtrList::RemoveHead()
{
	JZScopeLock cs(&m_cs);
	void* ptr = 0;
	if(m_Size)
	{
		ptr = m_Element[0].ptr;
		for(int i=0; i<m_Size-1; i++)
		{
			m_Element[i].ptr = m_Element[i+1].ptr;
		}	
		m_Element[m_Size-1].ptr = 0;
		m_Size--;
	}
	return ptr;
}

int JZPtrList::GetSize()
{
	JZScopeLock cs(&m_cs);
	return m_Size;
}
int JZPtrList::IsNew()
{
	JZScopeLock cs(&m_cs);
	return m_new;
}
void JZPtrList::ClearNew()
{
	JZScopeLock cs(&m_cs);
	m_new = 0;
}