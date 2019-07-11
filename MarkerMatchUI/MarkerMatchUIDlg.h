
// MarkerMatchUIDlg.h : 头文件
//

#pragma once
#include <opencv2\opencv.hpp>
#include "afxwin.h"

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
	bool                 m_bDarkerEnv;   //暗场模式;

private:
	void ListAllFiles(CString strFilePath);

public:
	CEdit m_editFilePath;
	CListBox m_lbFiles;
	CStatic m_sttTotal;
	afx_msg void OnSelchangeListImagefiles();
	afx_msg void OnBnClickedRadioWafer();
	afx_msg void OnBnClickedRadioMask2();
};
