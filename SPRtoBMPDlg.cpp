
// SPRtoBMPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SPRtoBMP.h"
#include "SPRtoBMPDlg.h"
#include "afxdialogex.h"

#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSPRtoBMPDlg dialog



CSPRtoBMPDlg::CSPRtoBMPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPRTOBMP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSPRtoBMPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSPRtoBMPDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_INPUT, &CSPRtoBMPDlg::OnBnClickedButtonInput)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &CSPRtoBMPDlg::OnBnClickedButtonOutput)
	ON_BN_CLICKED(IDOK, &CSPRtoBMPDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSPRtoBMPDlg message handlers

BOOL CSPRtoBMPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	wchar_t szFullAppPath[MAX_PATH] = { 0 };
	wchar_t szDrv[_MAX_DRIVE] = { 0 };
	wchar_t szDir[_MAX_DIR] = { 0 };
	GetModuleFileName(NULL, szFullAppPath, MAX_PATH);
	_wsplitpath_s(szFullAppPath, szDrv, _MAX_DRIVE, szDir, _MAX_DIR, NULL, 0, NULL, 0);

	m_szPath = new wchar_t[MAX_PATH];
	StringCchCopy(m_szPath, MAX_PATH, szDrv);
	StringCchCat(m_szPath, MAX_PATH, szDir);

	Init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSPRtoBMPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSPRtoBMPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSPRtoBMPDlg::Init()
{
	SetDlgItemText(IDC_EDIT_INPUT, L"...");
	SetDlgItemText(IDC_EDIT_OUTPUT, L"...");
}

void CSPRtoBMPDlg::OnBnClickedButtonInput()
{
	// TODO: Add your control notification handler code here
	CString szFilterInput = L"SPR File (*.spr)|*.spr|";

	CFileDialog dlgInput(TRUE, L".spr", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilterInput, (CSPRtoBMPDlg*)this);
	dlgInput.m_ofn.lpstrInitialDir = (LPCWSTR)m_szPath;

	if (dlgInput.DoModal() != IDOK) return;

	strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left(strFileName.ReverseFind(L'.'));

	SetDlgItemText(IDC_EDIT_INPUT, dlgInput.GetPathName());
}

void CSPRtoBMPDlg::OnBnClickedButtonOutput()
{
	// TODO: Add your control notification handler code here
	CString szFilterOutPut = L"BMP File (*.bmp)|*.bmp|";
	CFileDialog dlgOutput(FALSE, L".bmp", strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilterOutPut, (CSPRtoBMPDlg*)this);

	if (dlgOutput.DoModal() != IDOK) return;

	SetDlgItemText(IDC_EDIT_OUTPUT, dlgOutput.GetPathName());
}

void CSPRtoBMPDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString strInput;
	CString strTempOutput;
	CString strOutput;

	GetDlgItemText(IDC_EDIT_INPUT, strInput);
	GetDlgItemText(IDC_EDIT_OUTPUT, strTempOutput);

	//RGB565 mask
	DWORD dwMask[4];
	dwMask[0] = 0x0000f800;
	dwMask[1] = 0x000007e0;
	dwMask[2] = 0x0000001f;
	dwMask[3] = 0;

	FILE* fInput = NULL;
	_wfopen_s(&fInput, strInput, L"rb");

	if (!fInput)
	{
		return;
	}

	BYTE* filename = new BYTE[64];

	fread(filename, 64, 1, fInput);

	int garbageData;
	fread(&garbageData, sizeof(int), 1, fInput);

	int spritesCount;
	fread(&spritesCount, sizeof(int), 1, fInput);

	DWORD width;
	DWORD height;

	for (int i = 0; i<spritesCount; ++i)
	{
		if (i != 0)
		{
			CString strTemp;
			strTemp.Format(L"%d.bmp", i);
			strOutput = strTempOutput.Left(strTempOutput.ReverseFind('.'));
			strOutput += strTemp;
		}
		else
		{
			strOutput = strTempOutput;
		}

		DWORD wTemp;
		fread(&wTemp, sizeof(DWORD), 1, fInput);
		fread(&height, sizeof(DWORD), 1, fInput);

		if (wTemp % 2 != 0)
		{
			width = wTemp + 1;
		}
		else
		{
			width = wTemp;
		}

		int nSize = width*height;

		WORD* pBuffer = new WORD[nSize];

		for (int j = 0; j<nSize; ++j)
		{
			pBuffer[j] = 0x07ff;
		}

		if (wTemp % 2 != 0)
		{
			WORD* pTemp = new WORD[wTemp*height];
			fread(pTemp, sizeof(WORD)*wTemp*height, 1, fInput);

			int nBuffer, nTemp;
			nBuffer = 0;
			nTemp = 0;
			for (int k = 0; k<(int)height; ++k)
			{
				for (int l = 0; l<(int)wTemp; ++l)
				{
					pBuffer[l + nBuffer] = pTemp[l + nTemp];
				}
				nTemp += wTemp;
				nBuffer += wTemp + 1;
			}

			delete[] pTemp;
		}
		else
		{
			fread(pBuffer, sizeof(WORD)*nSize, 1, fInput);
		}

		HANDLE hf;
		DWORD dwTmp;
		WORD bitCount = 16;

		//bitmap file header
		BITMAPFILEHEADER hdr;
		hdr.bfType = 0x4d42;
		hdr.bfReserved1 = 0;
		hdr.bfReserved2 = 0;
		hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 4;

		//flip row order
		DWORD height2 = height - (height * 2);

		//bitmap info header
		BITMAPINFO bmpInfo = { 0 };
		bmpInfo.bmiHeader.biBitCount = bitCount;
		bmpInfo.bmiHeader.biHeight = height2;
		bmpInfo.bmiHeader.biWidth = wTemp;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biSizeImage = (width*height * 2) + sizeof(WORD);
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFO) + sizeof(DWORD) * 3;
		bmpInfo.bmiHeader.biClrUsed = 0;
		bmpInfo.bmiHeader.biCompression = BI_BITFIELDS;
		bmpInfo.bmiHeader.biXPelsPerMeter = 0x00000b12;
		bmpInfo.bmiHeader.biYPelsPerMeter = 0x00000b12;
		bmpInfo.bmiColors->rgbRed = (BYTE)dwMask[0];
		bmpInfo.bmiColors->rgbGreen = (BYTE)dwMask[1];
		bmpInfo.bmiColors->rgbBlue = (BYTE)dwMask[2];
		bmpInfo.bmiColors->rgbReserved = (BYTE)dwMask[3];

		hdr.bfSize = bmpInfo.bmiHeader.biSizeImage + 70;

		WORD wZero(0);

		//write file
		hf = CreateFile(strOutput, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(hf, &hdr, sizeof(hdr), &dwTmp, NULL);
		WriteFile(hf, &bmpInfo.bmiHeader, sizeof(bmpInfo.bmiHeader), &dwTmp, NULL);
		WriteFile(hf, &dwMask, sizeof(dwMask), &dwTmp, NULL);
		WriteFile(hf, pBuffer, width*height * 2, &dwTmp, NULL);
		WriteFile(hf, &wZero, sizeof(WORD), &dwTmp, NULL);
		CloseHandle(hf);

		delete[] pBuffer;
	}

	fclose(fInput);

	Init();
}
