#include "MarkerFinder.h"
#include <opencv2/imgproc/types_c.h>
#define  ENABLE_TEST  1

CMarkerFinder::CMarkerFinder()
{

}


CMarkerFinder::~CMarkerFinder()
{

}

//查找marker区域;
bool CMarkerFinder::LocateMarkerArea(Mat srcImg, vector<Rect> &vecMarkerRect) {
	Mat bImg;
	vector<Point> vecLoc;
	int nDilateKernalSize = 5;

	//先查找文字区域;
	if (!LocateTextArea(srcImg, bImg, vecLoc))
		return false;

	//对每一组文字区域进行处理;
	for (int i = 0; i < vecLoc.size();	i++)
	{
		Rect curR;
		curR.x = vecLoc[i].x;
		curR.width = vecLoc[i].y - vecLoc[i].x;
		curR.y = 0;
		curR.height = srcImg.rows;

		Mat roiImg = bImg(curR);

		//将二值图横纵坐标都缩小8倍
		Mat resizedBImg;
		resize(roiImg, resizedBImg, cvSize(roiImg.cols / 8, roiImg.rows / 8));

		//对二值图进行膨胀,然后检测文字区域;
		Mat dilateBImg;
		Mat kern = getStructuringElement(MORPH_ELLIPSE, Size(nDilateKernalSize, nDilateKernalSize));
		dilate(resizedBImg, dilateBImg, kern);

		//保存膨胀后的图;
		if (ENABLE_TEST) {
			char strFile[256];
			sprintf_s(strFile, "d:\\dialateBImg_%d.jpg", i);
			imwrite(string(strFile), dilateBImg);
		}

		//计算膨胀后的二值图的水平投影;
		int h = dilateBImg.rows;
		int w = dilateBImg.cols;
		int * pHist = new int[h];
		memset(pHist, 0, sizeof(int)*h);
		for (int row = 0; row < h - 1; row++)
		{
			for (int col = 0; col < w; col++)
			{
				char v1 = dilateBImg.at<char>(row, col);
				if (v1 != 0)
					pHist[row]++;
			}
		}

		//查找区域;
		int nStart = 0;
		int nContinusBlack = 0;

		Rect t; //marker区域;
		for (int j = 0; j < h;)
		{
			if (pHist[j] > 0) {
				
				nStart = j;
				while (pHist[j++] && j<h);
				 
				//只选择有一定长度的区域;
				int nLen = j - nStart+1;  //白色区域的长度;
				if (nLen > 22 && nLen < 42 /*&& nContinusBlack>=2*/)  //之前的连续黑区域要有一定长度;
				{
					t.y = nStart + nDilateKernalSize;
					t.height = nLen + 1 - nDilateKernalSize *2;
					t.x = 0;
					t.width = w;

					//回到原图坐标;
					t.x *= 8;
					t.y *= 8;
					t.width *= 8;
					t.height *= 8;
					t.x += curR.x;
					vecMarkerRect.push_back(t);
				}

				nContinusBlack = 0;  //连续黑色区域的长度重置;
			}
			else
			{
				j++;
				nContinusBlack++;
			}
		}
	}

	//在原图中标记文字区域;
	for (int k = 0; k < vecMarkerRect.size(); k++)
	{
		rectangle(srcImg, vecMarkerRect[k], Scalar(0, 255, 0), 8);
		//imwrite("d:\\src.jpg", srcImg);
	}
	
	return true;
}

//定位文字区域;
//bImg:返回的二值图;
//vecLoc:返回的文字区域，仅限横坐标;
bool CMarkerFinder::LocateTextArea(Mat srcImg, Mat &bImg, vector<Point> & vecLoc) {
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

	//二值化;
	double d = threshold(grad, bImg, 0, 255, THRESH_OTSU);
	
	if (ENABLE_TEST)
	{
		imwrite("d:\\grad.jpg", grad);
		imwrite("d:\\bimg.jpg", bImg);
	}

	//找阶跃点;
	int h = bImg.rows;
	int w = bImg.cols;
	int * nHist = new int[w];
	memset(nHist, 0, sizeof(int)*w);
	for (int r = 0; r < h - 1; r++)
	{
		//uchar* pdata = img.ptr<uchar>(r);
		for (int c = 0; c < w; c++)
		{
			char v1 = bImg.at<char>(r, c);
			char v2 = bImg.at<char>(r + 1, c);
			if (v1 != v2)
				nHist[c]++;
		}
	}

	int nExtendW = 10;  //拓展10个像素;
	int nThre = 8 * 2;  //出现的最简单字符个数(阶跃数)
	int nTextWidThre = 100;  //文字宽度;
	for (int i = 0; i < w - 1;)
	{
		bool b1 = (nHist[i] > nThre);   //左空右边缘;
		if (nHist[i] > nThre) {
			int nStart = i;
			while (nHist[i++] > nThre);
			if ((i - nStart) > nTextWidThre)
			{
				Point pt; //左边x坐标为x,右边x坐标为y;
				pt.x = nStart - nExtendW;
				pt.y = i + nExtendW;
				vecLoc.push_back(pt);
			}
		}
		else
			i++;
	}

	for (int i = 0; i < vecLoc.size(); i++)
	{
		//左边直线;
		Point pt = cvPoint(vecLoc[i].x, 0);
		Point pb = cvPoint(vecLoc[i].x, h - 1);
		line(srcImg, pt, pb, Scalar(0, 0, 255), 8);

		//右边直线;
		pt = cvPoint(vecLoc[i].y, 0);
		pb = cvPoint(vecLoc[i].y, h - 1);
		line(srcImg, pt, pb, Scalar(0, 0, 255), 8);
	}

	if (ENABLE_TEST)
		imwrite("d:\\res.jpg", srcImg);

	//找不到文字区;
	if (vecLoc.size() == 0)
		return false;

	return true;
}

