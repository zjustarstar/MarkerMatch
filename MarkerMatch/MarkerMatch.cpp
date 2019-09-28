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


void testDirection() {
	//string strSrcImg = "E:\\MyProject\\MarkerMatch\\数据\\倾斜\\20190814162129447.jpg";
	string strSrcImg = "E:\\MyProject\\MarkerMatch\\A4和B1\\20190522165103049.jpg";
	Mat srcImage = imread(strSrcImg);

	int nThre = 250;
	vector<Vec<int, 5>> vecFinal;
	CMarkerFinder::DetAlignment(srcImage, nThre, vecFinal);
	cout << "thre=" << nThre << endl;
	for (size_t i = 0; i < vecFinal.size(); i++)
	{
		Vec<int,5> l = vecFinal[i];
		Point pt1, pt2;
		pt1.x = l[0];
		pt1.y = l[1];
		pt2.x = l[2];
		pt2.y = l[3];

		Scalar clr;
		if ((pt1.x == pt2.x) || (pt1.y == pt2.y)) //直线
			clr = Scalar(255, 0, 0);
		else
			clr = Scalar(0, 0, 255);
		line(srcImage, pt1, pt2, clr, 4, LINE_AA); //Scalar函数用于调节线段颜色

		cout << "Line " << i << ": x1=" << pt1.x << ", x2=" << pt2.x << ": y1=" << pt1.y << ", y2=" << pt2.y << endl;
	}
	imwrite("d:\\final.jpg", srcImage);
	
	/*
	vector<Vec4i> lines;
	//与HoughLines不同的是，HoughLinesP得到lines的是含有直线上点的坐标的，所以下面进行划线时就不再需要自己求出两个点来确定唯一的直线了
	HoughLinesP(BImg, lines, 1, CV_PI / 250, 100, 200, 80);//注意第五个参数，为阈值

	//依次画出每条线段
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i] * nScale;

		line(srcImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 4, LINE_AA); //Scalar函数用于调节线段颜色

		cout << "Line " << i << ": x1=" << l[0] << ", x2=" << l[2] << endl;
	}*/

	namedWindow("dst", 0);
	resizeWindow("dst", 1368, 912);
	imshow("dst", srcImage);

}

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
	CMarkerFinder mf;
	mf.LocateTemplate(srcImg, tempImg, 0.4, 6, vecTempRect);
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
	string strFile = "E:\\0PE测试\\P12106暗场图片\\1.jpg";
	string strTempFile = "d:\\13.jpg";
	//string strTempFile = "E:\\MyProject\\MarkerMatch\\template\\temp-2.jpg";
	Mat srcImg2;
	Mat srcImg = imread(strFile);
	Mat tempImg = imread(strTempFile, 0);
	srcImg2 = srcImg.clone();

	CMarkerFinder mf;
	AlgParam ap;
	ap.locpattern_bCheckLastNum = false;
	ap.locpattern_bVerticalNum = true;
	ap.locpattern_fRatio = 0.8;
	ap.locpattern_nDelta = 20;
	mf.Init(tempImg, tempImg, tempImg, tempImg,ap);
	vector<LocMarker> vecRect;
	mf.LocatePattern(srcImg, true, 2, vecRect);
	
	DrawLocResult(srcImg, Scalar(0, 0, 255), vecRect, true);

	namedWindow("srcimg", 0);
	resizeWindow("srcimg", 1368, 912);
	imshow("srcimg", srcImg);

	//测试;
	/*Mat srcBImg, tempBImg;
	mf.GenerateBImg(srcImg2, srcBImg);
	mf.GenerateBImg(tempImg, tempBImg);
	imwrite("d:\\srcBImg.jpg", srcBImg);
	imwrite("d:\\tempBImg.jpg", tempBImg);

	CMarkerFinder mf2;
	mf2.Init(tempBImg, tempBImg, tempBImg, tempBImg);
	vecRect.clear();
	mf2.LocatePattern(srcBImg, true, 2, vecRect);

	DrawLocResult(srcImg2, Scalar(0, 0, 255), vecRect, true);

	namedWindow("srcBimg", 0);
	resizeWindow("srcBimg", 1368, 912);
	imshow("srcBimg", srcImg2);
	*/
}

void test_getbimg(Mat srcImg) {
	Mat aGray,b;
	cvtColor(srcImg, aGray, CV_BGR2GRAY);
	double dthre = threshold(aGray, b, 0, 255, THRESH_OTSU);
	threshold(aGray, b, dthre + 35, 255, THRESH_BINARY);

	imwrite("d:\\binary.jpg", b);

	namedWindow("src");
	imshow("src", srcImg);
	namedWindow("b");
	imshow("b", b);
}

void test_Diff() {
	string strImg2 = "d:\\IsMoving\\20190828165740494.jpg";  
	string strImg1 = "d:\\IsMoving\\20190828165734575.jpg";

	Rect r;
	r.x = 1500;//2720;//1500;
	r.y = 2300;//1410;//2300;
	r.width = 300;
	r.height = 300;

	Mat m1, m2;
	m1 = imread(strImg1);
	m2 = imread(strImg2);
	m1 = m1(r);
	m2 = m2(r);

	int nCount;
	nCount = CMarkerFinder::IsMoving(m1, m2, 25);
	cout << "nCount=" << nCount << endl;

	Mat diff;
	absdiff(m1, m2, diff);

	Mat gray;
	if (diff.channels() == 3)
		cvtColor(diff, gray, CV_BGR2GRAY);
	else
		gray = diff;

	int nCountNonZero = countNonZero(gray);
	float dRatio = nCountNonZero * 1.0 / (gray.rows * gray.cols);
	cout << "NonZero-Ratio=" << dRatio << endl;

	double MaxValue, MinValue;
	cv::minMaxLoc(gray, &MinValue, &MaxValue);
	cout << "max of diff:" << MaxValue << endl;

	namedWindow("src");
	imshow("src", m1);

	waitKey(0);
}

int main()
{
	clock_t s, e;
	s = clock();
	/*
	Mat src = imread("d:\\88.jpg");
	Mat bimg;
	CMarkerFinder::GenerateBImg(src, bimg);
	imwrite("d:\\xs.jpg", src);

	namedWindow("bimg");
	imshow("bimg", bimg);
	namedWindow("src");
	imshow("src", src);
	*/
	//test_black();
	testTemMatch();
	//testDirection();
	//startTrain();
	//startTest(srcImg);
	//test_AddArrow();

	e = clock();
	double dTime = (double)(e - s) / CLOCKS_PER_SEC * 1000;
	cout << "耗时:" << dTime << "ms" << endl;
	waitKey(0);

    return 0;
}

