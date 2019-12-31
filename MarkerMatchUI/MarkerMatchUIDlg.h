
// MarkerMatchUIDlg.h : 头文件
//

#pragma once
#include <opencv2\opencv.hpp>
#include "afxwin.h"
#include "..\\MarkerMatch\MarkerFinder.h"

using namespace std;
using namespace cv;

// CMarkerMatchUIDlg 对话框
class CMarkerMatchUIDlg : public CDialogEx
{
// 构造
public:
	CMarkerMatchUIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MARKERMATCHUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowser();

	CString              m_strFilePath;  //文件夹路径;
	std::vector<string>  m_vecFiles;
	int                  m_nAlgMode;     //0:暗场模式; 1:明场模式; 2:FineTune;用于算法测试;
	bool                 m_bSaveResult;  //保存识别结果;
	bool                 m_bGenMarkerDet; //通用十字模板检测;

private:
	void ListAllFiles(CString strFilePath);
	void DrawTempLocResult(Mat srcImg, Scalar color, vector<LocMarker> vecResult, bool bTextUp=true);
	void FindMarker_withText(Mat srcImg);
	void FindMarker_General_Temp(Mat srcImg);     //直接全图使用模板匹配;
	void FindMarker_General_HogTemp(Mat srcImg);  //先hog定位,再模板匹配;
	void SaveResults(Mat srcImg, vector<LocMarker> vecResult);
	bool AdjustRect(Rect & rH, Rect &rS);  //根据虚框和实框调整大小;

public:
	CMarkerFinder  m_mf;
	CEdit m_editFilePath;
	CListBox m_lbFiles;
	CString  m_strCurFileName;  //当前文件名;
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
