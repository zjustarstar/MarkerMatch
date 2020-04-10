#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

typedef enum GRADTYPE {
	GT_X,
	GT_Y,
	GT_BOTH
};

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

typedef struct AlgParam {
	int   nMarkerType;              //���������; 0��ʾʮ�֣�1��ʾ������;    

	//һЩ���⹤�յĲ���;
	int   bFlag_AfterAL;           //�����գ����д������;

	//cross�ֵ���λ��ز���;
	int   loccross_nNum;           //�ֵ����ص�cross������ֵ;
	float loccross_fHcThre;        //�ֵ�ʱ����ʮ�ֿ���ֵ;
	float loccross_fScThre;        //�ֵ�ʱ��ʵʮ�ֿ���ֵ;

	//pattern��λ����;
	int   locpattern_bCheckLastNum; //���һ�����ֵ��ٴ���֤;
	int   locpattern_bVerticalNum;  //�Ƿ��Ǵ�ֱ��������ְ��;
	float locpattern_fRatio;        //���һ�����ֵ�y�������;

	int  locpattern_bTwoStageLoc;  //�������׶�ʽ�ļ��.����������Ƭ���Ȳ���̫��,DeltaֵҲ�޷�����ʱ;
	                                //���õ�һ�μ�⵽����󣬾ͰѸ�����Ĩ�����ٽ��еڶ��μ��;
	int   locpattern_nScDelta;      //��ֵ��ʱ�Ĳ���;��������Ƭ���Ȳ����ʱ��Ĭ��Ϊ0.
	int   locpattern_nHcDelta;       
	float locpattern_fScMatchDegree;  //ƥ����ֵ��Ĭ��0.5;
	float locpattern_fHcMatchDegree;  //ƥ����ֵ��Ĭ��0.5;

	//finetune����;
	int finetune_nHcMargin;   //����finetune����,����ʮ�ֵ�margin;

	//���ھ���refineλ��ʱ�Ĳ���;
	int refine_nScThickSize;        //����refineʱ��ʵ��ʮ��bar��ȴ�С;
	int refine_nHcThickSize;        //����refineʱ����ʮ�ֵĺ�ȴ�С;

	//���ھ���ʱ������ʮ�ֿ�ľ�ȷ��λ;
	int refineHC_nMarginH;         //����FT_RefineHollyCross���������º�������չ�ߴ�;
	int refineHC_nMarginV;         //Ĭ�϶���6

	AlgParam() {
		nMarkerType = 0;              //Ĭ��ʮ��;
		//�漰�������⹤�ղ���;
		bFlag_AfterAL = 0;

		//�ֵ�ʱ�Ĳ���;
		loccross_fScThre = -0.9;  //�󲿷������-0.5���ԣ���ֵ̫С����������������Ŀ��;
		loccross_fHcThre = -0.7;

		locpattern_bCheckLastNum = 0; //Ĭ�ϲ������ٴ���֤;
		locpattern_bVerticalNum = 1;   //Ĭ����ֱ����;
		locpattern_fRatio = 0.8;

		locpattern_nScDelta = 0;
		locpattern_nHcDelta = 0;
		locpattern_fScMatchDegree = 0.5;
		locpattern_fHcMatchDegree = 0.5;
		locpattern_bTwoStageLoc = 0;

		finetune_nHcMargin = 0;

		refine_nHcThickSize = 6;     //����ʮ�ֵĺ��;
		refine_nScThickSize = 24;    //ʵ��ʮ�ֵ�bar���;

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

	//һЩ�����õ��ĺ���;
	static int IsMoving(Mat preImg, Mat curImg,int nThre);
	static bool DetAlignment(Mat srcImage, int &nThre, vector<Vec<int, 5>> & vecFound);
	static double GenerateBImg(Mat srcImg, Mat & bImg, GRADTYPE gt = GT_BOTH);

	bool Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern, AlgParam param);
	//ʮ�ֱ�����΢��;
	bool FinalFinetune(Mat srcImg, Mat &bImg, Rect &rectH, Rect &rectS);
	//���������΢��;
	bool FinalFinetune_Rect(Mat srcImg, Mat &bImg, Rect &rectH, Rect &rectS);
	
	//////////////////////////�������ֶ�λ�ķ���//////////////////////
	//��λ��������;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	//���������µ�marker����ʵ��ʮ��;
	static bool FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect);
	//���Ұ����µ�marker����,����ʮ��;
	static bool FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp, 
		                      vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect);
	
	////////////////////////////ͨ�ü�ⷽ��///////////////////////////
	//ģ��ƥ��;
	bool LocateTemplate(Mat srcImg, Mat tempImg, bool bHcPattern, float fMatchThre, int nMaxCount,
		vector<LocMarker> & vecTempRect);

	//�ֱ����ڶ�λ����/�����е��ض�pattern��patternͼ��init����;
	bool LocatePattern(Mat srcImg, bool bHcPattern, int nMaxCount, vector<LocMarker> & vecFound);
	//��λmarker����λ��;
	bool LocateMarkerByTempMatch(Mat srcImg, bool bHollowCross, vector<Rect> vecMarkerAreaRect,
		float dMatchThre, vector<LocMarker> &vecMarkerLoc);
	//ͨ�ð��ʮ�ּ��;
	bool LocateCrossAreaByHog(Mat srcImg, bool bHollowCross, vector<LocMarker> & vecFound);

	bool FT_RefineSolidCross(Mat grayImg, Rect &rectS);
	bool FT_RefineSolidRect(Mat grayImg, Rect &rectS);

	//test
	bool FT_RefineWhiteRect(Mat grayImg, Rect &rectW);

	//������;
	void Util_GenTempImg(Mat srcImg, int nDelta, Mat &outImg);
private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
	//��֤��λ;
	bool CheckLocTemp_ByLastNum(Mat srcImg, Mat tempImg, float fMatchThre, Rect r);
	//�����׶ε�ʮ�ֶ�λ;
	Rect FT_LocSolidCross(Mat grayImg, Mat bImg, double dthre);
	Rect FT_LocHollyCross(Mat grayImg, Mat bImg, double dthre);
	bool FT_AdjustRect(Rect & rH, Rect &rS);
	//�����׶εķ����Ƕ�λ;
	Rect FT_LocSolidRect(Mat grayImg, Mat bImg, double dthre);
	Rect FT_LocHollyRect(Mat grayImg, Mat bImg, double dthre);

	bool FT_FindBoundary(Mat data, int & s, int & e);
	//ʮ��marker:����ͼ���Ե�ĺ�ɫ����;
	bool FT_FindBlackMargin(Mat srcImg, Rect &r);
	//������marker:����ͼ������İ�ɫ��������;
	Rect FT_FindWhiteMargin(Mat bImg);

	//ʮ�ֱ�Ǿ����׶εĶ�λ΢��;
	bool FT_RefineHollyCross(Mat grayImg, Rect &rectH);
	//���α�Ǿ����׶εĶ�λ΢��;
	bool FT_RefineHollyRect(Mat grayImg, Rect &rectH);

	int FindThreByHist(Mat grayImg, float fThre);

private:
	HOGDescriptor m_hcHog;  //for hollowcross detector;
	HOGDescriptor m_scHog;  //for solidcross detector;

	//����ƥ��ͼ���,���ǻҶ�ͼ��
	Mat m_hcMarker;  //����hollow cross marker;
	Mat m_scMarker;  //����solid cross marker;
	Mat m_hcPattern; //����pattern;
	Mat m_scPattern; //����pattern;
};

