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

// 初始化检测算子,以及检测用的各种图像;
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcImg, ImgInfo scImg, ImgInfo hPatternImg, ImgInfo sPatternImg)
{
	Mat _hcImg(hcImg.nH, hcImg.nW, CV_8UC3, hcImg.pData, hcImg.nStep);
	Mat _scImg(scImg.nH, scImg.nW, CV_8UC3, scImg.pData, scImg.nStep);

	if (_hcImg.empty() || _scImg.empty())
		return ERROR_EMPTY_MARKER;

	Mat hpImg;
	if (hPatternImg.pData && hPatternImg.nW>0 && hPatternImg.nH>0)
		hpImg = Mat::Mat(hPatternImg.nH, hPatternImg.nW, CV_8UC3, hPatternImg.pData, hPatternImg.nStep);
	else
		hpImg = Mat::Mat();

	Mat spImg;
	if (sPatternImg.pData && sPatternImg.nW>0 && sPatternImg.nH>0)
		spImg = Mat::Mat(sPatternImg.nH, sPatternImg.nW, CV_8UC3, sPatternImg.pData, sPatternImg.nStep);
	else
		spImg = Mat::Mat();

	if (!g_mf.Init(_hcImg, _scImg, hpImg, spImg))
		return ERROR_FAIL_LOADPARM;

	return ERROR_NO;
}

//检测到的pattern个数;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {
	
	Mat srcImg(img.nH, img.nW, CV_8UC3, img.pData, img.nStep);
	if (srcImg.empty() || img.nH <= 0 || img.nW <= 0)
		return false;

	vector<LocMarker> vecFound;
	bool bRet = g_mf.LocatePattern(srcImg, bHollowCross, 2, vecFound);
	(*nSize) = vecFound.size();
	for (int i = 0; i < vecFound.size(); i++)
	{
		pRect[i].x = vecFound[i].rect.x;
		pRect[i].y = vecFound[i].rect.y;
		pRect[i].h = vecFound[i].rect.height;
		pRect[i].w = vecFound[i].rect.width;
		pRect[i].fConf = vecFound[i].fConfidence;
	}

	return true;
}

//检测到的十字丝;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {
	/*(*nSize) = 1;

	pRect[0].x = 50;
	pRect[0].y = 50;
	pRect[0].w = 120;
	pRect[0].h = 450;
	pRect[0].fConf = 0.92;

	Mat srcImg(img.nH, img.nW, CV_8UC3, img.pData,img.nStep);
	imwrite("d:\\xxxxx.jpg", srcImg);
	*/
	Mat srcImg(img.nH, img.nW, CV_8UC3, img.pData, img.nStep);

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

	return true;
}