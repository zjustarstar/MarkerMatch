#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

//��λ��������ʱ�Ĳ���;
typedef struct LocTexParam{
	int nTextBandWThre; //���ֿ��;
	int nStepCountThre; //���ֵ�����ַ�����(��Ծ��);
	LocTexParam() {
		nTextBandWThre = 100;   //ԭͼ��Сʱ�Ŀ��;
		nStepCountThre = 8 * 2; //8����򵥵��ַ�;
	}
};

//��λwafer maker(����)ʱ�Ĳ���;
typedef struct LocWafterMarkerParam {
	int nDistToText;   //Marker�ұ�Ե������֮��ľ���;
	int nMarkerWidth;    //marker�Ŀ��;
	LocWafterMarkerParam() {
		nMarkerWidth = 180;
	}
};

//��λMarkerʱ�Ľ��;
typedef struct LocMarker {
	Rect rect;
	float fConfidence;
	LocMarker() {
		rect = Rect(0, 0, 0, 0);
		fConfidence = 0.0;
	}
};

class CMarkerFinder
{
public:
	CMarkerFinder();
	virtual ~CMarkerFinder();
	//��λ��������;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	//��λmarker����λ��;
	static bool LocateMarker_Fine(Mat srcImg, Mat templateImg, vector<Rect> vecMarkerAreaRect,
		                          vector<LocMarker> &vecMarkerLoc);

	//���������µ�marker����ʵ��ʮ��;
	static bool FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect);
	//���Ұ����µ�marker����,����ʮ��;
	static bool FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp, 
		                      vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect);

	//������;
	static bool LocateTemplate(Mat srcImg, Mat tempImg, int nMaxCount,
		vector<Rect> & vecTempRect);

private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
};

