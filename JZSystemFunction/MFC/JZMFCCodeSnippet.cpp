
#include "JZMFCCodeSnippet.h"








// ��ȡ�ļ���׺
CString GetExtName(CString fileName) {
	int pos = fileName.Find(_T(".")); //��ȡ . ��λ��
	if (pos == -1) { //���û���ҵ���ֱ�ӷ��ظ��ַ���
		return fileName;
	}
	else {
		return GetExtName(fileName.Mid(pos + 1)); //�ҵ��˵Ļ�������������ֱ����ײ�
	}
}







