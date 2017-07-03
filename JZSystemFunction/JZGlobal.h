#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "JZMathDefine.h"

// 平台指针
typedef int		PlatformPtrType;


#define YES		(1)
#define NO		(0)


// C++ 属性宏
/************************************************************************
{
public:
// 只读属性
GETPROP(int, variate)
GET(int, variate) {
return _variate;
}

// 读写属性
PROP(int, variate2)
GET(int, variate2) {
return _variate2;
}
SET(int, variate2) {
... // do some thing
_variate2 = variate2;
}


private:
int _variate;
int _variate2;
}
***********************************************************************/
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

