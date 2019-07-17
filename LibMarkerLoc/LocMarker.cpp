#include "LocMarker.h"
#include "..\\MarkerMatch\MarkerFinder.h"
#include <opencv2\opencv.hpp>

using namespace cv;

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif


// ��ʼ���������
extern "C" _declspec(dllexport) bool initDetector()
{
	return true;
}

//��⵽��pattern����;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, int * nSize, LocRect * pRect) {
	(*nSize) = 2;
	pRect[0].x = 10;
	pRect[0].y = 10;
	pRect[0].w = 20;
	pRect[0].h = 15;
	pRect[0].fConf = 0.7;

	pRect[1].x = 20;
	pRect[1].y = 20;
	pRect[1].w = 40;
	pRect[1].h = 45;
	pRect[1].fConf = 0.8;

	return true;
}

//��⵽��ʮ��˿;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect) {
	(*nSize) = 1;

	pRect[0].x = 50;
	pRect[0].y = 50;
	pRect[0].w = 120;
	pRect[0].h = 450;
	pRect[0].fConf = 0.92;

	Mat srcImg(img.nH, img.nW, CV_8UC3, img.pData, img.nStep);
	imwrite("d:\\xxxxx.jpg", srcImg);

	return true;
}