// canTestMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "canTestMFC.h"
#include "canTestMFCDlg.h"

#include "canlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCDlg dialog

CCanTestMFCDlg::CCanTestMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCanTestMFCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCanTestMFCDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCanTestMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCanTestMFCDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCanTestMFCDlg, CDialog)
	//{{AFX_MSG_MAP(CCanTestMFCDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnSend)
	ON_BN_CLICKED(IDC_BUTTON1, OnOnBus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCDlg message handlers

BOOL CCanTestMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCanTestMFCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CCanTestMFCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



/////////////////////////////////////////////////////////////////////


//
//called when the OnBus button is pressed 
//
void CCanTestMFCDlg::OnOnBus() 
{
	int		stat;
    int		handle;
	char	tmpStr[200];

	//init canlib
	canInitializeLibrary();

	//open channel 0
    handle = canOpenChannel(0, canOPEN_EXCLUSIVE);
    if (handle < 0) {
		sprintf(tmpStr, "ERROR: canOpenChannel() handle returned: %d", handle);
       AfxMessageBox(tmpStr);
    }

	//set up the bus
    stat = canSetBusParams(handle, canBITRATE_125K, 0, 0, 0, 0, 0);
    if (stat < 0) {
        AfxMessageBox("ERROR: canSetBusParams().");
    }
 
	//go on bus
    stat = canBusOn(handle);
    if (stat < 0) {
		AfxMessageBox("ERROR: canBusOn().");
    } 
	
	//set notification
	stat = canSetNotify(handle, this->GetSafeHwnd() , canNOTIFY_RX | canNOTIFY_ERROR | canNOTIFY_STATUS | canEVENT_TX | canNOTIFY_ENVVAR);
	
	//save the can channel handle in the global variable m_handle 
	m_handle = handle;	
}


//
//called when the Send button is pressed. 
//
void CCanTestMFCDlg::OnSend() 
{
	char	data[8];
	int		stat;

	//send a can message on channel 0
    stat = canWrite(m_handle, 1234, data, 8, canMSG_EXT);
    if (stat < 0) {
		AfxMessageBox("ERROR: canWrite().");
    }		
}


//
//WindowProc called (by the system) when there is an incomming event eg a WM__CANLIB event. 
//
LRESULT CCanTestMFCDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	char tmpStr[200];

	//check that it is a WM__CANLIB event
	if (message == WM__CANLIB) {

		//check the occurred event
		switch (lParam) {
			case 32000:
				sprintf(tmpStr, "WPARAM: %d  LPARAM: 0x%x  canEVENT_RX", wParam, lParam);
				break;
			case 32001:
				sprintf(tmpStr, "WPARAM: %d  LPARAM: 0x%x  canEVENT_TX", wParam, lParam);
				break;
			case 32002:
				sprintf(tmpStr, "WPARAM: %d  LPARAM: 0x%x  canEVENT_ERROR", wParam, lParam);
				break;
			case 32003:
				sprintf(tmpStr, "WPARAM: %d  LPARAM: 0x%x  canEVENT_STATUS", wParam, lParam);
				break;
			case canEVENT_ENVVAR:
				sprintf(tmpStr, "WPARAM: %d  LPARAM: 0x%x  canEVENT_ENVVAR", wParam, lParam);
				break;
				
			default:
				sprintf(tmpStr, "WPARAM: %d  LPARAM: 0x%x ", wParam, lParam);
				break;
			}
		AfxMessageBox(tmpStr); 
	}	

	return CDialog::WindowProc(message, wParam, lParam);
}
