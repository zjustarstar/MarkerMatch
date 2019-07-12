#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

//定位文字区域时的参数;
typedef struct LocTexParam{
	int nTextBandWThre; //文字宽度;
	int nStepCountThre; //出现的最简单字符个数(阶跃数);
	LocTexParam() {
		nTextBandWThre = 100;   //原图大小时的宽度;
		nStepCountThre = 8 * 2; //8个最简单的字符;
	}
};

//定位wafer maker(暗场)时的参数;
typedef struct LocWafterMarkerParam {
	int nDistToText;   //Marker右边缘和文字之间的距离;
	int nMarkerWidth;    //marker的宽度;
	LocWafterMarkerParam() {
		nMarkerWidth = 180;
	}
};

//定位Marker时的结果;
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
	//定位文字区域;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	//定位marker具体位置;
	static bool LocateMarker_Fine(Mat srcImg, Mat templateImg, vector<Rect> vecMarkerAreaRect,
		                          vector<LocMarker> &vecMarkerLoc);

	//查找明场下的marker区域，实心十字;
	static bool FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect);
	//查找暗场下的marker区域,空心十字;
	static bool FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp, 
		                      vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect);

	//测试用;
	static bool LocateTemplate(Mat srcImg, Mat tempImg, int nMaxCount,
		vector<Rect> & vecTempRect);

private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
};

