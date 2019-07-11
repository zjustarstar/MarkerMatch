// MarkerMatch.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MarkerFinder.h"
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace std;
using namespace cv;

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif


int main()
{
	/*
	string strSolidCross = "E:\\MyProject\\MarkerMatch\\template\\SolidCross4.jpg";
	Mat scImg = imread(strSolidCross);
	Mat scGrayImg;
	cvtColor(scImg, scGrayImg, CV_BGR2GRAY);
	Mat scBiImg;
	threshold(scGrayImg, scBiImg, 0, 255, THRESH_OTSU);
	imwrite("d:\\solidcross4_b.jpg", scBiImg);
	*/
	//E:\MyProject\MarkerMatch\����\6.11����\5.12-13����\12\UNKNOWN\WN00
	//string strSrcImg = "E:\\MyProject\\MarkerMatch\\img\\20190419155800745_rotated.jpg";
	string strSrcImg = "E:\\MyProject\\MarkerMatch\\����\\16\\S5K-49512-RA\\WN01\\20190616121244562.jpg";
	//string strSrcImg  = "E:\\MyProject\\MarkerMatch\\����\\6.11����\\5.12-13����\\12\\UNKNOWN\\WN00\\20190512142914217.jpg";
	string strTempImg = "E:\\MyProject\\MarkerMatch\\template\\solidcross4_b.jpg";

	Mat srcImg = imread(strSrcImg);
	Mat tempImg = imread(strTempImg);

	clock_t s, e;
	s = clock();
	Mat bImg;
	vector<Point> vecLoc;
	vector<Rect> vecRect;
	CMarkerFinder::LocateMarkerArea(srcImg, vecRect);
	e = clock();
	cout << "The Location time is: " << (double)(e - s) / CLOCKS_PER_SEC * 1000 << "ms" << endl;

	Mat srcGrayImg;
	Mat tempGrayImg;
	cvtColor(srcImg, srcGrayImg, CV_BGR2GRAY);
	cvtColor(tempImg, tempGrayImg, CV_BGR2GRAY);

	//�Զ�ֵ��ͼ�����ƥ��;
	Mat srcBImg;
	Mat resImg;
	threshold(srcGrayImg, srcBImg, 0, 255, THRESH_OTSU);

	matchTemplate(srcBImg, tempGrayImg, resImg, CV_TM_CCOEFF_NORMED); //�����ϵ��ƥ�䷨(���ƥ��1)
	normalize(resImg, resImg, 0, 1, NORM_MINMAX);

	double minValue, maxValue;
	Point minLoc, maxLoc;
	int nSpace = 15;
	for (int i = 0; i < 4; i++)
	{
		minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
		if (maxValue < 0.7)
			continue;
		cout << "max_value= " << maxValue << endl;
		cout << "maxLoc_x=" << maxLoc.x << ",maxLoc_y=" << maxLoc.y << endl;


		int startX = maxLoc.x - nSpace;
		int startY = maxLoc.y - nSpace;
		int endX = maxLoc.x + nSpace;
		int endY = maxLoc.y + nSpace;
		if (startX<0 || startY < 0)
		{
			startX = 0;
			startY = 0;
		}
		if (endX > resImg.cols - 1 || endY > resImg.rows - 1)
		{
			endX = resImg.cols - 1;
			endY = resImg.rows - 1;
		}
		//�����ƥ�����Χ�����ݶ���գ������´�ƥ�仹���ڸ�����
		Mat temp = Mat::zeros(endX - startX, endY - startY, CV_32FC1);
		temp.copyTo(resImg(Rect(startX, startY, temp.cols, temp.rows)));

		rectangle(srcImg, maxLoc, Point(maxLoc.x + tempImg.cols, maxLoc.y + tempImg.rows), Scalar(0, 0, 255), 6);
	}

	namedWindow("srcimg", 0);
	resizeWindow("srcimg", 1368, 912);
	imshow("srcimg", srcImg);

	waitKey(0);

    return 0;
}

