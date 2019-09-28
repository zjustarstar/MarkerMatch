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
	, m_nGenTempImageDelta(0)
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
	DDX_Control(pDX, IDC_CHECK_SAVERESULT, m_chkSaveResult);
	DDX_Control(pDX, IDC_CHECK_GEN_MAKERDETECTOR, m_chkGenMarkerDet);
	DDX_Text(pDX, IDC_EDIT_DISTTOTEXT, m_nDistToText);
	DDX_Text(pDX, IDC_EDIT_DELTA, m_nGenTempImageDelta);
	DDX_Control(pDX, IDC_STATIC_CROSS_LOCATION, m_sttLocInfo);
}

BEGIN_MESSAGE_MAP(CMarkerMatchUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CMarkerMatchUIDlg::OnBnClickedButtonBrowser)
	ON_LBN_SELCHANGE(IDC_LIST_IMAGEFILES, &CMarkerMatchUIDlg::OnSelchangeListImagefiles)
	ON_BN_CLICKED(IDC_RADIO_WAFER, &CMarkerMatchUIDlg::OnBnClickedRadioWafer)
	ON_BN_CLICKED(IDC_RADIO_MASK2, &CMarkerMatchUIDlg::OnBnClickedRadioMask2)
	ON_BN_CLICKED(IDC_BUTTON_BATCH, &CMarkerMatchUIDlg::OnBnClickedButtonBatch)
	ON_BN_CLICKED(IDC_CHECK_SAVERESULT, &CMarkerMatchUIDlg::OnBnClickedCheckSaveresult)
	ON_BN_CLICKED(IDC_CHECK_GEN_MAKERDETECTOR, &CMarkerMatchUIDlg::OnBnClickedCheckGenMakerdetector)
	ON_BN_CLICKED(IDC_RADIO_FINETUNE, &CMarkerMatchUIDlg::OnBnClickedRadioFinetune)
	ON_BN_CLICKED(IDC_RADIO_GENTEMPIMAGE, &CMarkerMatchUIDlg::OnBnClickedRadioGentempimage)
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
	CButton * pBtnFinetune = (CButton *)GetDlgItem(IDC_RADIO_FINETUNE);
	CButton * pGenDetection = (CButton *)GetDlgItem(IDC_CHECK_GEN_MAKERDETECTOR);
	pBtnFinetune->SetCheck(1);
	pWnd->EnableWindow(FALSE);
	pGenDetection->SetCheck(1);

	m_nAlgMode = 2;
	m_bGenMarkerDet = true;

	m_nDistToText = 50;
	UpdateData(FALSE);

	//暗/明场template;
	string strTempImg_mask   = "E:\\MyProject\\MarkerMatch\\MarkerMatch\\x64\\Release\\template\\temp_solidcross.jpg";
	string strTempImg_wafter = "E:\\MyProject\\MarkerMatch\\MarkerMatch\\x64\\Release\\template\\temp_hollowcross.jpg"; 
	Mat tempImg_h, tempImg_s;
	tempImg_h = imread(strTempImg_wafter);
	tempImg_s = imread(strTempImg_mask);

	AlgParam ap;
	ap.finetune_nHcMargin = 0;
	ap.refine_nScThickSize = 34;//36;  //24;
	ap.refine_nHcThickSize = 8;  //12; //6
	//设置检测用的cross marker;
	if (!m_mf.Init(tempImg_h, tempImg_s, Mat::Mat(), Mat::Mat(),ap))
		AfxMessageBox("fail to init");

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

