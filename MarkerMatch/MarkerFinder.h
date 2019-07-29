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
	void Test();
	CMarkerFinder();
	virtual ~CMarkerFinder();
	bool Init(Mat hcMarker, Mat scMarker, Mat hcPattern, Mat scPattern);
	void FinalFinetune(Mat srcImg, Mat &bImg);
	static void GenerateBImg(Mat srcImg, Mat & bImg);
	//////////////////////////�������ֶ�λ�ķ���//////////////////////
	//��λ��������;
	static bool LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc);
	//���������µ�marker����ʵ��ʮ��;
	static bool FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect);
	//���Ұ����µ�marker����,����ʮ��;
	static bool FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp, 
		                      vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect);
	
	////////////////////////////ͨ�ü�ⷽ��///////////////////////////
	//������;
	static bool LocateTemplate(Mat srcImg, Mat tempImg, float fMatchThre, int nMaxCount,
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
	

private:
	HOGDescriptor m_hcHog;  //for hollowcross detector;
	HOGDescriptor m_scHog;  //for solidcross detector;
	
	//����ƥ��ͼ���,���ǻҶ�ͼ��
	Mat m_hcMarker;  //����hollow cross marker;
	Mat m_scMarker;  //����solid cross marker;
	Mat m_hcPattern; //����pattern;
	Mat m_scPattern; //����pattern;
};

