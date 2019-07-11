#include "MarkerFinder.h"
#include <opencv2/imgproc/types_c.h>
#define  ENABLE_TEST  1

CMarkerFinder::CMarkerFinder()
{

}


CMarkerFinder::~CMarkerFinder()
{

}

//����marker����;
bool CMarkerFinder::LocateMarkerArea(Mat srcImg, vector<Rect> &vecMarkerRect) {
	Mat bImg;
	vector<Point> vecLoc;
	int nDilateKernalSize = 5;

	//�Ȳ�����������;
	if (!LocateTextArea(srcImg, bImg, vecLoc))
		return false;

	//��ÿһ������������д���;
	for (int i = 0; i < vecLoc.size();	i++)
	{
		Rect curR;
		curR.x = vecLoc[i].x;
		curR.width = vecLoc[i].y - vecLoc[i].x;
		curR.y = 0;
		curR.height = srcImg.rows;

		Mat roiImg = bImg(curR);

		//����ֵͼ�������궼��С8��
		Mat resizedBImg;
		resize(roiImg, resizedBImg, cvSize(roiImg.cols / 8, roiImg.rows / 8));

		//�Զ�ֵͼ��������,Ȼ������������;
		Mat dilateBImg;
		Mat kern = getStructuringElement(MORPH_ELLIPSE, Size(nDilateKernalSize, nDilateKernalSize));
		dilate(resizedBImg, dilateBImg, kern);

		//�������ͺ��ͼ;
		if (ENABLE_TEST) {
			char strFile[256];
			sprintf_s(strFile, "d:\\dialateBImg_%d.jpg", i);
			imwrite(string(strFile), dilateBImg);
		}

		//�������ͺ�Ķ�ֵͼ��ˮƽͶӰ;
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

		//��������;
		int nStart = 0;
		int nContinusBlack = 0;

		Rect t; //marker����;
		for (int j = 0; j < h;)
		{
			if (pHist[j] > 0) {
				
				nStart = j;
				while (pHist[j++] && j<h);
				 
				//ֻѡ����һ�����ȵ�����;
				int nLen = j - nStart+1;  //��ɫ����ĳ���;
				if (nLen > 22 && nLen < 42 /*&& nContinusBlack>=2*/)  //֮ǰ������������Ҫ��һ������;
				{
					t.y = nStart + nDilateKernalSize;
					t.height = nLen + 1 - nDilateKernalSize *2;
					t.x = 0;
					t.width = w;

					//�ص�ԭͼ����;
					t.x *= 8;
					t.y *= 8;
					t.width *= 8;
					t.height *= 8;
					t.x += curR.x;
					vecMarkerRect.push_back(t);
				}

				nContinusBlack = 0;  //������ɫ����ĳ�������;
			}
			else
			{
				j++;
				nContinusBlack++;
			}
		}
	}

	//��ԭͼ�б����������;
	for (int k = 0; k < vecMarkerRect.size(); k++)
	{
		rectangle(srcImg, vecMarkerRect[k], Scalar(0, 255, 0), 8);
		//imwrite("d:\\src.jpg", srcImg);
	}
	
	return true;
}

//��λ��������;
//bImg:���صĶ�ֵͼ;
//vecLoc:���ص��������򣬽��޺�����;
bool CMarkerFinder::LocateTextArea(Mat srcImg, Mat &bImg, vector<Point> & vecLoc) {
	Mat gray;
	cvtColor(srcImg, gray, CV_BGR2GRAY);

	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	/// �� X�����ݶ�
	Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	/// �� Y�����ݶ�
	Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	//��ֵ��;
	double d = threshold(grad, bImg, 0, 255, THRESH_OTSU);
	
	if (ENABLE_TEST)
	{
		imwrite("d:\\grad.jpg", grad);
		imwrite("d:\\bimg.jpg", bImg);
	}

	//�ҽ�Ծ��;
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

	int nExtendW = 10;  //��չ10������;
	int nThre = 8 * 2;  //���ֵ�����ַ�����(��Ծ��)
	int nTextWidThre = 100;  //���ֿ��;
	for (int i = 0; i < w - 1;)
	{
		bool b1 = (nHist[i] > nThre);   //����ұ�Ե;
		if (nHist[i] > nThre) {
			int nStart = i;
			while (nHist[i++] > nThre);
			if ((i - nStart) > nTextWidThre)
			{
				Point pt; //���x����Ϊx,�ұ�x����Ϊy;
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
		//���ֱ��;
		Point pt = cvPoint(vecLoc[i].x, 0);
		Point pb = cvPoint(vecLoc[i].x, h - 1);
		line(srcImg, pt, pb, Scalar(0, 0, 255), 8);

		//�ұ�ֱ��;
		pt = cvPoint(vecLoc[i].y, 0);
		pb = cvPoint(vecLoc[i].y, h - 1);
		line(srcImg, pt, pb, Scalar(0, 0, 255), 8);
	}

	if (ENABLE_TEST)
		imwrite("d:\\res.jpg", srcImg);

	//�Ҳ���������;
	if (vecLoc.size() == 0)
		return false;

	return true;
}