//根据十字框微调实心十字rect位置。实心十字如果离虚心十字太近了，则往相反方向稍微移动一点.
//两者靠的太近时,二值化容易黏在一起,导致匹配时产生偏差.往反方向移动可以抵消这种偏差。
bool CMarkerMatchUIDlg::AdjustRect(Rect & rH, Rect &rS) {
	int DIST = 4;  //距离阈值;
	int MOVE = 3;  //移动值
	bool bAdjust = false;

	//左边
	if (abs(rS.x - rH.x) <= DIST)
	{
		rS.x += MOVE;
		bAdjust = true;
	}
	//右边
	else {
		int rightS = rS.x + rS.width;
		int rightH = rH.x + rH.width;
		if (abs(rightH - rightS) <= DIST)
		{
			rS.x -= MOVE;
			bAdjust = true;
		}
	}

	//上边;
	if (abs(rS.y - rH.y) <= DIST)
	{
		rS.y += MOVE;
		bAdjust = true;
	}
	else {
		int bottomS = rS.y + rS.height;
		int bottomH = rH.y + rH.height;
		if (abs(bottomH - bottomS) <= DIST)
		{
			rS.y -= MOVE;
			bAdjust = true;
		}
	}

	return bAdjust;
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

	CString strFileName;
	m_lbFiles.GetText(nCurIndex, strFileName);
	m_strCurFileName = strFileName;
	strFileName = m_strFilePath + "\\" + strFileName;

	Mat srcImg = imread(strFileName.GetBuffer(0));

	//模板匹配;
	if (m_nAlgMode == 3)
	{
		Mat bImg = srcImg;
		m_mf.Util_GenTempImg(srcImg, m_nGenTempImageDelta,bImg);

		namedWindow("src");
		imshow("src", srcImg);
		namedWindow("b");
		imshow("b", bImg);

		waitKey(1);
	}
	//fine tune;
	else if (m_nAlgMode == 2)
	{
		Mat b;
		Rect rH, rS;

		clock_t s, e;
		s = clock();

		namedWindow("sourceImage");
		imshow("sourceImage", srcImg);

		m_mf.FinalFinetune(srcImg, b,rH,rS);
		rectangle(srcImg, rS, Scalar(0, 0, 255), 1);

		CString strMsg;
		e = clock();
		double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
		strMsg.Format("检测耗时:%.1f MS", dTime);
		m_sttTotalTime.SetWindowTextA(strMsg);

		//画出结果;
		cv::Point p_sc;  //实心十字中心点;
		p_sc.x = rS.x + rS.width / 2;
		p_sc.y = rS.y + rS.height / 2;

		Point p_hc;     //虚心十字中心点
		p_hc.x = rH.x + rH.width / 2;
		p_hc.y = rH.y + rH.height / 2;

		int nDeltaX, nDeltaY;
		nDeltaX = p_sc.x - p_hc.x;
		nDeltaY = p_sc.y - p_hc.y;
		strMsg.Format("定位结果：x=%d, y=%d\n\t deltaX=%d, deltaY=%d", p_hc.x, p_hc.y, nDeltaX, nDeltaY);
		//strMsg.Format("deltaX=%d, deltaY=%d", nDeltaX, nDeltaY);
		m_sttLocInfo.SetWindowTextA(strMsg);

		if (abs(nDeltaX) <= 5 && abs(nDeltaY) <= 5) {
			m_mf.FT_RefineSolidCross(srcImg, rS);
			p_sc.x = rS.x + rS.width / 2;
			p_sc.y = rS.y + rS.height / 2;

			nDeltaX = p_sc.x - p_hc.x;
			nDeltaY = p_sc.y - p_hc.y;
			CString strTemp;
			strTemp.Format("\n 新结果：deltaX=%d, deltaY=%d",nDeltaX, nDeltaY);
			m_sttLocInfo.SetWindowTextA(strMsg + strTemp);

			rectangle(srcImg, rS, Scalar(0, 255, 255), 1);
		}
	
		circle(srcImg, p_hc, 2, Scalar(255, 0, 0), 1);
		circle(srcImg, p_sc, 2, Scalar(0, 0, 255), 1);

		rectangle(srcImg, rH, Scalar(255, 0, 0), 1);
		
		namedWindow("bimg", 0);
		resizeWindow("bimg", 500, 500);
		imshow("bimg", b);

		namedWindow("img", 0);
		resizeWindow("img", 500, 500);
		imshow("img", srcImg);

		//imwrite("d:\\finetune.jpg", srcImg);
	}
	//其它检测;
	else
	{
		//非通用的检测方法，通过文字区域定位;
		if (!m_bGenMarkerDet)
			FindMarker_withText(srcImg);
		else
			FindMarker_General_HogTemp(srcImg);

		namedWindow("img", 0);
		resizeWindow("img", 684, 456);
		imshow("img", srcImg);
		//imwrite("d:\\result.jpg", srcImg);
	}

}


void CMarkerMatchUIDlg::OnBnClickedRadioWafer()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nAlgMode = 0;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(TRUE);
}


void CMarkerMatchUIDlg::OnBnClickedRadioMask2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nAlgMode = 1;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(FALSE);
}

