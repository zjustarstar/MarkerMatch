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
	//pattern��λ����;
	int  locpattern_bCheckLastNum; //���һ�����ֵ��ٴ���֤;
	int  locpattern_bVerticalNum;  //�Ƿ��Ǵ�ֱ��������ְ��;
	float locpattern_fRatio;       //���һ�����ֵ�y�������;

	//finetune����;
	int finetune_nHcMargin;   //����finetune����,����ʮ�ֵ�margin;

	AlgParam() {
		locpattern_bCheckLastNum = 0; //Ĭ�ϲ������ٴ���֤;
		locpattern_bVerticalNum = 1;   //Ĭ����ֱ����;
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

	//һЩ�����õ��ĺ���;
	static int IsMoving(Mat preImg, Mat curImg);
	static bool DetAlignment(Mat srcImage, int &nThre, vector<Vec<int, 5>> & vecFound);
	static void GenerateBImg(Mat srcImg, Mat & bImg, GRADTYPE gt = GT_BOTH);

	bool Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern, AlgParam param);
	bool FinalFinetune(Mat srcImg, Mat &bImg, Rect &rectH, Rect &rectS);
	
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
	bool LocateTemplate(Mat srcImg, Mat tempImg, float fMatchThre, int nMaxCount,
		vector<LocMarker> & vecTempRect);

	//�ֱ����ڶ�λ����/�����е��ض�pattern��patternͼ��init����;
	bool LocatePattern(Mat srcImg, bool bHcPattern, int nMaxCount, vector<LocMarker> & vecFound);
	//��λmarker����λ��;
	bool LocateMarkerByTempMatch(Mat srcImg, bool bHollowCross, vector<Rect> vecMarkerAreaRect,
		float dMatchThre, vector<LocMarker> &vecMarkerLoc);
	//ͨ�ð��ʮ�ּ��;
	bool LocateCrossAreaByHog(Mat srcImg, double dHitThre, bool bHollowCross, vector<LocMarker> & vecFound);

private:
	static void   FindTextCord(Mat bImg, LocTexParam struLTParam, vector<Rect> & vecRect);
	//��֤��λ;
	bool CheckLocTemp_ByLastNum(Mat srcImg, Mat tempImg, float fMatchThre, Rect r);
	//�����׶εĶ�λ;
	Rect FT_LocSolidCross(Mat grayImg, Mat bImg, double dthre);
	Rect FT_LocHollyCross(Mat grayImg, Mat bImg, double dthre);
	bool FT_FindBoundary(Mat data, int & s, int & e);
	bool FT_FindBlackMargin(Mat srcImg, Rect &r);

private:
	HOGDescriptor m_hcHog;  //for hollowcross detector;
	HOGDescriptor m_scHog;  //for solidcross detector;

	
	//����ƥ��ͼ���,���ǻҶ�ͼ��
	Mat m_hcMarker;  //����hollow cross marker;
	Mat m_scMarker;  //����solid cross marker;
	Mat m_hcPattern; //����pattern;
	Mat m_scPattern; //����pattern;
};

