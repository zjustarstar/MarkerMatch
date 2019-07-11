#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

typedef struct LocTexParam{
	int nTextBandWThre; //���ֿ��;
	int nStepCountThre; //���ֵ�����ַ�����(��Ծ��);
	LocTexParam() {
		nTextBandWThre = 100;   //ԭͼ��Сʱ�Ŀ��;
		nStepCountThre = 8 * 2; //8����򵥵��ַ�;
	}
};

class CMarkerFinder
{
public:
	CMarkerFinder();
	virtual ~CMarkerFinder();
	//��λ��������;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	static bool LocateMarkerArea(Mat srcImg, vector<Rect> &vecMarkerRect);
	static bool LocateMarker_Fine(Mat srcImg, Mat templateImg, vector<Rect> vecMarkerAreaRect,
		                          vector<Rect> &vecMarkerLoc);
	static bool LocateTemplate(Mat srcImg, Mat tempImg, int nMaxCount, 
		                          vector<Rect> & vecTempRect);

private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
};

