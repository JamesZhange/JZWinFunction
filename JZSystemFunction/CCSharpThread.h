/*
*  http://blog.csdn.net/rabbit2003/article/details/20618
*/


#pragma once 
#include <Windows.h>

class CJThread;          //Ԥ����



/******************************/
/*     class IRunable         */
/******************************/

class IRunable {           //ִ�нӿ�

protected:
	virtual DWORD Run() = 0;   //�̷߳�����������ʵ��

	friend class CJThread;   //����CJThread����Run�������������������˴��ⲿ����
};







/******************************/
/*     class CJThread         */
/******************************/

class CJThread : public IRunable {          //�߳���

	IRunable *m_runobj;						//�̹߳�����ִ�ж���
	HANDLE m_hThread;						//�߳̾��

	static DWORD WINAPI ThreadBody(LPVOID param) {
		CJThread *pThis = (CJThread *)param;
		IRunable *runobj = pThis->m_runobj ? pThis->m_runobj : pThis;  //ȷ��IRunable����
		return runobj->Run();          //���߳̿���Ȩ����IRunable.Run
	}

protected:

	DWORD Run() { return 0; }   //ʵ���̷߳������Ա���ʵ����CJThread

public:
	CJThread(IRunable *runobj = NULL) :m_runobj(runobj) {};   //���췽������m_runobj��ֵ

	bool Start() {   //�����̲߳�������������Դ����飬�����߳��Ѿ�������
		m_hThread = CreateThread(NULL, 0, CJThread::ThreadBody, (LPVOID)this, 0, NULL);
		return NULL != m_hThread;
	}

	void Join() { WaitForSingleObject(m_hThread, INFINITE); }   //�ȴ��߳��˳�
};







/******************************/
/*   class CCSharpThread      */
/******************************/

template <class T>

class CCSharpThread : public CJThread {

	typedef DWORD(T::*ThreadFunc)();

	//�̷߳�������ģ��������һ��ԭ��ΪDWORD ()�ķǾ�̬��Ա����
	//�������ĸ�������������ʱָ��
	ThreadFunc ThreadBody;

	//�ṩ�̷߳����Ķ���
	T *m_TheadFuncObj;

protected:

	virtual DWORD Run() { return (m_TheadFuncObj->*ThreadBody)(); }        //�����̷߳���

public:

	CCSharpThread(T &obj, ThreadFunc _threadfunc) : m_TheadFuncObj(&obj), ThreadBody(_threadfunc) {}
};




/*******************************/
/*   CCSharpThread ʹ������     */
/*******************************/

/*********************************************

class someclass {
	int _counter;
public:
	someclass() { _counter = 0; }  // ��ʼ��

	// �̺߳���1
	DWORD Thread1() { 
		while (_counter < 3) { 
			cout << "Thread1: ���������Ƿ�3" << endl;   
			Sleep(500); 
		}
		cout << "Thread1 ��⵽�������ﵽ3���˳�" << endl;
		return 0;
	}

	// �̺߳���2
	DWORD Thread2() {
		for (; _counter < 3; _counter++) {
			cout << "Thread2: ��������1����ǰֵΪcounter = " << _counter << endl;
			Sleep(1000);
		}
		return 0;
	}
};

// ���Ժ��� // ���Ժ��������Զ����� App2 �����

void CSharpThreadTest() {

	someclass appObj;
	CCSharpThread<someclass> thread1(appObj, someclass::Thread1);
	CCSharpThread<someclass> thread2(appObj, someclass::Thread2);

	thread1.Start();  //�����߳�
	thread2.Start();  //�����߳�

	thread1.Join();  //�ȴ��߳��˳�
	thread2.Join();  //�ȴ��߳��˳�
}

*********************************************/
