// canTestMFC.h : main header file for the CANTESTMFC application
//

#if !defined(AFX_CANTESTMFC_H__587F7F4E_4B35_41AD_8163_5D4E5434CDE9__INCLUDED_)
#define AFX_CANTESTMFC_H__587F7F4E_4B35_41AD_8163_5D4E5434CDE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCApp:
// See canTestMFC.cpp for the implementation of this class
//

class CCanTestMFCApp : public CWinApp
{
public:
	CCanTestMFCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCanTestMFCApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCanTestMFCApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANTESTMFC_H__587F7F4E_4B35_41AD_8163_5D4E5434CDE9__INCLUDED_)
