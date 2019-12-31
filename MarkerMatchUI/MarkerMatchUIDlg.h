
// MarkerMatchUIDlg.h : ͷ�ļ�
//

#pragma once
#include <opencv2\opencv.hpp>
#include "afxwin.h"
#include "..\\MarkerMatch\MarkerFinder.h"

using namespace std;
using namespace cv;

// CMarkerMatchUIDlg �Ի���
class CMarkerMatchUIDlg : public CDialogEx
{
// ����
public:
	CMarkerMatchUIDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MARKERMATCHUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowser();

	CString              m_strFilePath;  //�ļ���·��;
	std::vector<string>  m_vecFiles;
	int                  m_nAlgMode;     //0:����ģʽ; 1:����ģʽ; 2:FineTune;�����㷨����;
	bool                 m_bSaveResult;  //����ʶ����;
	bool                 m_bGenMarkerDet; //ͨ��ʮ��ģ����;

private:
	void ListAllFiles(CString strFilePath);
	void DrawTempLocResult(Mat srcImg, Scalar color, vector<LocMarker> vecResult, bool bTextUp=true);
	void FindMarker_withText(Mat srcImg);
	void FindMarker_General_Temp(Mat srcImg);     //ֱ��ȫͼʹ��ģ��ƥ��;
	void FindMarker_General_HogTemp(Mat srcImg);  //��hog��λ,��ģ��ƥ��;
	void SaveResults(Mat srcImg, vector<LocMarker> vecResult);
	bool AdjustRect(Rect & rH, Rect &rS);  //��������ʵ�������С;

public:
	CMarkerFinder  m_mf;
	CEdit m_editFilePath;
	CListBox m_lbFiles;
	CString  m_strCurFileName;  //��ǰ�ļ���;
	CStatic m_sttTotal;
	afx_msg void OnSelchangeListImagefiles();
	afx_msg void OnBnClickedRadioWafer();
	afx_msg void OnBnClickedRadioMask2();
	CStatic m_sttTotalTime;
	int m_nDistToText;
	afx_msg void OnBnClickedCheckGenMakerdetector();
	CButton m_chkGenMarkerDet;
	CButton m_chkSaveResult;
	afx_msg void OnBnClickedCheckSaveresult();
	afx_msg void OnBnClickedButtonBatch();
	afx_msg void OnBnClickedRadioFinetune();
	CStatic m_sttLocInfo;
	int m_nGenTempImageDelta;
	afx_msg void OnBnClickedRadioGentempimage();
	afx_msg void OnBnClickedButtonTemp();
};
