#include "stdafx.h"
#include "JZMFCCodeSnippet.h"








// 获取文件后缀
CString GetExtName(CString fileName) {
	int pos = fileName.Find(_T(".")); //获取 . 的位置
	if (pos == -1) { //如果没有找到，直接返回该字符串
		return fileName;
	}
	else {
		return GetExtName(fileName.Mid(pos + 1)); //找到了的话，往深层遍历，直到最底层
	}
}







