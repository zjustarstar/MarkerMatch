#include "LocMarker.h"
#include "..\\MarkerMatch\MarkerFinder.h"
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace cv;

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif

CMarkerFinder g_mf;

bool checkTime() {
	time_t t;
	t = time(NULL);

	tm pTm;
	localtime_s(&pTm, &t);
	int nYear = pTm.tm_year + 1900;
	int nMonth = pTm.tm_mon + 1;

	if ((nYear > 2020) && (nMonth > 1))
		return false;
	else
		return true;
}

// 初始化检测算子,以及检测用的各种图像;
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcImg, ImgInfo scImg, 
	                                             ImgInfo hPatternImg, ImgInfo sPatternImg,
	                                             AlgorithParams param)
{
	//if (!checkTime())
	//	return ERROR_FAIL_LOADPARM; 

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

	//算法参数;
	AlgParam ap;
	ap.bFlag_AfterAL    = param.bFlag_AfterAL;
	ap.nMarkerType      = param.nMarkerType;
	ap.loccross_nNum    = param.loccross_nNum;
	ap.loccross_fHcThre = param.loccross_fHcThre;
	ap.loccross_fScThre = param.loccross_fScThre;

	ap.locpattern_bCheckLastNum = param.locpattern_bCheckLastNum;
	ap.locpattern_bVerticalNum = param.locpattern_bVerticalNum;
	ap.locpattern_fRatio  = param.locpattern_fRatio;
	ap.locpattern_bTwoStageLoc = param.locpattern_bTwoStageLoc;
	ap.locpattern_fScMatchDegree = param.locpattern_fScMatchDegree;
	ap.locpattern_fHcMatchDegree = param.locpattern_fHcMatchDegree;
	ap.locpattern_nScDelta = param.locpattern_nScDelta;
	ap.locpattern_nHcDelta = param.locpattern_nHcDelta;
	ap.locpattern_nScDelta_right = param.locpattern_nScDelta_right;
	ap.locpattern_nHcDelta_right = param.locpattern_nHcDelta_right;

	ap.finetune_nBlackBgThre = param.finetune_nBlackBgThre;
	ap.finetune_nHcMargin = param.finetune_nHcMargin;
	ap.refine_nHcThickSize = param.refine_nHcThickSize;
	ap.refine_nScThickSize = param.refine_nScThickSize;

	ap.refineHC_nMarginH = param.refineHC_nMarginH;
	ap.refineHC_nMarginV = param.refineHC_nMarginV;
	
	if (!g_mf.Init(_hcImg, _scImg, hpImg, spImg,ap))
		return ERROR_FAIL_LOADPARM;

	return ERROR_NO;
}

extern "C" _declspec(dllexport) bool LocatePattern_Seperate(ImgInfo img, int nPos, bool bHollowCross, int * nSize, LocRect * pRect)
{
	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat tempImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	if (tempImg.empty() || img.nH <= 0 || img.nW <= 0)
		return false;

	vector<LocMarker> vecFound;
	bool bRet = g_mf.LocatePattern_Seperate(tempImg,nPos,bHollowCross, 2, vecFound);
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

//检测到的pattern个数;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {
	

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

	return true;
}

bool MyCompare(LocMarker m1, LocMarker m2) {
	return (m1.fConfidence > m2.fConfidence);
}

//检测到的十字丝;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {

	//if (k2 > 5000)
	//	return false;

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	float fTempMatchThre = 0.3;

	//铝后图像，噪声很多，只能在亮场在定位空心十字，因此需要修改这几个参数;
	if (g_mf.m_algParams.bFlag_AfterAL) {
		g_mf.m_algParams.loccross_fHcThre = -0.95;
		fTempMatchThre = 0;
	}

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);

	//首先检测到有十字的区域;
	vector<LocMarker> vecMarkerArea;
	g_mf.LocateCrossAreaByHog(srcImg, bHollowCross, vecMarkerArea);

	//然后进行模板匹配;
	vector<Rect> vecMarkerRect;
	for (int i = 0; i < vecMarkerArea.size(); i++)
		vecMarkerRect.push_back(vecMarkerArea[i].rect);
	vector<LocMarker> vecFound;
	g_mf.LocateMarkerByTempMatch(srcImg, bHollowCross, vecMarkerRect, fTempMatchThre, vecFound);

	//对结果从大到小进行排序;
	sort(vecFound.begin(), vecFound.end(), MyCompare);

	//根据参数返回有限的数据;不能超过loccross_nNum;
	(*nSize) = vecFound.size();
	int n = vecFound.size();
	if (n >= g_mf.m_algParams.loccross_nNum)
		n = g_mf.m_algParams.loccross_nNum;
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

