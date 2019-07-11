
// MarkerMatchUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MarkerMatchUI.h"
#include "MarkerMatchUIDlg.h"
#include "afxdialogex.h"
#include "..\\MarkerMatch\MarkerFinder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif


// CMarkerMatchUIDlg 对话框

CMarkerMatchUIDlg::CMarkerMatchUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MARKERMATCHUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMarkerMatchUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FOLDERPATH, m_editFilePath);
	DDX_Control(pDX, IDC_LIST_IMAGEFILES, m_lbFiles);
	DDX_Control(pDX, IDC_STATIC_TOTALFILES, m_sttTotal);
}

BEGIN_MESSAGE_MAP(CMarkerMatchUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CMarkerMatchUIDlg::OnBnClickedButtonBrowser)
	ON_LBN_SELCHANGE(IDC_LIST_IMAGEFILES, &CMarkerMatchUIDlg::OnSelchangeListImagefiles)
	ON_BN_CLICKED(IDC_RADIO_WAFER, &CMarkerMatchUIDlg::OnBnClickedRadioWafer)
	ON_BN_CLICKED(IDC_RADIO_MASK2, &CMarkerMatchUIDlg::OnBnClickedRadioMask2)
END_MESSAGE_MAP()


// CMarkerMatchUIDlg 消息处理程序

BOOL CMarkerMatchUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CButton * pBtnDarkerEnv = (CButton *)GetDlgItem(IDC_RADIO_WAFER);
	CButton * pBtnBrightEnv = (CButton *)GetDlgItem(IDC_RADIO_MASK2);
	pBtnBrightEnv->SetCheck(1);
	m_bDarkerEnv = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMarkerMatchUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMarkerMatchUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMarkerMatchUIDlg::OnBnClickedButtonBrowser()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;
	char Buffer[MAX_PATH];

	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;//初始化制定的root目录很不容易，
	bi.pszDisplayName = Buffer;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "选择测试文件夹";
	bi.lpfn = NULL;
	bi.ulFlags = BIF_EDITBOX;//包括文件

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		//取得文件夹路径到Buffer里
		m_strFilePath.Format("%s", Buffer);
		m_editFilePath.SetWindowText(m_strFilePath);
	}

	ListAllFiles(m_strFilePath);

	int nSize = m_lbFiles.GetCount();
	CString strTile;
	strTile.Format("一共有%d个文件", nSize);
	m_sttTotal.SetWindowText(strTile);

	//将所有图像文件保存在vector中;
	m_vecFiles.clear();
	for (int i = 0; i < nSize; i++)
	{
		CString strFileName;
		m_lbFiles.GetText(i, strFileName);
		string s(strFileName.GetBuffer(0));
		m_vecFiles.push_back(s);
	}
}


void CMarkerMatchUIDlg::ListAllFiles(CString strFilePath)
{
	m_lbFiles.ResetContent();

	//获得当前文件夹下的所有图像;
	CFileFind findFile;
	CString   strDstFiles;

	strDstFiles = strFilePath + "\\*.jpg";
	BOOL bWorking = findFile.FindFile(strDstFiles);

	while (bWorking)
	{
		bWorking = findFile.FindNextFile();
		CString strFileName = findFile.GetFileName();
		m_lbFiles.AddString(strFileName);
	}
	findFile.Close();

	strDstFiles = strFilePath + "\\*.jpeg";
	bWorking = findFile.FindFile(strDstFiles);
	while (bWorking)
	{
		bWorking = findFile.FindNextFile();
		CString strFileName = findFile.GetFileName();
		m_lbFiles.AddString(strFileName);
	}
	findFile.Close();

	strDstFiles = strFilePath + "\\*.png";
	bWorking = findFile.FindFile(strDstFiles);
	while (bWorking)
	{
		bWorking = findFile.FindNextFile();
		CString strFileName = findFile.GetFileName();
		m_lbFiles.AddString(strFileName);
	}
	findFile.Close();
}

void CMarkerMatchUIDlg::OnSelchangeListImagefiles()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCurIndex = m_lbFiles.GetCurSel();
	if (-1 == nCurIndex)
	{
		return;
	}

	CString strFileName;
	m_lbFiles.GetText(nCurIndex, strFileName);
	strFileName = m_strFilePath + "\\" + strFileName;

	Mat srcImg = imread(strFileName.GetBuffer(0));

	Mat bImg;
	vector<Point> vecLoc;
	vector<Rect> vecRect;
	CMarkerFinder::LocateMarkerArea(srcImg, vecRect);

	namedWindow("img", 0);
	resizeWindow("img", 684, 456);
	imshow("img", srcImg);
}


void CMarkerMatchUIDlg::OnBnClickedRadioWafer()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bDarkerEnv = true;
}


void CMarkerMatchUIDlg::OnBnClickedRadioMask2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bDarkerEnv = false;
}
