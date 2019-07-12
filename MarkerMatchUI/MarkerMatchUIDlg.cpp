
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
	pBtnBrightEnv->SetCheck(1);
	pWnd->EnableWindow(FALSE);
	m_bDarkerEnv = false;

	m_nDistToText = 50;
	UpdateData(FALSE);

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

void CMarkerMatchUIDlg::OnSelchangeListImagefiles()
{
	UpdateData(TRUE);

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCurIndex = m_lbFiles.GetCurSel();
	if (-1 == nCurIndex)
	{
		return;
	}

	//����template;
	string strTempImg_mask = "E:\\MyProject\\MarkerMatch\\template\\solidcross4_b.jpg";
	string strTempImg_wafter = "E:\\MyProject\\MarkerMatch\\template\\temp_mask.jpg";
	Mat tempImg;

	CString strFileName;
	m_lbFiles.GetText(nCurIndex, strFileName);
	strFileName = m_strFilePath + "\\" + strFileName;

	Mat srcImg = imread(strFileName.GetBuffer(0));

	clock_t s, e;
	s = clock();

	//�Ȳ�����������;
	Mat bImg;
	LocTexParam struLTParam;
	vector<Rect> vecTextLoc;
	//������,�ò�����Сһ��;��Ϊ�������Ƚϵ�,�ַ����ܼ�⵽�ĸ��ʵ�;���������Ƚϴ�;
	if (m_bDarkerEnv)
		struLTParam.nStepCountThre = 3 * 2;
	if (!CMarkerFinder::LocateTextArea(srcImg, struLTParam, bImg, vecTextLoc))
		return ;
	
	//����Marker����;
	vector<Rect> vecMakerAreaRect;
	LocWafterMarkerParam wmp;
	wmp.nDistToText = m_nDistToText;
	if (m_bDarkerEnv)
	{
		//��ⰵ��marker;
		CMarkerFinder::FindWafterMarkerArea(srcImg, bImg, wmp, vecTextLoc, vecMakerAreaRect);
		tempImg = imread(strTempImg_wafter);
	}
	else
	{
		//�������marker;
		CMarkerFinder::FindMaskMarkerArea(srcImg, bImg,vecTextLoc,vecMakerAreaRect);
		tempImg = imread(strTempImg_mask);
	}

	//�����ҵ�marker;
	vector<LocMarker> vecMakerLoc;
	CMarkerFinder::LocateMarker_Fine(srcImg, tempImg, vecMakerAreaRect, vecMakerLoc);

	CString strMsg;
	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	strMsg.Format("����ʱ:%.1f MS", dTime);
	m_sttTotalTime.SetWindowTextA(strMsg);

	//��Text����;
	for (int i = 0; i < vecTextLoc.size(); i++)
		rectangle(srcImg, vecTextLoc[i], Scalar(0, 0, 255), 8);
	//��ԭͼ�б��Marker����;������Ӧ��λ������������;
	for (int k = 0; k < vecMakerAreaRect.size(); k++)
		rectangle(srcImg, vecMakerAreaRect[k], Scalar(0, 255, 0), 8);
	//��ʾ���յ�marker����;
	for (int j = 0; j < vecMakerLoc.size(); j++)
	{
		Rect k = vecMakerLoc[j].rect;
		rectangle(srcImg, k, Scalar(255, 0, 255), 8);

		//��ʾmaxvalue
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bDarkerEnv = true;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(TRUE);
}


void CMarkerMatchUIDlg::OnBnClickedRadioMask2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bDarkerEnv = false;
	CEdit * pWnd = (CEdit *)GetDlgItem(IDC_EDIT_DISTTOTEXT);
	pWnd->EnableWindow(FALSE);
}