extern "C" _declspec(dllexport) bool IsMoving(ImgInfo preImg, ImgInfo curImg, int nThre, int * nCount) {

	if (preImg.nChannels != curImg.nChannels)
		return false;

	int nTypes = CV_8UC3;
	if (preImg.nChannels == 1)
		nTypes = CV_8UC1;
	Mat pImg(preImg.nH, preImg.nW, nTypes, preImg.pData, preImg.nStep);
	Mat cImg(curImg.nH, curImg.nW, nTypes, curImg.pData, curImg.nStep);

	(*nCount) = CMarkerFinder::IsMoving(pImg, cImg, nThre);

	return true;
}

extern "C" _declspec(dllexport) bool IsMoving_Finetune(ImgInfo preImg, ImgInfo curImg, LocRect r1, LocRect r2, int nThre, int * nCount)
{
	if (preImg.nChannels != curImg.nChannels)
		return false;

	int nTypes = CV_8UC3;
	if (preImg.nChannels == 1)
		nTypes = CV_8UC1;
	Mat pImg(preImg.nH, preImg.nW, nTypes, preImg.pData, preImg.nStep);
	Mat cImg(curImg.nH, curImg.nW, nTypes, curImg.pData, curImg.nStep);

	//r1的比对;
	Rect rect1,rect2;
	rect1.x = r1.x;
	rect1.y = r1.y;
	rect1.width = r1.w;
	rect1.height = r1.h;

	Mat pImg1 = pImg(rect1);
	Mat cImg1 = cImg(rect1);

	int n1,n2;
	n1 = CMarkerFinder::IsMoving(pImg1, cImg1, nThre);

	//r2的比对;
	rect2.x = r2.x;
	rect2.y = r2.y;
	rect2.width = r2.w;
	rect2.height = r2.h;
	Mat pImg2 = pImg(rect2);
	Mat cImg2 = cImg(rect2);
	n2 = CMarkerFinder::IsMoving(pImg2, cImg2, nThre);

	(*nCount) = (n1 > n2) ? n1 : n2;

	return true;
}

extern "C" _declspec(dllexport) bool IsBlack(ImgInfo img, int * nMean, int * nMax) {

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat tempImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	if (tempImg.empty() || img.nH <= 0 || img.nW <= 0)
		return false;

	Mat grayImg;
	if (img.nChannels == 1)
		grayImg = tempImg;
	else
		cvtColor(tempImg, grayImg, CV_BGR2GRAY);

	//最大最小值;
	double minVal, maxVal;
	int    minIdx[2] = {}, maxIdx[2] = {};	// minnimum Index, maximum Index
	minMaxIdx(grayImg, &minVal, &maxVal, minIdx, maxIdx);
	(*nMax) = maxVal;

	//均值;
   Scalar tempVal = cv::mean(grayImg);
	float matMean = tempVal.val[0];
	(*nMean) = (int)(matMean);
}

extern "C" _declspec(dllexport) int FindAlignment(ImgInfo img, int nThre, int * nSize, MyLines * pLines) {

	Mat srcImg(img.nH, img.nW, CV_8UC3, img.pData, img.nStep);

	vector<Vec<int, 5>> vecFound;
	int nThreshold = nThre;
	CMarkerFinder::DetAlignment(srcImg, nThreshold, vecFound);
	
	(*nSize) = vecFound.size();
	for (int i = 0; i < vecFound.size(); i++)
	{
		pLines[i].x1 = vecFound[i].val[0];
		pLines[i].y1 = vecFound[i].val[1];
		pLines[i].x2 = vecFound[i].val[2];
		pLines[i].y2 = vecFound[i].val[3];
	}

	return nThreshold;
}

