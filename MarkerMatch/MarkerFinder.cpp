#include "MarkerFinder.h"
#include <opencv2/imgproc/types_c.h>
#define  ENABLE_TEST  1

CMarkerFinder::CMarkerFinder()
{

}


CMarkerFinder::~CMarkerFinder()
{

}

//��ͼ�в���ģ��ͼƬ;
//srcImg & tempImg: ԭͼ��ģ��ͼ;
//nMaxCount: ͼ�������ܴ��ڵ�ģ��;
//vecTempRect:����ֵ����ʾ�ҵ���ģ������;
bool CMarkerFinder::LocateTemplate(Mat srcImg, Mat tempImg, int nMaxCount, vector<Rect> & vecTempRect) {
	
	Mat srcGrayImg;
	Mat tempGrayImg;
	cvtColor(srcImg, srcGrayImg, CV_BGR2GRAY);
	cvtColor(tempImg, tempGrayImg, CV_BGR2GRAY);

	//�Զ�ֵ��ͼ�����ƥ��;
	Mat srcBImg;
	Mat resImg;
	threshold(srcGrayImg, srcBImg, 0, 255, THRESH_OTSU);

	matchTemplate(srcBImg, tempGrayImg, resImg, CV_TM_CCOEFF_NORMED); //�����ϵ��ƥ�䷨(���ƥ��1)
	//normalize(resImg, resImg, 0, 1, NORM_MINMAX);

	double minValue, maxValue;
	Point minLoc, maxLoc;
	int nSpace = 25;

	for (int i = 0; i < nMaxCount; i++)
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

		Rect k;
		k.x = maxLoc.x;
		k.y = maxLoc.y;
		k.width = tempImg.cols;
		k.height = tempImg.rows;
		vecTempRect.push_back(k);
	
		rectangle(srcImg, k, Scalar(255, 0, 255), 8);
		//��ʾmaxvalue
		char msg[10];
		sprintf_s(msg, "%.1f", maxValue);
		putText(srcImg, string(msg), cvPoint(k.x, k.y - 10), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);

	}

	return vecTempRect.size();
}

//����-����Marker;
//srcImg:ԭͼ;
//templateImg: ģ��ͼ;
//vecMarkerAreaRect: maker�Ĵ������򣬷Ǿ�ϸ;
//vecMarkerLoc:��ȷ��Marker����,�������;
bool CMarkerFinder::LocateMarker_Fine(Mat srcImg, Mat templateImg,vector<Rect> vecMarkerAreaRect,
	                                  vector<Rect> &vecMarkerLoc) 
{
	int nSize = vecMarkerAreaRect.size();
	if (nSize == 0)
		return false;

	Mat grayTempImg;
	cvtColor(templateImg, grayTempImg, CV_BGR2GRAY);
	for (int i = 0; i < nSize; i++) {
		Rect r = vecMarkerAreaRect[i];

		//ģ��Ĵ�С����������С����ƥ��;
		if ((templateImg.rows > r.height) || (templateImg.cols > r.width))
			continue;

		Mat roiImg = srcImg(r);
		Mat graySrcImg;
		Mat BSrcImg;
		cvtColor(roiImg, graySrcImg, CV_BGR2GRAY);
		threshold(graySrcImg, BSrcImg, 0, 255, THRESH_OTSU);

		//��ֵͼ���ƥ�䣺
		Mat resImg;
		matchTemplate(BSrcImg, grayTempImg, resImg, CV_TM_CCOEFF_NORMED); //�����ϵ��ƥ�䷨(���ƥ��1)
		//normalize(resImg, resImg, 0, 1, NORM_MINMAX);

		double minValue, maxValue;
		Point minLoc, maxLoc;
		minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
		//ƥ��ȹ�С�Ĳ�Ҫ;
		if (maxValue < 0.5)
			continue;

		Rect k;
		k.x = maxLoc.x + r.x;
		k.y = maxLoc.y + r.y;
		k.width = templateImg.cols;
		k.height = templateImg.rows;
		vecMarkerLoc.push_back(k);

		//��ʾmaxvalue
		char msg[10];
		sprintf_s(msg, "%.1f", maxValue);
		putText(srcImg, string(msg), cvPoint(k.x, k.y - 10), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);
	}

	for (int j = 0; j < vecMarkerLoc.size(); j++)
	{
		rectangle(srcImg, vecMarkerLoc[j], Scalar(255, 0, 255), 8);
	}
}

