// MarkerMatchUIDlg.cpp : ʵ���ļ�
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


// CMarkerMatchUIDlg �Ի���

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


// CMarkerMatchUIDlg ��Ϣ�������

BOOL CMarkerMatchUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

									// TODO: �ڴ���Ӷ���ĳ�ʼ������
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

	//��/����template;
	string strTempImg_mask   = "E:\\MyProject\\MarkerMatch\\MarkerMatch\\x64\\Release\\template\\temp_solidcross.jpg";
	string strTempImg_wafter = "E:\\MyProject\\MarkerMatch\\MarkerMatch\\x64\\Release\\template\\temp_hollowcross.jpg"; 
	Mat tempImg_h, tempImg_s;
	tempImg_h = imread(strTempImg_wafter);
	tempImg_s = imread(strTempImg_mask);

	AlgParam ap;
	ap.finetune_nHcMargin = 0;
	ap.refine_nScThickSize = 34;//36;  //24;
	ap.refine_nHcThickSize = 8;  //12; //6
	//���ü���õ�cross marker;
	if (!m_mf.Init(tempImg_h, tempImg_s, Mat::Mat(), Mat::Mat(),ap))
		AfxMessageBox("fail to init");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMarkerMatchUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMarkerMatchUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMarkerMatchUIDlg::OnBnClickedButtonBrowser()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;
	char Buffer[MAX_PATH];

	//��ʼ����ڲ���bi��ʼ
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;//��ʼ���ƶ���rootĿ¼�ܲ����ף�
	bi.pszDisplayName = Buffer;//�˲�����ΪNULL������ʾ�Ի���
	bi.lpszTitle = "ѡ������ļ���";
	bi.lpfn = NULL;
	bi.ulFlags = BIF_EDITBOX;//�����ļ�

	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//������ʾѡ��Ի���
	if (pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		//ȡ���ļ���·����Buffer��
		m_strFilePath.Format("%s", Buffer);
		m_editFilePath.SetWindowText(m_strFilePath);
	}

	ListAllFiles(m_strFilePath);

	int nSize = m_lbFiles.GetCount();
	CString strTile;
	strTile.Format("һ����%d���ļ�", nSize);
	m_sttTotal.SetWindowText(strTile);

	//������ͼ���ļ�������vector��;
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

	//��õ�ǰ�ļ����µ�����ͼ��;
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

