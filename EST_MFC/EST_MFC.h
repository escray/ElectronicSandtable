
// EST_MFC.h : main header file for the EST_MFC application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CEST_MFCApp:
// See EST_MFC.cpp for the implementation of this class
//

class CEST_MFCApp : public CWinApp
{
public:
	CEST_MFCApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEST_MFCApp theApp;
