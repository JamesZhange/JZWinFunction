#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// 平台指针
typedef int		PlatformPtrType;

#define YES		(1)
#define NO		(0)


// C++ 属性宏
#define PROP(T,X) __declspec(property(get= __get##X,put= __put##X))T X;
#define GETPROP(T,X) __declspec(property(get= __get##X))T X; //只读属性
#define SETPROP(T,X) __declspec(property(put= __put##X))T X; //只写属必
#define GET(T,X) T __get##X(void) 
#define SET(T,X) void __put##X(T value)




class JZGroble
{
public:
	JZGroble();
	~JZGroble();
};