//显示模板匹配的结果。同时显示定位框和匹配度文字;
//bTextUp为true表示文字显示在框上面
void CMarkerMatchUIDlg::DrawTempLocResult(Mat srcImg, Scalar color, vector<LocMarker> vecResult,bool bTextUp)
{
	//显示最终的marker区域;
	for (int j = 0; j < vecResult.size(); j++)
	{
		Rect k = vecResult[j].rect;
		rectangle(srcImg, k, color, 8);

		//显示maxvalue
		char msg[10];
		sprintf_s(msg, "%.2f", vecResult[j].fConfidence);

		//默认文字显示在框上;
		int y = (k.y - 10) > 0 ? (k.y - 10) : 0;
		if (!bTextUp)
		{
			y = (k.y + k.height + 50);
			if (y >= srcImg.rows)
				y = srcImg.rows - 1;
		}
		putText(srcImg, string(msg), cvPoint(k.x, y), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);
	}

}

//基于文字定位的marker检测方法;
void CMarkerMatchUIDlg::FindMarker_withText(Mat srcImg) {
	//明场template;
	string strTempImg_mask = "E:\\MyProject\\MarkerMatch\\template\\temp_solidcross.jpg";
	string strTempImg_wafter = "E:\\MyProject\\MarkerMatch\\template\\temp_hollowcross.jpg";
	Mat tempImg_h = imread(strTempImg_wafter);
	Mat tempImg_s = imread(strTempImg_mask);

	CMarkerFinder mf;
	AlgParam ap;
	mf.Init(tempImg_h, tempImg_s, Mat::Mat(), Mat::Mat(),ap);

	clock_t s, e;
	s = clock();

	//先查找文字区域;
	Mat bImg;
	LocTexParam struLTParam;
	vector<Rect> vecTextLoc;
	//暗场下,该参数更小一点;因为总体亮度较低,字符可能检测到的概率低;明场下亮度较大;
	if (m_nAlgMode == 0)
		struLTParam.nStepCountThre = 3 * 2;
	if (!mf.LocateTextArea(srcImg, struLTParam, bImg, vecTextLoc))
		return;

	//再找Marker区域;
	vector<Rect> vecMakerAreaRect;
	LocWafterMarkerParam wmp;
	wmp.nDistToText = m_nDistToText;
	bool bHollowCross;
	if (m_nAlgMode == 0)
	{
		//检测暗场marker;
		mf.FindWafterMarkerArea(srcImg, bImg, wmp, vecTextLoc, vecMakerAreaRect);
		bHollowCross = true;
	}
	else if (m_nAlgMode == 1)
	{
		//检测明场marker;
		mf.FindMaskMarkerArea(srcImg, bImg, vecTextLoc, vecMakerAreaRect);
		bHollowCross = false;
	}
	else
		return;

	//最终找到marker;
	vector<LocMarker> vecMakerLoc;
	mf.LocateMarkerByTempMatch(srcImg, bHollowCross, vecMakerAreaRect, 0.4, vecMakerLoc);

	CString strMsg;
	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	strMsg.Format("检测耗时:%.1f MS", dTime);
	m_sttTotalTime.SetWindowTextA(strMsg);

	//在画之前先保存数据;
	if (m_bSaveResult)
		SaveResults(srcImg, vecMakerLoc);

	//画Text区域;
	for (int i = 0; i < vecTextLoc.size(); i++)
		rectangle(srcImg, vecTextLoc[i], Scalar(0, 0, 255), 8);
	//在原图中标记Marker区域;该区域应该位于文字区域中;
	for (int k = 0; k < vecMakerAreaRect.size(); k++)
		rectangle(srcImg, vecMakerAreaRect[k], Scalar(0, 255, 0), 8);
	//显示最终的marker区域;
	DrawTempLocResult(srcImg, Scalar(255, 0, 255), vecMakerLoc);
}


//通用的marker检测方法,直接采用template检测；
void CMarkerMatchUIDlg::FindMarker_General_Temp(Mat srcImg) {

}


