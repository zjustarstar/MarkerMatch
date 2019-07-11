#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

typedef struct LocTexParam{
	int nTextBandWThre; //文字宽度;
	int nStepCountThre; //出现的最简单字符个数(阶跃数);
	LocTexParam() {
		nTextBandWThre = 100;   //原图大小时的宽度;
		nStepCountThre = 8 * 2; //8个最简单的字符;
	}
};

class CMarkerFinder
{
public:
	CMarkerFinder();
	virtual ~CMarkerFinder();
	//定位文字区域;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	static bool LocateMarkerArea(Mat srcImg, vector<Rect> &vecMarkerRect);
	static bool LocateMarker_Fine(Mat srcImg, Mat templateImg, vector<Rect> vecMarkerAreaRect,
		                          vector<Rect> &vecMarkerLoc);
	static bool LocateTemplate(Mat srcImg, Mat tempImg, int nMaxCount, 
		                          vector<Rect> & vecTempRect);

private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
};