//����ʮ�ֿ�΢��ʵ��ʮ��rectλ�á�ʵ��ʮ�����������ʮ��̫���ˣ������෴������΢�ƶ�һ��.
//���߿���̫��ʱ,��ֵ���������һ��,����ƥ��ʱ����ƫ��.���������ƶ����Ե�������ƫ�
bool CMarkerMatchUIDlg::AdjustRect(Rect & rH, Rect &rS) {
	int DIST = 4;  //������ֵ;
	int MOVE = 3;  //�ƶ�ֵ
	bool bAdjust = false;

	//���
	if (abs(rS.x - rH.x) <= DIST)
	{
		rS.x += MOVE;
		bAdjust = true;
	}
	//�ұ�
	else {
		int rightS = rS.x + rS.width;
		int rightH = rH.x + rH.width;
		if (abs(rightH - rightS) <= DIST)
		{
			rS.x -= MOVE;
			bAdjust = true;
		}
	}

	//�ϱ�;
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

	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

	//ģ��ƥ��;
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
		strMsg.Format("����ʱ:%.1f MS", dTime);
		m_sttTotalTime.SetWindowTextA(strMsg);

		//�������;
		cv::Point p_sc;  //ʵ��ʮ�����ĵ�;
		p_sc.x = rS.x + rS.width / 2;
		p_sc.y = rS.y + rS.height / 2;

		Point p_hc;     //����ʮ�����ĵ�
		p_hc.x = rH.x + rH.width / 2;
		p_hc.y = rH.y + rH.height / 2;

		int nDeltaX, nDeltaY;
		nDeltaX = p_sc.x - p_hc.x;
		nDeltaY = p_sc.y - p_hc.y;
		strMsg.Format("��λ�����x=%d, y=%d\n\t deltaX=%d, deltaY=%d", p_hc.x, p_hc.y, nDeltaX, nDeltaY);
		//strMsg.Format("deltaX=%d, deltaY=%d", nDeltaX, nDeltaY);
		m_sttLocInfo.SetWindowTextA(strMsg);

		if (abs(nDeltaX) <= 5 && abs(nDeltaY) <= 5) {
			m_mf.FT_RefineSolidCross(srcImg, rS);
			p_sc.x = rS.x + rS.width / 2;
			p_sc.y = rS.y + rS.height / 2;

			nDeltaX = p_sc.x - p_hc.x;
			nDeltaY = p_sc.y - p_hc.y;
			CString strTemp;
			strTemp.Format("\n �½����deltaX=%d, deltaY=%d",nDeltaX, nDeltaY);
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
	//�������;
	else
	{
		//��ͨ�õļ�ⷽ����ͨ����������λ;
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_nAlgMode = 0;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(TRUE);
}


void CMarkerMatchUIDlg::OnBnClickedRadioMask2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_nAlgMode = 1;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(FALSE);
}

//��ʾģ��ƥ��Ľ����ͬʱ��ʾ��λ���ƥ�������;
//bTextUpΪtrue��ʾ������ʾ�ڿ�����
void CMarkerMatchUIDlg::DrawTempLocResult(Mat srcImg, Scalar color, vector<LocMarker> vecResult,bool bTextUp)
{
	//��ʾ���յ�marker����;
	for (int j = 0; j < vecResult.size(); j++)
	{
		Rect k = vecResult[j].rect;
		rectangle(srcImg, k, color, 8);

		//��ʾmaxvalue
		char msg[10];
		sprintf_s(msg, "%.2f", vecResult[j].fConfidence);

		//Ĭ��������ʾ�ڿ���;
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

//�������ֶ�λ��marker��ⷽ��;
void CMarkerMatchUIDlg::FindMarker_withText(Mat srcImg) {
	//����template;
	string strTempImg_mask = "E:\\MyProject\\MarkerMatch\\template\\temp_solidcross.jpg";
	string strTempImg_wafter = "E:\\MyProject\\MarkerMatch\\template\\temp_hollowcross.jpg";
	Mat tempImg_h = imread(strTempImg_wafter);
	Mat tempImg_s = imread(strTempImg_mask);

	CMarkerFinder mf;
	AlgParam ap;
	mf.Init(tempImg_h, tempImg_s, Mat::Mat(), Mat::Mat(),ap);

	clock_t s, e;
	s = clock();

	//�Ȳ�����������;
	Mat bImg;
	LocTexParam struLTParam;
	vector<Rect> vecTextLoc;
	//������,�ò�����Сһ��;��Ϊ�������Ƚϵ�,�ַ����ܼ�⵽�ĸ��ʵ�;���������Ƚϴ�;
	if (m_nAlgMode == 0)
		struLTParam.nStepCountThre = 3 * 2;
	if (!mf.LocateTextArea(srcImg, struLTParam, bImg, vecTextLoc))
		return;

	//����Marker����;
	vector<Rect> vecMakerAreaRect;
	LocWafterMarkerParam wmp;
	wmp.nDistToText = m_nDistToText;
	bool bHollowCross;
	if (m_nAlgMode == 0)
	{
		//��ⰵ��marker;
		mf.FindWafterMarkerArea(srcImg, bImg, wmp, vecTextLoc, vecMakerAreaRect);
		bHollowCross = true;
	}
	else if (m_nAlgMode == 1)
	{
		//�������marker;
		mf.FindMaskMarkerArea(srcImg, bImg, vecTextLoc, vecMakerAreaRect);
		bHollowCross = false;
	}
	else
		return;

	//�����ҵ�marker;
	vector<LocMarker> vecMakerLoc;
	mf.LocateMarkerByTempMatch(srcImg, bHollowCross, vecMakerAreaRect, 0.4, vecMakerLoc);

	CString strMsg;
	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	strMsg.Format("����ʱ:%.1f MS", dTime);
	m_sttTotalTime.SetWindowTextA(strMsg);

	//�ڻ�֮ǰ�ȱ�������;
	if (m_bSaveResult)
		SaveResults(srcImg, vecMakerLoc);

	//��Text����;
	for (int i = 0; i < vecTextLoc.size(); i++)
		rectangle(srcImg, vecTextLoc[i], Scalar(0, 0, 255), 8);
	//��ԭͼ�б��Marker����;������Ӧ��λ������������;
	for (int k = 0; k < vecMakerAreaRect.size(); k++)
		rectangle(srcImg, vecMakerAreaRect[k], Scalar(0, 255, 0), 8);
	//��ʾ���յ�marker����;
	DrawTempLocResult(srcImg, Scalar(255, 0, 255), vecMakerLoc);
}


//ͨ�õ�marker��ⷽ��,ֱ�Ӳ���template��⣻
void CMarkerMatchUIDlg::FindMarker_General_Temp(Mat srcImg) {

}


//ͨ�õ�marker��ⷽ�������hog��template match;
void CMarkerMatchUIDlg::FindMarker_General_HogTemp(Mat srcImg) {

	bool bHollowCross;
	double dHitThre;
	if (m_nAlgMode == 0)
	{
		bHollowCross = true;
		dHitThre = -0.7;  //�����Ĳ���
	}
	else if (m_nAlgMode == 1) {
		bHollowCross = false;
		dHitThre = -0.7;  //�����Ĳ���; //ԭ��-0.5
	}
	else
		return;

	clock_t s, e;
	s = clock();

	//���ȼ�⵽��ʮ�ֵ�����;
	vector<LocMarker> vecMarkerArea;
	m_mf.LocateCrossAreaByHog(srcImg, dHitThre, bHollowCross, vecMarkerArea);

	//Ȼ�����ģ��ƥ��;
	vector<Rect> vecMarkerRect;
	for (int i = 0; i < vecMarkerArea.size(); i++)
		vecMarkerRect.push_back(vecMarkerArea[i].rect);
	vector<LocMarker> vecResult;
	m_mf.LocateMarkerByTempMatch(srcImg, bHollowCross, vecMarkerRect, 0.3, vecResult);

	CString strMsg;
	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	strMsg.Format("����ʱ:%.1f MS", dTime);
	m_sttTotalTime.SetWindowTextA(strMsg);

	//�ڻ�֮ǰ�ȱ�������;
	if (m_bSaveResult)
		SaveResults(srcImg, vecResult);

	//hog����;
	//DrawTempLocResult(srcImg, Scalar(0, 255, 0), vecMarkerArea,false);
	//���ն�λ��marker���򣬸�ϸ��;
	DrawTempLocResult(srcImg, Scalar(0,0,255), vecResult);
}

//������;
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

	//Ϊ�˲�����������ϸ�����ʱ�䱣������;
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
		strTemp = strTemp.substr(0, strTemp.length() - 4); //��Ҫ.jpg;
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

		//����ģ���ļ��Ĳ���;
		if (m_nAlgMode == 3)
		{

		}
		else
		{
			//��ͨ�õļ�ⷽ����ͨ����������λ;
			if (!m_bGenMarkerDet)
				FindMarker_withText(srcImg);
			else
				FindMarker_General_HogTemp(srcImg);
		}

	}
}

void CMarkerMatchUIDlg::OnBnClickedRadioFinetune()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_nAlgMode = 2;
}


void CMarkerMatchUIDlg::OnBnClickedRadioGentempimage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_nAlgMode = 3;
}
