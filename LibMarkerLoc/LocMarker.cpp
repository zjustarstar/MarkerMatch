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

// ��ʼ���������,�Լ�����õĸ���ͼ��;
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcImg, ImgInfo scImg, 
	                                             ImgInfo hPatternImg, ImgInfo sPatternImg,
	                                             AlgorithParams param)
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

	//�㷨����;
	AlgParam ap;
	ap.locpattern_bCheckLastNum = param.locpattern_bCheckLastNum;
	ap.locpattern_bVerticalNum = param.locpattern_bVerticalNum;
	ap.locpattern_fRatio  = param.locpattern_fRatio;
	ap.finetune_nHcMargin = param.finetune_nHcMargin;

	if (!g_mf.Init(_hcImg, _scImg, hpImg, spImg,ap))
		return ERROR_FAIL_LOADPARM;

	return ERROR_NO;
}

//��⵽��pattern����;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {
	
	//if (k1 > 5000)
	//	return false;

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

//��⵽��ʮ��˿;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {

	//if (k2 > 5000)
	//	return false;

	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);

	double dHitThre;
	if (bHollowCross)
		dHitThre = -0.7;  //�����Ĳ���
	else
		dHitThre = -0.5;  //�����Ĳ���;

	//���ȼ�⵽��ʮ�ֵ�����;
	vector<LocMarker> vecMarkerArea;
	g_mf.LocateCrossAreaByHog(srcImg, dHitThre, bHollowCross, vecMarkerArea);

	//Ȼ�����ģ��ƥ��;
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

extern "C" _declspec(dllexport) bool IsMoving(ImgInfo preImg, ImgInfo curImg, int nThre, int * nCount) {

	if (preImg.nChannels != curImg.nChannels)
		return false;

	int nTypes = CV_8UC3;
	if (preImg.nChannels == 1)
		nTypes = CV_8UC1;
	Mat pImg(preImg.nH, preImg.nW, nTypes, preImg.pData, preImg.nStep);
	Mat cImg(curImg.nH, curImg.nW, nTypes, curImg.pData, curImg.nStep);

	Mat diff;
	absdiff(pImg, cImg, diff);

	Mat gray;
	if (diff.channels() == 3)
		cvtColor(diff, gray, CV_BGR2GRAY);
	else
		gray = diff;

	Mat  b;  //��ֵͼ;�̶���ֵ;
	//�ֵ�:nThre��Ϊ50��������nThre��Ϊ25�ȽϺ���;
	threshold(gray, b, nThre, 255, THRESH_BINARY);
	(*nCount) = countNonZero(b);

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

	//�����Сֵ;
	double minVal, maxVal;
	int    minIdx[2] = {}, maxIdx[2] = {};	// minnimum Index, maximum Index
	minMaxIdx(grayImg, &minVal, &maxVal, minIdx, maxIdx);
	(*nMax) = maxVal;

	//��ֵ;
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

extern "C" _declspec(dllexport) int FineTune(ImgInfo img, LocRect roiRect, LocRect * pHRect, LocRect * pSRect) {
	
	int nTypes = CV_8UC3;
	if (img.nChannels == 1)
		nTypes = CV_8UC1;

	Mat srcImg(img.nH, img.nW, nTypes, img.pData, img.nStep);
	int nImgW = srcImg.cols;
	int nImgH = srcImg.rows;

	//��������;
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
	Mat bImg;  //���ڲ��Ե�;
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