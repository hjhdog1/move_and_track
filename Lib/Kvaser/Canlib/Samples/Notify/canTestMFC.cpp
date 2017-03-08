// canTestMFC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "canTestMFC.h"
#include "canTestMFCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCApp

BEGIN_MESSAGE_MAP(CCanTestMFCApp, CWinApp)
	//{{AFX_MSG_MAP(CCanTestMFCApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCApp construction

CCanTestMFCApp::CCanTestMFCApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCanTestMFCApp object

CCanTestMFCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCApp initialization

BOOL CCanTestMFCApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

	CCanTestMFCDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
