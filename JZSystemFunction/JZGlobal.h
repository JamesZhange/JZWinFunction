#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// ƽָ̨��
typedef int		PlatformPtrType;

#define YES		(1)
#define NO		(0)


// C++ ���Ժ�
#define PROP(T,X) __declspec(property(get= __get##X,put= __put##X))T X;
#define GETPROP(T,X) __declspec(property(get= __get##X))T X; //ֻ������
#define SETPROP(T,X) __declspec(property(put= __put##X))T X; //ֻд����
#define GET(T,X) T __get##X(void) 
#define SET(T,X) void __put##X(T value)




class JZGroble
{
public:
	JZGroble();
	~JZGroble();
};

