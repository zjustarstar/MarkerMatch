
// MarkerMatchUIDlg.h : ͷ�ļ�
//

#pragma once
#include <opencv2\opencv.hpp>
#include "afxwin.h"

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
	bool                 m_bDarkerEnv;   //����ģʽ;

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
