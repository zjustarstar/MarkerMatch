// MarkerMatch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MarkerFinder.h"
#include "Train.h"
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace std;
using namespace cv;

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif

int findmarker() {
	//string strSrcImg = "d:\\aa.jpg";
	string strSrcImg = "E:\\MyProject\\MarkerMatch\\A4和B1\\20190522164741972.jpg";
	string strTempImg = "E:\\MyProject\\MarkerMatch\\template\\temp_mask.jpg";
	//string strTempImg = "E:\\MyProject\\MarkerMatch\\template\\solidcross4_b.jpg";

	Mat srcImg = imread(strSrcImg);
	Mat tempImg = imread(strTempImg);

	clock_t s, e;
	vector<LocMarker> vecTempRect;
	s = clock();
	CMarkerFinder::LocateTemplate(srcImg, tempImg, 6, vecTempRect);
	e = clock();
	cout << "The LocTemp time is: " << (double)(e - s) / CLOCKS_PER_SEC * 1000 << "ms" << endl;

	for (int i = 0; i < vecTempRect.size(); i++)
	{
		Rect k = vecTempRect[i].rect;
		rectangle(srcImg, k, Scalar(255, 0, 255), 8);
		//显示maxvalue
		char msg[10];
		sprintf_s(msg, "%.2f", vecTempRect[i].fConfidence);
		putText(srcImg, string(msg), cvPoint(k.x, k.y - 10), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);
	}

	namedWindow("srcimg", 0);
	resizeWindow("srcimg", 1368, 912);
	imshow("srcimg", srcImg);

	waitKey(0);
}

void test() {
	startTrain();

}

int main()
{
	/*
	string stra = "E:\\MyProject\\MarkerMatch\\template\\nonSolidCross.jpg";
	Mat a = imread(stra);
	Mat b;
	Mat aGray;
	cvtColor(a, aGray, CV_BGR2GRAY);
	threshold(aGray, b, 0, 255, THRESH_OTSU);
	imwrite("d:\\nonSolidCross_b.jpg", b);
	*/

	string strFile = "d:\\A4和B1\\20190419153231344.jpg";
	Mat srcImg = imread(strFile);

	//startTrain();
	startTest(srcImg);

    return 0;
}

