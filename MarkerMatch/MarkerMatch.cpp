// MarkerMatch.cpp : 定义控制台应用程序的入口点。
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
	string strSrcImg = "E:\\MyProject\\MarkerMatch\\数据\\1\\20190611164502063.jpg";
	//string strSrcImg = "d:\\a.jpg";
	//string strTempImg = "E:\\MyProject\\MarkerMatch\\MarkerMatch\\template\\temp1.jpg";
	string strTempImg = "E:\\MyProject\\MarkerMatch\\template\\solidcross4_b.jpg";

	Mat srcImg = imread(strSrcImg);
	Mat tempImg = imread(strTempImg);

	clock_t s, e;
	s = clock();
	Mat bImg;
	vector<Point> vecLoc;
	vector<Rect> vecRect;
	//CMarkerFinder::LocateMarkerArea(srcImg, vecRect);
	e = clock();
	cout << "The Location time is: " << (double)(e - s) / CLOCKS_PER_SEC * 1000 << "ms" << endl;

	vector<Rect> vecTempRect;
	s = clock();
	CMarkerFinder::LocateTemplate(srcImg, tempImg, 4, vecTempRect);
	e = clock();
	cout << "The LocTemp time is: " << (double)(e - s) / CLOCKS_PER_SEC * 1000 << "ms" << endl;

	namedWindow("srcimg", 0);
	resizeWindow("srcimg", 1368, 912);
	imshow("srcimg", srcImg);

	waitKey(0);

    return 0;
}

