// canTestMFCDlg.h : header file
//

#if !defined(AFX_CANTESTMFCDLG_H__F34CCB14_11C6_449E_9B18_5B5AE6FA1F24__INCLUDED_)
#define AFX_CANTESTMFCDLG_H__F34CCB14_11C6_449E_9B18_5B5AE6FA1F24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCanTestMFCDlg dialog

class CCanTestMFCDlg : public CDialog
{
// Construction
public:
	CCanTestMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCanTestMFCDlg)
	enum { IDD = IDD_CANTESTMFC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCanTestMFCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	//global handle for can channel
	int m_handle;

	// Generated message map functions
	//{{AFX_MSG(CCanTestMFCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSend();
	afx_msg void OnOnBus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANTESTMFCDLG_H__F34CCB14_11C6_449E_9B18_5B5AE6FA1F24__INCLUDED_)