//cross maker的精调;
extern "C" _declspec(dllexport) int FineTune(ImgInfo img, LocRect roiRect, LocRect * pHRect, LocRect * pSRect) {
	
	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	int nImgW = srcImg.cols;
	int nImgH = srcImg.rows;

	//参数查验;
	if (roiRect.x<0 || roiRect.x>nImgW - 1 || 
		roiRect.y<0 || roiRect.y>nImgH - 1 || 
		roiRect.w>nImgW || roiRect.h>nImgH)
		return -1;

	Rect r;
	r.x = roiRect.x;
	r.y = roiRect.y;
	r.width = roiRect.w;
	r.height = roiRect.h;

	Mat roiImg = srcImg(r);
	Mat bImg;  //用于测试的;
	Rect rH, rS;
	if (!g_mf.FinalFinetune(roiImg, bImg, rH, rS))
		return -2;

	pHRect->x = rH.x;
	pHRect->y = rH.y;
	pHRect->w = rH.width;
	pHRect->h = rH.height;

	pSRect->x = rS.x;
	pSRect->y = rS.y;
	pSRect->w = rS.width;
	pSRect->h = rS.height;

	return 0;
}

//Rect marker的精调函数;
extern "C" _declspec(dllexport) int FineTune_Rect(ImgInfo img, LocRect roiRect, LocRect * pHRect, LocRect * pSRect) {

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	int nImgW = srcImg.cols;
	int nImgH = srcImg.rows;

	//参数查验;
	if (roiRect.x<0 || roiRect.x>nImgW - 1 ||
		roiRect.y<0 || roiRect.y>nImgH - 1 ||
		roiRect.w > nImgW || roiRect.h > nImgH)
		return -1;

	Rect r;
	r.x = roiRect.x;
	r.y = roiRect.y;
	r.width = roiRect.w;
	r.height = roiRect.h;

	Mat roiImg = srcImg(r);
	Mat bImg;  //用于测试的;
	Rect rH, rS;
	if (!g_mf.FinalFinetune_Rect(roiImg, bImg, rH, rS))
		return -2;

	pHRect->x = rH.x;
	pHRect->y = rH.y;
	pHRect->w = rH.width;
	pHRect->h = rH.height;

	pSRect->x = rS.x;
	pSRect->y = rS.y;
	pSRect->w = rS.width;
	pSRect->h = rS.height;

	return 0;
}

//solid cross marker的第二阶段精调;
extern "C" _declspec(dllexport) void FineTune_RefineSRect(ImgInfo img, LocRect roiRect, LocRect * pSRect) {

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	int nImgW = srcImg.cols;
	int nImgH = srcImg.rows;

	Rect r;
	r.x = roiRect.x;
	r.y = roiRect.y;
	r.width = roiRect.w;
	r.height = roiRect.h;
	Mat roiImg = srcImg(r);

	//原来的SolidCross值;
	Rect oriSRect;
	oriSRect.x = pSRect->x;
	oriSRect.y = pSRect->y;
	oriSRect.width = pSRect->w;
	oriSRect.height = pSRect->h;

	Rect rS;
	g_mf.FT_RefineSolidCross(roiImg, oriSRect);
	pSRect->x = oriSRect.x;
	pSRect->y = oriSRect.y;
	pSRect->w = oriSRect.width;
	pSRect->h = oriSRect.height;
}

//solid rect marker的第二阶段精调;
extern "C" _declspec(dllexport) void FineTune_RefineSRect2(ImgInfo img, LocRect roiRect, LocRect * pSRect) {

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	int nImgW = srcImg.cols;
	int nImgH = srcImg.rows;

	Rect r;
	r.x = roiRect.x;
	r.y = roiRect.y;
	r.width = roiRect.w;
	r.height = roiRect.h;
	Mat roiImg = srcImg(r);

	//原来的SolidCross值;
	Rect oriSRect;
	oriSRect.x = pSRect->x;
	oriSRect.y = pSRect->y;
	oriSRect.width = pSRect->w;
	oriSRect.height = pSRect->h;

	Rect rS;
	g_mf.FT_RefineSolidRect(roiImg, oriSRect);
	pSRect->x = oriSRect.x;
	pSRect->y = oriSRect.y;
	pSRect->w = oriSRect.width;
	pSRect->h = oriSRect.height;
}

extern "C" _declspec(dllexport) void Util_GenerateTempFile(ImgInfo img, int nDelta, char * chFileName) {
	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);

	Mat out = srcImg;
	g_mf.Util_GenTempImg(srcImg, nDelta, out);
	string strFile(chFileName);
	imwrite(chFileName, out);
}