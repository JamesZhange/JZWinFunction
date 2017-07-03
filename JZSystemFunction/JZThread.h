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

	//线程方法，是模板参数类的一个原型为DWORD ()的非静态成员方法
	//具体是哪个方法可在运行时指定
	ThreadFunc ThreadProc;

	//提供线程方法的对象
	T *m_TheadFuncObj;

protected:

	virtual DWORD Run() { return (m_TheadFuncObj->*ThreadProc)(); }        //重载线程方法

public:

	JZCSharpThread(T &obj, ThreadFunc _threadfunc) : m_TheadFuncObj(&obj), ThreadProc(_threadfunc) {}
};



/*******************************/
/*            使用样例          */
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

// 测试函数 // 测试函数都可以独立于 App2 类调用

void CSharpThreadTest() {

someclass appObj;
JZCSharpThread<someclass> thread1(appObj, &someclass::ThreadTest01);
JZCSharpThread<someclass> thread2(appObj, &someclass::ThreadTest02);

thread1.Start(NULL);  //启动线程
thread2.Start(NULL);  //启动线程

thread1.Stop();  //等待线程退出
thread2.Stop();  //等待线程退出

}

*********************************************/
