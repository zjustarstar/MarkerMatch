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
	int   nMarkerType;              //标记物类型; 0表示十字，1表示正方形;    

	//一些特殊工艺的参数;
	int   bFlag_AfterAL;           //铝后工艺，会有大量噪点;

	//cross粗调定位相关参数;
	int   loccross_nNum;           //粗调返回的cross个数阈值;
	float loccross_fHcThre;        //粗调时的虚十字框阈值;
	float loccross_fScThre;        //粗调时的实十字框阈值;

	//pattern定位函数;
	int   locpattern_bCheckLastNum; //最后一个数字的再次验证;
	int   locpattern_bVerticalNum;  //是否是垂直方向的数字版号;
	float locpattern_fRatio;        //最后一个数字的y坐标比率;

	int  locpattern_bTwoStageLoc;  //开启二阶段式的检测.仅用于左右片亮度差异太大,Delta值也无法补充时;
	                                //采用第一次检测到区域后，就把该区域抹掉，再进行第二次检测;
	int   locpattern_nScDelta;      //二值化时的补偿;用于左右片亮度差异大时。默认为0.
	int   locpattern_nHcDelta;       
	float locpattern_fScMatchDegree;  //匹配阈值。默认0.5;
	float locpattern_fHcMatchDegree;  //匹配阈值。默认0.5;

	//finetune函数;
	int finetune_nHcMargin;   //用于finetune函数,空心十字的margin;

	//用于精调refine位置时的参数;
	int refine_nScThickSize;        //精调refine时的实心十字bar厚度大小;
	int refine_nHcThickSize;        //精调refine时空心十字的厚度大小;

	//用于精调时的虚心十字框的精确定位;
	int refineHC_nMarginH;         //用于FT_RefineHollyCross函数的上下和左右扩展尺寸;
	int refineHC_nMarginV;         //默认都是6

	AlgParam() {
		nMarkerType = 0;              //默认十字;
		//涉及部分特殊工艺参数;
		bFlag_AfterAL = 0;

		//粗调时的参数;
		loccross_fScThre = -0.9;  //大部分情况下-0.5可以，该值太小了容易引入过多虚假目标;
		loccross_fHcThre = -0.7;

		locpattern_bCheckLastNum = 0; //默认不进行再次验证;
		locpattern_bVerticalNum = 1;   //默认竖直方向;
		locpattern_fRatio = 0.8;

		locpattern_nScDelta = 0;
		locpattern_nHcDelta = 0;
		locpattern_fScMatchDegree = 0.5;
		locpattern_fHcMatchDegree = 0.5;
		locpattern_bTwoStageLoc = 0;

		finetune_nHcMargin = 0;

		refine_nHcThickSize = 6;     //空心十字的厚度;
		refine_nScThickSize = 24;    //实心十字的bar厚度;

		refineHC_nMarginH = 6;
		refineHC_nMarginV = 6;
	}

};

class CMarkerFinder
{
public:
	AlgParam      m_algParams;

	CMarkerFinder();
	virtual ~CMarkerFinder();

	//一些其它用到的函数;
	static int IsMoving(Mat preImg, Mat curImg,int nThre);
	static bool DetAlignment(Mat srcImage, int &nThre, vector<Vec<int, 5>> & vecFound);
	static double GenerateBImg(Mat srcImg, Mat & bImg, GRADTYPE gt = GT_BOTH);

	bool Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern, AlgParam param);
	//十字标记物的微调;
	bool FinalFinetune(Mat srcImg, Mat &bImg, Rect &rectH, Rect &rectS);
	//方框标记物的微调;
	bool FinalFinetune_Rect(Mat srcImg, Mat &bImg, Rect &rectH, Rect &rectS);
	
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
	bool LocateTemplate(Mat srcImg, Mat tempImg, bool bHcPattern, float fMatchThre, int nMaxCount,
		vector<LocMarker> & vecTempRect);

	//分别用于定位明场/暗场中的特定pattern，pattern图由init传入;
	bool LocatePattern(Mat srcImg, bool bHcPattern, int nMaxCount, vector<LocMarker> & vecFound);
	//定位marker具体位置;
	bool LocateMarkerByTempMatch(Mat srcImg, bool bHollowCross, vector<Rect> vecMarkerAreaRect,
		float dMatchThre, vector<LocMarker> &vecMarkerLoc);
	//通用版的十字检测;
	bool LocateCrossAreaByHog(Mat srcImg, bool bHollowCross, vector<LocMarker> & vecFound);

	bool FT_RefineSolidCross(Mat grayImg, Rect &rectS);
	bool FT_RefineSolidRect(Mat grayImg, Rect &rectS);

	//test
	bool FT_RefineWhiteRect(Mat grayImg, Rect &rectW);

	//工具类;
	void Util_GenTempImg(Mat srcImg, int nDelta, Mat &outImg);
private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
	//验证定位;
	bool CheckLocTemp_ByLastNum(Mat srcImg, Mat tempImg, float fMatchThre, Rect r);
	//精调阶段的十字定位;
	Rect FT_LocSolidCross(Mat grayImg, Mat bImg, double dthre);
	Rect FT_LocHollyCross(Mat grayImg, Mat bImg, double dthre);
	bool FT_AdjustRect(Rect & rH, Rect &rS);
	//精调阶段的方框标记定位;
	Rect FT_LocSolidRect(Mat grayImg, Mat bImg, double dthre);
	Rect FT_LocHollyRect(Mat grayImg, Mat bImg, double dthre);

	bool FT_FindBoundary(Mat data, int & s, int & e);
	//十字marker:查找图像边缘的黑色区域;
	bool FT_FindBlackMargin(Mat srcImg, Rect &r);
	//正方形marker:查找图像两侧的白色竖条区域;
	Rect FT_FindWhiteMargin(Mat bImg);

	//十字标记精调阶段的定位微调;
	bool FT_RefineHollyCross(Mat grayImg, Rect &rectH);
	//方形标记精调阶段的定位微调;
	bool FT_RefineHollyRect(Mat grayImg, Rect &rectH);

	int FindThreByHist(Mat grayImg, float fThre);

private:
	HOGDescriptor m_hcHog;  //for hollowcross detector;
	HOGDescriptor m_scHog;  //for solidcross detector;

	//用于匹配和检测的,都是灰度图；
	Mat m_hcMarker;  //暗场hollow cross marker;
	Mat m_scMarker;  //明场solid cross marker;
	Mat m_hcPattern; //暗场pattern;
	Mat m_scPattern; //明场pattern;
};

