#pragma once
#include <windows.h>
#include "JZScopeLock.h"

class JZThread
{
public:
	JZThread(void);
	virtual ~JZThread(void);

	void SetPriority(int priority);
	int GetPriority();

	int Start(void* pArg);
	void Stop();

protected:
	virtual DWORD Run() = 0;
	static  DWORD WINAPI ThreadProc(void* lpArg)
	{
		JZThread* p = (JZThread*) lpArg;
		return p->Run();
	}

	HANDLE m_Thread;
	int m_Running;
	JZEvent m_Event;
};




/****************************************************************/
/*                  class JZCSharpThread                        */
/*  http://blog.csdn.net/rabbit2003/article/details/20618       */
/****************************************************************/

template <class T>

class JZCSharpThread : public JZThread {

	typedef DWORD(T::*ThreadFunc)();

	//�̷߳�������ģ��������һ��ԭ��ΪDWORD ()�ķǾ�̬��Ա����
	//�������ĸ�������������ʱָ��
	ThreadFunc ThreadProc;

	//�ṩ�̷߳����Ķ���
	T *m_TheadFuncObj;

protected:

	virtual DWORD Run() { return (m_TheadFuncObj->*ThreadProc)(); }        //�����̷߳���

public:

	JZCSharpThread(T &obj, ThreadFunc _threadfunc) : m_TheadFuncObj(&obj), ThreadProc(_threadfunc) {}
};



/*******************************/
/*   CCSharpThread ʹ������     */
/*******************************/

/*********************************************

class someclass {
	int debug01 = 0;
	int debug02 = 0;
	DWORD ThreadTest01()
	{
		for (int i = 0; i < 10; i++)
		{
			printf("thread 01 run %d\n", debug01++);
			Sleep(500);
		}
		printf("--thread 01 end--\n");
		return 0;
	}

	DWORD ThreadTest02()
	{
		for (int i = 0; i < 10; i++)
		{
			printf("thread 02 run %d\n", debug02++);
			Sleep(300);
		}
		printf("--thread 02 end--\n");
		return 0;
	}
}

// ���Ժ��� // ���Ժ��������Զ����� App2 �����

void CSharpThreadTest() {

someclass appObj;
JZCSharpThread<someclass> thread1(appObj, &someclass::ThreadTest01);
JZCSharpThread<someclass> thread2(appObj, &someclass::ThreadTest02);

thread1.Start(NULL);  //�����߳�
thread2.Start(NULL);  //�����߳�

thread1.Stop();  //�ȴ��߳��˳�
thread2.Stop();  //�ȴ��߳��˳�

}

*********************************************/
