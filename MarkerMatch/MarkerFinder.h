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
	void Test();
	CMarkerFinder();
	virtual ~CMarkerFinder();
	bool Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern);
	void FinalFinetune(Mat srcImg, Mat &bImg);
	static void GenerateBImg(Mat srcImg, Mat & bImg);
	//////////////////////////基于文字定位的方案//////////////////////
	//定位文字区域;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	//查找明场下的marker区域，实心十字;
	static bool FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect);
	//查找暗场下的marker区域,空心十字;
	static bool FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp, 
		                      vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect);
	
	////////////////////////////通用检测方案///////////////////////////
	//测试用;
	static bool LocateTemplate(Mat srcImg, Mat tempImg, float fMatchThre, int nMaxCount,
		vector<LocMarker> & vecTempRect);

	//分别用于定位明场/暗场中的特定pattern，pattern图由init传入;
	bool LocatePattern(Mat srcImg, bool bHcPattern, int nMaxCount, vector<LocMarker> & vecFound);
	//定位marker具体位置;
	bool LocateMarkerByTempMatch(Mat srcImg, bool bHollowCross, vector<Rect> vecMarkerAreaRect,
		float dMatchThre, vector<LocMarker> &vecMarkerLoc);
	//通用版的十字检测;
	bool LocateCrossAreaByHog(Mat srcImg, double dHitThre, bool bHollowCross, vector<LocMarker> & vecFound);

private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
	

private:
	HOGDescriptor m_hcHog;  //for hollowcross detector;
	HOGDescriptor m_scHog;  //for solidcross detector;
	
	//用于匹配和检测的,都是灰度图；
	Mat m_hcMarker;  //暗场hollow cross marker;
	Mat m_scMarker;  //明场solid cross marker;
	Mat m_hcPattern; //暗场pattern;
	Mat m_scPattern; //明场pattern;
};

