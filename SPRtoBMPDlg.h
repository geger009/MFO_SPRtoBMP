
// SPRtoBMPDlg.h : header file
//

#pragma once


// CSPRtoBMPDlg dialog
class CSPRtoBMPDlg : public CDialogEx
{
// Construction
public:
	CSPRtoBMPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPRTOBMP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CString		strFileName;
	wchar_t*	m_szPath;
	void		Init();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonInput();
	afx_msg void OnBnClickedButtonOutput();
	afx_msg void OnBnClickedOk();
};
