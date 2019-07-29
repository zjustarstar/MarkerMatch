#include "LocMarker.h"
#include "..\\MarkerMatch\MarkerFinder.h"
#include <opencv2\opencv.hpp>

using namespace cv;

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif

CMarkerFinder g_mf;
static int k1, k2;

// 初始化检测算子,以及检测用的各种图像;
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcImg, ImgInfo scImg, ImgInfo hPatternImg, ImgInfo sPatternImg)
{
	int nType = CV_8UC3;
	if (hcImg.nChannels == 1)
		nType = CV_8UC1;
	Mat _hcImg(hcImg.nH, hcImg.nW, nType, hcImg.pData, hcImg.nStep);

	nType = CV_8UC3;
	if (scImg.nChannels == 1)
		nType = CV_8UC1;
	Mat _scImg(scImg.nH, scImg.nW, nType, scImg.pData, scImg.nStep);

	if (_hcImg.empty() || _scImg.empty())
		return ERROR_EMPTY_MARKER;

	Mat hpImg;
	if (hPatternImg.pData && hPatternImg.nW>0 && hPatternImg.nH>0)
	{
		nType = CV_8UC3;
		if (hPatternImg.nChannels == 1)
			nType = CV_8UC1;
		hpImg = Mat::Mat(hPatternImg.nH, hPatternImg.nW, nType, hPatternImg.pData, hPatternImg.nStep);
	}
	else
		hpImg = Mat::Mat();

	Mat spImg;
	if (sPatternImg.pData && sPatternImg.nW > 0 && sPatternImg.nH > 0)
	{
		nType = CV_8UC3;
		if (sPatternImg.nChannels == 1)
			nType = CV_8UC1;
		spImg = Mat::Mat(sPatternImg.nH, sPatternImg.nW, CV_8UC3, sPatternImg.pData, sPatternImg.nStep);
	}
	else
		spImg = Mat::Mat();

	if (!g_mf.Init(_hcImg, _scImg, hpImg, spImg))
		return ERROR_FAIL_LOADPARM;

	k1 = 0;
	k2 = 0;

	return ERROR_NO;
}

//检测到的pattern个数;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {
	
	if (k1 > 5000)
		return false;

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat tempImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	if (tempImg.empty() || img.nH <= 0 || img.nW <= 0)
		return false;

	vector<LocMarker> vecFound;
	bool bRet = g_mf.LocatePattern(tempImg, bHollowCross, 2, vecFound);
	(*nSize) = vecFound.size();
	for (int i = 0; i < vecFound.size(); i++)
	{
		pRect[i].x = vecFound[i].rect.x;
		pRect[i].y = vecFound[i].rect.y;
		pRect[i].h = vecFound[i].rect.height;
		pRect[i].w = vecFound[i].rect.width;
		pRect[i].fConf = vecFound[i].fConfidence;
	}

	k1++;

	return true;
}

//检测到的十字丝;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {

	if (k2 > 5000)
		return false;

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);

	double dHitThre;
	if (bHollowCross)
		dHitThre = -0.7;  //暗场的参数
	else
		dHitThre = -0.5;  //明场的参数;

	//首先检测到有十字的区域;
	vector<LocMarker> vecMarkerArea;
	g_mf.LocateCrossAreaByHog(srcImg, dHitThre, bHollowCross, vecMarkerArea);

	//然后进行模板匹配;
	vector<Rect> vecMarkerRect;
	for (int i = 0; i < vecMarkerArea.size(); i++)
		vecMarkerRect.push_back(vecMarkerArea[i].rect);
	vector<LocMarker> vecFound;
	g_mf.LocateMarkerByTempMatch(srcImg, bHollowCross, vecMarkerRect, 0.3, vecFound);

	int n = vecFound.size();
	(*nSize) = n;
	for (int i = 0; i < n; i++)
	{
		pRect[i].x = vecFound[i].rect.x;
		pRect[i].y = vecFound[i].rect.y;
		pRect[i].h = vecFound[i].rect.height;
		pRect[i].w = vecFound[i].rect.width;
		pRect[i].fConf = vecFound[i].fConfidence;
	}

	k2++;
	
	return true;
}