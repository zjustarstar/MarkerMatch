// MarkerMatch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MarkerFinder.h"
#include "Train.h"
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "..\\LibMarkerLoc\\LocMarker.h"

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
	CMarkerFinder::LocateTemplate(srcImg, tempImg, 0.4, 6, vecTempRect);
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

void DrawLocResult(Mat srcImg, Scalar color, vector<LocMarker> vecResult, bool bTextUp)
{
	//显示最终的marker区域;
	for (int j = 0; j < vecResult.size(); j++)
	{
		Rect k = vecResult[j].rect;
		rectangle(srcImg, k, color, 8);

		//显示maxvalue
		char msg[10];
		sprintf_s(msg, "%.2f", vecResult[j].fConfidence);

		//默认文字显示在框上;
		int y = (k.y - 10) > 0 ? (k.y - 10) : 0;
		if (!bTextUp)
		{
			y = (k.y + k.height + 50);
			if (y >= srcImg.rows)
				y = srcImg.rows - 1;
		}
		putText(srcImg, string(msg), cvPoint(k.x, y), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);
	}
}

void testTemMatch() {
	string strFile = "d:\\1.jpg";
	string strTempFile = "d:\\temp.bmp";
	//string strTempFile = "E:\\MyProject\\MarkerMatch\\template\\temp-2.jpg";
	Mat srcImg = imread(strFile);
	Mat tempImg = imread(strTempFile, 0);

	CMarkerFinder mf;
	mf.Init(tempImg, tempImg, tempImg, tempImg);
	vector<LocMarker> vecRect;
	mf.LocatePattern(srcImg, true, 2, vecRect);
	
	DrawLocResult(srcImg, Scalar(0, 0, 255), vecRect, true);


	namedWindow("srcimg", 0);
	resizeWindow("srcimg", 1368, 912);
	imshow("srcimg", srcImg);
}

void test_getbimg(Mat srcImg) {
	Mat aGray,b;
	cvtColor(srcImg, aGray, CV_BGR2GRAY);
	double dthre = threshold(aGray, b, 0, 255, THRESH_OTSU);
	//threshold(aGray, b, dthre + 10, 255, THRESH_BINARY);

	imwrite("d:\\binary.jpg", b);

	namedWindow("src");
	imshow("src", srcImg);
	namedWindow("b");
	imshow("b", b);
}

void test_Diff() {
	string strImg2 = "d:\\cur.jpg";
	string strImg1 = "d:\\pre.jpg";

	Mat m1, m2;
	m1 = imread(strImg1);
	m2 = imread(strImg2);

	int nCount = CMarkerFinder::IsMoving(m1, m2);
	cout << "nCount=" << nCount << endl;

	int d;
	cin >> d;
}

void test_black()  {
	string strImg1 = "d:\\1.jpg";

	Mat m1;
	m1 = imread(strImg1);

		Mat grayImg;
		if (m1.channels() == 1)
			grayImg = m1;
		else
			cvtColor(m1, grayImg, CV_BGR2GRAY);

		//最大最小值;
		double minVal, maxVal;
		int    minIdx[2] = {}, maxIdx[2] = {};	// minnimum Index, maximum Index
		minMaxIdx(grayImg, &minVal, &maxVal, minIdx, maxIdx);
		cout << "max=" << maxVal << endl;

		//均值;
		Scalar tempVal = cv::mean(grayImg);
		int matMean = (int)(tempVal.val[0]);
		cout << "mean=" << matMean << endl;

		int d;
		cin >> d;
}

int main()
{
	/*
	string stra = "E:\\MyProject\\MarkerMatch\\样本\\SolidHollowCross\\12111259_0.jpg";
	Mat a = imread(stra);
	//a = 255 - a;
	Mat b;
	Mat aGray;
	cvtColor(a, aGray, CV_BGR2GRAY);
	double dthre = threshold(aGray, b, 0, 255, THRESH_OTSU);
	float fRatio = countNonZero(b)*1.0 / (b.rows*b.cols*1.0);
	cout << "1: non-zero=" << fRatio << endl;
	namedWindow("1");
	imshow("1", b);

	threshold(aGray, b, dthre - 10, 255, THRESH_BINARY);
	fRatio = countNonZero(b)*1.0 / (b.rows*b.cols*1.0);
	cout << "2: non-zero=" << fRatio << endl;

	namedWindow("2");
	imshow("2", b);
	*/
	test_black();

	//startTrain();
	//startTest(srcImg);

	waitKey(0);

    return 0;
}

