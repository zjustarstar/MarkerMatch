
// MarkerMatchUI.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMarkerMatchUIApp: 
// �йش����ʵ�֣������ MarkerMatchUI.cpp
//

class CMarkerMatchUIApp : public CWinApp
{
public:
	CMarkerMatchUIApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMarkerMatchUIApp theApp;