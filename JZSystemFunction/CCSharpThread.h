/*
*  http://blog.csdn.net/rabbit2003/article/details/20618
*/


#pragma once 
#include <Windows.h>

class CJThread;          //预声明



/******************************/
/*     class IRunable         */
/******************************/

class IRunable {           //执行接口

protected:
	virtual DWORD Run() = 0;   //线程方法，由子类实现

	friend class CJThread;   //允许CJThread调用Run方法，但不允许其他人从外部调用
};







/******************************/
/*     class CJThread         */
/******************************/

class CJThread : public IRunable {          //线程类

	IRunable *m_runobj;						//线程关联的执行对象
	HANDLE m_hThread;						//线程句柄

	static DWORD WINAPI ThreadBody(LPVOID param) {
		CJThread *pThis = (CJThread *)param;
		IRunable *runobj = pThis->m_runobj ? pThis->m_runobj : pThis;  //确定IRunable对象
		return runobj->Run();          //将线程控制权交给IRunable.Run
	}

protected:

	DWORD Run() { return 0; }   //实现线程方法，以便能实例化CJThread

public:
	CJThread(IRunable *runobj = NULL) :m_runobj(runobj) {};   //构造方法，给m_runobj赋值

	bool Start() {   //创建线程并启动。这里忽略错误检查，例如线程已经启动等
		m_hThread = CreateThread(NULL, 0, CJThread::ThreadBody, (LPVOID)this, 0, NULL);
		return NULL != m_hThread;
	}

	void Join() { WaitForSingleObject(m_hThread, INFINITE); }   //等待线程退出
};







/******************************/
/*   class CCSharpThread      */
/******************************/

template <class T>

class CCSharpThread : public CJThread {

	typedef DWORD(T::*ThreadFunc)();

	//线程方法，是模板参数类的一个原型为DWORD ()的非静态成员方法
	//具体是哪个方法可在运行时指定
	ThreadFunc ThreadBody;

	//提供线程方法的对象
	T *m_TheadFuncObj;

protected:

	virtual DWORD Run() { return (m_TheadFuncObj->*ThreadBody)(); }        //重载线程方法

public:

	CCSharpThread(T &obj, ThreadFunc _threadfunc) : m_TheadFuncObj(&obj), ThreadBody(_threadfunc) {}
};




/*******************************/
/*   CCSharpThread 使用样例     */
/*******************************/

/*********************************************

class someclass {
	int _counter;
public:
	someclass() { _counter = 0; }  // 初始化

	// 线程函数1
	DWORD Thread1() { 
		while (_counter < 3) { 
			cout << "Thread1: 检查计数器是否到3" << endl;   
			Sleep(500); 
		}
		cout << "Thread1 检测到计数器达到3，退出" << endl;
		return 0;
	}

	// 线程函数2
	DWORD Thread2() {
		for (; _counter < 3; _counter++) {
			cout << "Thread2: 计数器加1，当前值为counter = " << _counter << endl;
			Sleep(1000);
		}
		return 0;
	}
};

// 测试函数 // 测试函数都可以独立于 App2 类调用

void CSharpThreadTest() {

	someclass appObj;
	CCSharpThread<someclass> thread1(appObj, someclass::Thread1);
	CCSharpThread<someclass> thread2(appObj, someclass::Thread2);

	thread1.Start();  //启动线程
	thread2.Start();  //启动线程

	thread1.Join();  //等待线程退出
	thread2.Join();  //等待线程退出
}

*********************************************/
