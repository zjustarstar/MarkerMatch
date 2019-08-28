#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

typedef enum GRADTYPE {
	GT_X,
	GT_Y,
	GT_BOTH
};

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

typedef struct AlgParam {
	//pattern定位函数;
	int  locpattern_bCheckLastNum; //最后一个数字的再次验证;
	int  locpattern_bVerticalNum;  //是否是垂直方向的数字版号;
	float locpattern_fRatio;       //最后一个数字的y坐标比率;

	//finetune函数;
	int finetune_nHcMargin;   //用于finetune函数,空心十字的margin;

	AlgParam() {
		locpattern_bCheckLastNum = 0; //默认不进行再次验证;
		locpattern_bVerticalNum = 1;   //默认竖直方向;
		locpattern_fRatio = 0.8;

		finetune_nHcMargin = 0;
	}

};

class CMarkerFinder
{
public:
	AlgParam      m_algParams;

	void Test();
	CMarkerFinder();
	virtual ~CMarkerFinder();

	//一些其它用到的函数;
	static int IsMoving(Mat preImg, Mat curImg);
	static bool DetAlignment(Mat srcImage, int &nThre, vector<Vec<int, 5>> & vecFound);
	static void GenerateBImg(Mat srcImg, Mat & bImg, GRADTYPE gt = GT_BOTH);

	bool Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern, AlgParam param);
	bool FinalFinetune(Mat srcImg, Mat &bImg, Rect &rectH, Rect &rectS);
	
	//////////////////////////基于文字定位的方案//////////////////////
	//定位文字区域;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	//查找明场下的marker区域，实心十字;
	static bool FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect);
	//查找暗场下的marker区域,空心十字;
	static bool FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp, 
		                      vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect);
	
	////////////////////////////通用检测方案///////////////////////////
	//模板匹配;
	bool LocateTemplate(Mat srcImg, Mat tempImg, float fMatchThre, int nMaxCount,
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
	//验证定位;
	bool CheckLocTemp_ByLastNum(Mat srcImg, Mat tempImg, float fMatchThre, Rect r);
	//精调阶段的定位;
	Rect FT_LocSolidCross(Mat grayImg, Mat bImg, double dthre);
	Rect FT_LocHollyCross(Mat grayImg, Mat bImg, double dthre);
	bool FT_FindBoundary(Mat data, int & s, int & e);
	bool FT_FindBlackMargin(Mat srcImg, Rect &r);

private:
	HOGDescriptor m_hcHog;  //for hollowcross detector;
	HOGDescriptor m_scHog;  //for solidcross detector;

	
	//用于匹配和检测的,都是灰度图；
	Mat m_hcMarker;  //暗场hollow cross marker;
	Mat m_scMarker;  //明场solid cross marker;
	Mat m_hcPattern; //暗场pattern;
	Mat m_scPattern; //明场pattern;
};