//通用的marker检测方法，结合hog和template match;
void CMarkerMatchUIDlg::FindMarker_General_HogTemp(Mat srcImg) {

	bool bHollowCross;
	double dHitThre;
	if (m_nAlgMode == 0)
	{
		bHollowCross = true;
		dHitThre = -0.7;  //暗场的参数
	}
	else if (m_nAlgMode == 1) {
		bHollowCross = false;
		dHitThre = -0.7;  //明场的参数; //原来-0.5
	}
	else
		return;

	clock_t s, e;
	s = clock();

	//首先检测到有十字的区域;
	vector<LocMarker> vecMarkerArea;
	m_mf.LocateCrossAreaByHog(srcImg, dHitThre, bHollowCross, vecMarkerArea);

	//然后进行模板匹配;
	vector<Rect> vecMarkerRect;
	for (int i = 0; i < vecMarkerArea.size(); i++)
		vecMarkerRect.push_back(vecMarkerArea[i].rect);
	vector<LocMarker> vecResult;
	m_mf.LocateMarkerByTempMatch(srcImg, bHollowCross, vecMarkerRect, 0.3, vecResult);

	CString strMsg;
	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	strMsg.Format("检测耗时:%.1f MS", dTime);
	m_sttTotalTime.SetWindowTextA(strMsg);

	//在画之前先保存数据;
	if (m_bSaveResult)
		SaveResults(srcImg, vecResult);

	//hog区域;
	//DrawTempLocResult(srcImg, Scalar(0, 255, 0), vecMarkerArea,false);
	//最终定位的marker区域，更细致;
	DrawTempLocResult(srcImg, Scalar(0,0,255), vecResult);
}

//保存结果;
void CMarkerMatchUIDlg::SaveResults(Mat srcImg, vector<LocMarker> vecResult) {

	string strHollowCross = "d:\\CrossData\\HollowCross\\";
	string strSolidCross = "d:\\CrossData\\SolidCross\\";

	string strPath;
	if (m_nAlgMode == 0)
		strPath = strHollowCross;
	else if (m_nAlgMode == 1)
		strPath = strSolidCross;
	else if (m_nAlgMode == 2)
		return;

	int nSize = vecResult.size();
	int mon, d, h, m, ms;
	CTime t = GetCurrentTime();
	mon = t.GetMonth();
	d = t.GetDay();
	h = t.GetHour();
	m = t.GetMinute();
	ms = t.GetSecond();

	//为了不重名，按照细到秒的时间保存数据;
	string strFileName;
	int xMargin = 40;
	int yMargin = 40;
	for (int i = 0; i < nSize; i++)
	{
		Rect r;
		r = vecResult[i].rect;
		r.x -= xMargin;
		if (r.x < 0) r.x = 0;
		r.y -= xMargin;
		if (r.y < 0) r.y = 0;
		r.width += (xMargin*2);
		if (r.x + r.width > srcImg.cols)
			r.width = srcImg.cols - r.x - 1;
		r.height += (yMargin*2);
		if (r.y + r.height > srcImg.rows)
			r.height = srcImg.rows - r.y - 1;

		char chName[256];
		//sprintf_s(chName, "%d%d%d%d%d_%d.jpg", mon, d, h, m, ms, i);
		string strTemp(m_strCurFileName.GetBuffer(0));
		strTemp = strTemp.substr(0, strTemp.length() - 4); //不要.jpg;
		sprintf_s(chName, "%s_%d.jpg", strTemp.c_str(), i);

		strFileName = strPath + string(chName);
		imwrite(strFileName, srcImg(r));
	}
}

void CMarkerMatchUIDlg::OnBnClickedCheckGenMakerdetector() {
	m_bGenMarkerDet = m_chkGenMarkerDet.GetCheck();
}

void CMarkerMatchUIDlg::OnBnClickedCheckSaveresult() {
	m_bSaveResult = m_chkSaveResult.GetCheck();
}

void CMarkerMatchUIDlg::OnBnClickedButtonBatch() {

	for (int i = 0; i < m_vecFiles.size(); i++)
	{
		m_lbFiles.SetCurSel(i);

		CString strFileName;
		m_lbFiles.GetText(i, strFileName);
		m_strCurFileName = strFileName;
		strFileName = m_strFilePath + "\\" + strFileName;

		Mat srcImg = imread(strFileName.GetBuffer(0));

		//生成模板文件的测试;
		if (m_nAlgMode == 3)
		{

		}
		else
		{
			//非通用的检测方法，通过文字区域定位;
			if (!m_bGenMarkerDet)
				FindMarker_withText(srcImg);
			else
				FindMarker_General_HogTemp(srcImg);
		}

	}
}

void CMarkerMatchUIDlg::OnBnClickedRadioFinetune()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nAlgMode = 2;
}


void CMarkerMatchUIDlg::OnBnClickedRadioGentempimage()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nAlgMode = 3;
}
