// One Hertz.h : main header file for the One Hertz application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// COneHertzApp:
// See One Hertz.cpp for the implementation of this class
//

class COneHertzApp : public CWinApp
{
public:
	COneHertzApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern COneHertzApp theApp;