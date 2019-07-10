// MarkerMatch.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace std;
using namespace cv;

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world400d.lib")
#else  
#pragma comment(lib,"opencv_world400.lib")
#endif

void FindTextArea(Mat srcImg) {
	Mat gray;
	cvtColor(srcImg, gray, CV_BGR2GRAY);

	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// 求 X方向梯度
	Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	/// 求 Y方向梯度
	Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	imwrite("d:\\grad.jpg", grad);

	Mat bimg;
	threshold(grad, bimg, 0, 255, THRESH_OTSU);
	imwrite("d:\\bimg.jpg", bimg);

	//找阶跃点;
	int h = bimg.rows;
	int w = bimg.cols;
	int * nHist = new int[w];
	memset(nHist, 0, sizeof(int)*w);
	for (int r = 0; r < h-1; r++)
	{
		//uchar* pdata = img.ptr<uchar>(r);
		for (int c = 0; c < w; c++)
		{
			char v1 = bimg.at<char>(r, c);
			char v2 = bimg.at<char>(r + 1, c);
			if (v1 != v2)
				nHist[c]++;
		}
	}

	int nThre = 8*2;  //出现的字符个数(阶跃数)
	int nTextWidThre = 100;
	vector<int> vecLoc;
	for (int i = 0; i < w-1;)
	{
		bool b1 = (nHist[i]>nThre);   //左空右边缘;
		if (nHist[i] > nThre) {
			int nStart = i;
			while (nHist[i++] > nThre);
			if ((i - nStart) > nTextWidThre)
			{
				vecLoc.push_back(nStart);
				vecLoc.push_back(i);
			}
		}
		else
			i++;
	}

	for (int i = 0; i < vecLoc.size(); i++)
	{
		Point pt = cvPoint(vecLoc[i], 0);
		Point pb = cvPoint(vecLoc[i], h - 1);
		line(srcImg, pt, pb, Scalar(0, 0, 255), 4);
	}
	
	imwrite("d:\\res.jpg", srcImg);
}

bool FindOuterMarker(Mat srcImg, Mat tempImg) {

}

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
	//E:\MyProject\MarkerMatch\数据\对位图像\2CW0026-3-1
	//string strSrcImg = "E:\\MyProject\\MarkerMatch\\img\\20190419155800745_rotated.jpg";
	string strSrcImg  = "E:\\MyProject\\MarkerMatch\\数据\\对位图像\\2SF0189-030APD1\\20190607144956654.jpg";
	string strTempImg = "E:\\MyProject\\MarkerMatch\\template\\solidcross4_b.jpg";

	Mat srcImg = imread(strSrcImg);
	Mat tempImg = imread(strTempImg);

	FindTextArea(srcImg);

	Mat srcGrayImg;
	Mat tempGrayImg;
	cvtColor(srcImg, srcGrayImg, CV_BGR2GRAY);
	cvtColor(tempImg, tempGrayImg, CV_BGR2GRAY);

	//对二值化图像进行匹配;
	Mat srcBImg;
	Mat resImg;
	threshold(srcGrayImg, srcBImg, 0, 255, THRESH_OTSU);

	matchTemplate(srcBImg, tempGrayImg, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)
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
		//将最高匹配点周围的数据都清空，以免下次匹配还是在附近：
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