//����marker����;
bool CMarkerFinder::LocateMarkerArea(Mat srcImg, vector<Rect> &vecMarkerRect) {
	Mat bImg;
	vector<Rect> vecLoc;
	int nDilateKernalSize = 5;

	//�Ȳ�����������;
	LocTexParam struLTParam;
	struLTParam.nStepCountThre = 3 * 2;
	if (!LocateTextArea(srcImg, struLTParam, bImg, vecLoc))
		return false;

	//��ÿһ������������д���;
	for (int i = 0; i < vecLoc.size();	i++)
	{
		Rect curR = vecLoc[i];

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
				while (pHist[j++] && j<h);  //���Ұ�ɫ����;
				 
				//ֻѡ����һ�����ȵ�����;
				int nLen = j - nStart+1;  //��ɫ����ĳ���;
				if (nLen > 22 && nLen < 42 )  //��ɫ���򳤶ȵ�����;
				{
					t.y = nStart + nDilateKernalSize;  //ԭʼ��С��Ҫ��������ʱ�ĺ˴�С;
					t.height = nLen + 1 - nDilateKernalSize *2;
					t.x = 0;
					t.width = w;

					//�ص�ԭͼ����;
					t.x *= 8;
					t.y *= 8;
					t.width *= 8;
					t.height *= 8;
					//����ROI��������;
					t.x += curR.x;
					t.y += curR.y;
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

		delete[] pHist;
	}

	//��ԭͼ�б����������;
	for (int k = 0; k < vecMarkerRect.size(); k++)
	{
		rectangle(srcImg, vecMarkerRect[k], Scalar(0, 255, 0), 8);
		//imwrite("d:\\src.jpg", srcImg);
	}
	
	return true;
}

//bImg:����Ķ�ֵͼ;
//struLTParam:�������ֶ�λ�Ĳ���;
//nScale:ԭͼ�����ű���;
//����������������Ҵ�ֱ�߽�
void CMarkerFinder::FindTextCord(Mat bImg, LocTexParam struLTParam,vector<Rect> & vecRect) {

	int h = bImg.rows;
	int w = bImg.cols;

	//��ֱͶӰ���ҽ�Ծ��;
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

	//������״��������,��λ��������;
	int nExtend = 20;  //��չ20������;
	int nStepThre = struLTParam.nStepCountThre;
	int nTextWidThre = struLTParam.nTextBandWThre;
	for (int i = 0; i < w - 1;)
	{
		//���Ҿ���һ����Ծ�������;
		if (nHist[i] > nStepThre) {
			int nStart = i;
			while (nHist[i++] > nStepThre);
			if ((i - nStart) > nTextWidThre)
			{
				Rect rr; 
				//���ұ߽�;
				int x1 = (nStart - nExtend) > 0 ? (nStart - nExtend) : 0;
				int x2 = (i + nExtend) < w ? (i + nExtend) : (w - 1);

				//�߶�Ĭ��Ϊȫͼ�߶�,���±߽��Ժ�λ;
				rr.x = x1;
				rr.width = x2 - x1 + 1;
				rr.y = 0;
				rr.height = (h - 1);
				vecRect.push_back(rr);
			}
		}
		else
			i++;
	}

	delete[] nHist;

	//��λÿ����������±߽�;
	for (int i = 0; i < vecRect.size(); i++)
	{
		Mat roiBImg = bImg(vecRect[i]);

		int nRoiH = roiBImg.rows;
		int nRoiW = roiBImg.cols;

		int * pHHist = new int[h];
		memset(pHHist, 0, sizeof(int)*h);
		for (int r = 0; r < nRoiH - 1; r++)
		{
			for (int c = 0; c < nRoiW; c++)
			{
				char v1 = roiBImg.at<char>(r, c);
				if (v1)
					pHHist[r]++;
			}
		}

		//�ֱ���ϵ��¼�¼��һ�������λ��;
		int nTop = 0;
		int nBottom=h-1;
		while (pHHist[nTop++] == 0);
		while (pHHist[nBottom--] == 0);

		//��΢��չ��;
		nTop = (nTop - nExtend) > 0 ? (nTop - nExtend) : 0;
		nBottom = ((nBottom + nExtend) < h) ? (nBottom + nExtend) : (h - 1);
		vecRect[i].y = nTop;
		vecRect[i].height = nBottom - nTop;

		delete[] pHHist;
	}
}

//��λ��������;
//struLTParam: ���ֶ�λʱ�õĲ���;
//bImg:���صĶ�ֵͼ;
//vecLoc:���ص������������䣬���޺�����;
bool CMarkerFinder::LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc) {
	int nScale = 2; 

	//��Сһ����д���;
	Mat resizedSrcImg;
	resize(srcImg, resizedSrcImg, cvSize(srcImg.cols / nScale, srcImg.rows / nScale));
	struLTParam.nTextBandWThre /= nScale;

	Mat gray;
	cvtColor(resizedSrcImg, gray, CV_BGR2GRAY);

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

	//������������;
	FindTextCord(bImg, struLTParam, vecLoc);
	//�����лص�ԭͼ��С;
	for (int i = 0; i < vecLoc.size(); i++)
	{
		vecLoc[i].x *= nScale;
		vecLoc[i].y *= nScale;
		vecLoc[i].width *= nScale;
		vecLoc[i].height *= nScale;
	}

	//������;
	for (int i = 0; i < vecLoc.size(); i++)
		rectangle(srcImg, vecLoc[i], Scalar(0, 0, 255), 8);

	if (ENABLE_TEST)
		imwrite("d:\\res.jpg", srcImg);

	//�Ҳ���������;
	if (vecLoc.size() == 0)
		return false;

	//�Ļ�ԭ���Ĵ�С
	resize(bImg, bImg, cvSize(bImg.cols * nScale, bImg.rows * nScale));

	return true;
}

