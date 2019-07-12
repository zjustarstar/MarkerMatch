
// MarkerMatchUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MarkerMatchUI.h"
#include "MarkerMatchUIDlg.h"
#include "afxdialogex.h"
#include "..\\MarkerMatch\MarkerFinder.h"
#include <opencv2/imgproc/types_c.h>

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
	, m_nDistToText(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMarkerMatchUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FOLDERPATH, m_editFilePath);
	DDX_Control(pDX, IDC_LIST_IMAGEFILES, m_lbFiles);
	DDX_Control(pDX, IDC_STATIC_TOTALFILES, m_sttTotal);
	DDX_Control(pDX, IDC_STATIC_SPENDING, m_sttTotalTime);
	DDX_Text(pDX, IDC_EDIT_DISTTOTEXT, m_nDistToText);
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
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	CButton * pBtnDarkerEnv = (CButton *)GetDlgItem(IDC_RADIO_WAFER);
	CButton * pBtnBrightEnv = (CButton *)GetDlgItem(IDC_RADIO_MASK2);
	pBtnBrightEnv->SetCheck(1);
	pWnd->EnableWindow(FALSE);
	m_bDarkerEnv = false;

	m_nDistToText = 50;
	UpdateData(FALSE);

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
	UpdateData(TRUE);

	// TODO: 在此添加控件通知处理程序代码
	int nCurIndex = m_lbFiles.GetCurSel();
	if (-1 == nCurIndex)
	{
		return;
	}

	//明场template;
	string strTempImg_mask = "E:\\MyProject\\MarkerMatch\\template\\solidcross4_b.jpg";
	string strTempImg_wafter = "E:\\MyProject\\MarkerMatch\\template\\temp_mask.jpg";
	Mat tempImg;

	CString strFileName;
	m_lbFiles.GetText(nCurIndex, strFileName);
	strFileName = m_strFilePath + "\\" + strFileName;

	Mat srcImg = imread(strFileName.GetBuffer(0));

	clock_t s, e;
	s = clock();

	//先查找文字区域;
	Mat bImg;
	LocTexParam struLTParam;
	vector<Rect> vecTextLoc;
	//暗场下,该参数更小一点;因为总体亮度较低,字符可能检测到的概率低;明场下亮度较大;
	if (m_bDarkerEnv)
		struLTParam.nStepCountThre = 3 * 2;
	if (!CMarkerFinder::LocateTextArea(srcImg, struLTParam, bImg, vecTextLoc))
		return ;
	
	//再找Marker区域;
	vector<Rect> vecMakerAreaRect;
	LocWafterMarkerParam wmp;
	wmp.nDistToText = m_nDistToText;
	if (m_bDarkerEnv)
	{
		//检测暗场marker;
		CMarkerFinder::FindWafterMarkerArea(srcImg, bImg, wmp, vecTextLoc, vecMakerAreaRect);
		tempImg = imread(strTempImg_wafter);
	}
	else
	{
		//检测明场marker;
		CMarkerFinder::FindMaskMarkerArea(srcImg, bImg,vecTextLoc,vecMakerAreaRect);
		tempImg = imread(strTempImg_mask);
	}

	//最终找到marker;
	vector<LocMarker> vecMakerLoc;
	CMarkerFinder::LocateMarker_Fine(srcImg, tempImg, vecMakerAreaRect, vecMakerLoc);

	CString strMsg;
	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	strMsg.Format("检测耗时:%.1f MS", dTime);
	m_sttTotalTime.SetWindowTextA(strMsg);

	//画Text区域;
	for (int i = 0; i < vecTextLoc.size(); i++)
		rectangle(srcImg, vecTextLoc[i], Scalar(0, 0, 255), 8);
	//在原图中标记Marker区域;该区域应该位于文字区域中;
	for (int k = 0; k < vecMakerAreaRect.size(); k++)
		rectangle(srcImg, vecMakerAreaRect[k], Scalar(0, 255, 0), 8);
	//显示最终的marker区域;
	for (int j = 0; j < vecMakerLoc.size(); j++)
	{
		Rect k = vecMakerLoc[j].rect;
		rectangle(srcImg, k, Scalar(255, 0, 255), 8);

		//显示maxvalue
		char msg[10];
		sprintf_s(msg, "%.2f", vecMakerLoc[j].fConfidence);
		putText(srcImg, string(msg), cvPoint(k.x, k.y - 10), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);
	}

	namedWindow("img", 0);
	resizeWindow("img", 684, 456);
	imshow("img", srcImg);
}


void CMarkerMatchUIDlg::OnBnClickedRadioWafer()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bDarkerEnv = true;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(TRUE);
}


void CMarkerMatchUIDlg::OnBnClickedRadioMask2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bDarkerEnv = false;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(FALSE);
}
