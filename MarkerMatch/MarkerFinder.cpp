#include "MarkerFinder.h"
#include <opencv2/imgproc/types_c.h>
#define  ENABLE_TEST  0

CMarkerFinder::CMarkerFinder()
{

}


CMarkerFinder::~CMarkerFinder()
{

}

//查找暗场下的marker
//srcImg & tempImg: 原图和模板图;
//wmp:定位晶片marker时的参数;
//vecLocRect: 返回的定位结果;
bool CMarkerFinder::FindWafterMarkerArea(Mat srcImg, Mat bImg, LocWafterMarkerParam wmp,
										vector<Rect>vecTextLoc, vector<Rect> &vecMarkerAreaRect)
{
	int nSize = vecTextLoc.size();
	if (nSize == 0)
		return false;

	int nExtend = 10;
	for (int i = 0; i < nSize; i++)
	{
		Rect wafterRect;
		//默认maker在文字的左边;
		wafterRect.x = vecTextLoc[i].x - wmp.nDistToText - wmp.nMarkerWidth - nExtend;
		if (wafterRect.x < 0)
			wafterRect.x = 0;
		wafterRect.y = vecTextLoc[i].y;
		wafterRect.width = wmp.nMarkerWidth + nExtend * 2;
		
		int nTextBottom = vecTextLoc[i].y + vecTextLoc[i].height;
		//假设maker的底部坐标和文本坐标差不多。为了稳妥，增加了marker大小的高度;
		int nWafterBtm = nTextBottom + wmp.nMarkerWidth;
		wafterRect.height = nWafterBtm - wafterRect.y;

		vecMarkerAreaRect.push_back(wafterRect);
	}

	//如果只有1个marker，还要找第二个;
	if (nSize == 1)
	{
		//缩放后的二值图;
		Mat resizedBImg;
		resize(bImg, resizedBImg, cvSize(bImg.cols / 8, bImg.rows / 8));
		
		//对二值图进行膨胀,然后检测Marker区域;
		Mat dilateBImg;
		Mat kern = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		dilate(resizedBImg, dilateBImg, kern);

		//计算垂直投影：
		int h = dilateBImg.rows;
		int w = dilateBImg.cols;
		int * pHist = new int[w];
		memset(pHist, 0, sizeof(int)*w);
		for (int row = 0; row < h; row++)
		{
			for (int col = 0; col < w; col++)
			{
				//假设已有marker区域的左边还有一个marker
				//第二个marker的距离：5*wmp.nDistToText;
				int nMarkerLeft = vecMarkerAreaRect[0].x - 10*wmp.nDistToText - wmp.nMarkerWidth; 
				nMarkerLeft /= 8;
				int nMarkerRight = vecTextLoc[0].x + vecTextLoc[0].width;
				nMarkerRight /= 8;

				//已有marker区域不计算;
				if (col>=nMarkerLeft && col<=nMarkerRight)
					continue;

				char v1 = dilateBImg.at<char>(row, col);
				if (v1 != 0)
					pHist[col]++;
			}
		}

		//寻找candidate maker区域;
		vector<Rect> vecTemp;
		Rect rr;
		for (int k = 0; k < w;)
		{
			int nStart;
			if (pHist[k] > 0) {
				nStart = k;
				//找到第一个非黑点;
				while (pHist[k++]);
				//可能是marker;
				if ((k - nStart) * 8 > 0.7 * wmp.nMarkerWidth)
				{
					rr.x = nStart * 8;  //缩放8倍以后，需要算回原始坐标;
					rr.width = (k - nStart) * 8;
					rr.y = 0;
					rr.height = bImg.rows - 1;
					vecTemp.push_back(rr);

					//rectangle(srcImg, rr, Scalar(255, 255, 255), 8);
				}
			}
			else
				k++;
		}

		delete[] pHist;

		//只有一个区域的话，那就是这个marker;
		if (vecTemp.size() == 1)
		{
			rr = vecTemp[0];
			rr.x -= 30;
			rr.width += 60;
			vecMarkerAreaRect.push_back(rr);
		}
		//多个区域的话，不管是2个还是3个，都是第二个;
		else if (vecTemp.size() > 1)
		{
			rr = vecTemp[1];
			rr.x -= 20;
			rr.width += 40;
			vecMarkerAreaRect.push_back(rr);
		}
	}

	return true;
}


//在图中查找模板图片;
//srcImg & tempImg: 原图和模板图;
//nMaxCount: 图中最多可能存在的模板;
//vecTempRect:返回值，表示找到的模板区域;
bool CMarkerFinder::LocateTemplate(Mat srcImg, Mat tempImg, int nMaxCount, vector<Rect> & vecTempRect) {
	
	Mat srcGrayImg;
	Mat tempGrayImg;
	cvtColor(srcImg, srcGrayImg, CV_BGR2GRAY);
	cvtColor(tempImg, tempGrayImg, CV_BGR2GRAY);

	//对二值化图像进行匹配;
	Mat srcBImg;
	Mat resImg;
	threshold(srcGrayImg, srcBImg, 0, 255, THRESH_OTSU);

	matchTemplate(srcBImg, tempGrayImg, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)
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

		//将最高匹配点周围的数据都清空，以免下次匹配还是在附近：
		Mat temp = Mat::zeros(endX - startX, endY - startY, CV_32FC1);
		temp.copyTo(resImg(Rect(startX, startY, temp.cols, temp.rows)));

		Rect k;
		k.x = maxLoc.x;
		k.y = maxLoc.y;
		k.width = tempImg.cols;
		k.height = tempImg.rows;
		vecTempRect.push_back(k);
	
		rectangle(srcImg, k, Scalar(255, 0, 255), 8);
		//显示maxvalue
		char msg[10];
		sprintf_s(msg, "%.1f", maxValue);
		putText(srcImg, string(msg), cvPoint(k.x, k.y - 10), FONT_HERSHEY_PLAIN, 4, Scalar(255, 0, 255), 6);
	}

	return vecTempRect.size();
}

//精调-查找Marker;
//srcImg:原图;
//templateImg: 模板图;
//vecMarkerAreaRect: maker的大致区域，非精细;
//vecMarkerLoc:精确的Marker定位结果,输出参数;
bool CMarkerFinder::LocateMarker_Fine(Mat srcImg, Mat templateImg,vector<Rect> vecMarkerAreaRect,
	                                  vector<LocMarker> &vecMarkerLoc)
{
	int nSize = vecMarkerAreaRect.size();
	if (nSize == 0)
		return false;

	Mat grayTempImg;
	cvtColor(templateImg, grayTempImg, CV_BGR2GRAY);
	for (int i = 0; i < nSize; i++) {
		Rect r = vecMarkerAreaRect[i];

		//模板的大小必须比区域更小才能匹配;
		if ((templateImg.rows > r.height) || (templateImg.cols > r.width))
			continue;

		Mat roiImg = srcImg(r);
		Mat graySrcImg;
		Mat BSrcImg;
		cvtColor(roiImg, graySrcImg, CV_BGR2GRAY);
		threshold(graySrcImg, BSrcImg, 0, 255, THRESH_OTSU);

		//二值图像的匹配：
		Mat resImg;
		matchTemplate(BSrcImg, grayTempImg, resImg, CV_TM_CCOEFF_NORMED); //化相关系数匹配法(最好匹配1)

		double minValue, maxValue;
		Point minLoc, maxLoc;
		minMaxLoc(resImg, &minValue, &maxValue, &minLoc, &maxLoc);
		//匹配度过小的不要;
		if (maxValue < 0.5)
			continue;

		//切换回全图坐标;
		Rect k;
		LocMarker lm;
		k.x = maxLoc.x + r.x;
		k.y = maxLoc.y + r.y;
		k.width = templateImg.cols;
		k.height = templateImg.rows;

		lm.rect = k;
		lm.fConfidence = maxValue;
		vecMarkerLoc.push_back(lm);
	}

	return true;
}

//查找mask的实心十字marker区域;
bool CMarkerFinder::FindMaskMarkerArea(Mat srcImg, Mat bImg, vector<Rect>vecTextLoc, vector<Rect> &vecMarkerRect) {
	
	int nDilateKernalSize = 5;

	//对每一组文字区域进行处理; 
	for (int i = 0; i < vecTextLoc.size();	i++)
	{
		Rect curR = vecTextLoc[i];
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
				while (pHist[j++] && j<h);  //查找白色区域;
				 
				//只选择有一定长度的区域;
				int nLen = j - nStart+1;  //白色区域的长度;
				if (nLen > 22 && nLen < 42 )  //白色区域长度的限制;
				{
					t.y = nStart + nDilateKernalSize;  //原始大小需要加上膨胀时的核大小;
					t.height = nLen + 1 - nDilateKernalSize *2;
					t.x = 0;
					t.width = w;

					//回到原图坐标;
					t.x *= 8;
					t.y *= 8;
					t.width *= 8;
					t.height *= 8;
					//加上ROI区域坐标;
					t.x += curR.x;
					t.y += curR.y;
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

		delete[] pHist;
	}
	
	return true;
}

//bImg:输入的二值图;
//struLTParam:用于文字定位的参数;
//nScale:原图的缩放比例;
//查找文字区域的左右垂直边界
void CMarkerFinder::FindTextCord(Mat bImg, LocTexParam struLTParam,vector<Rect> & vecRect) {

	int h = bImg.rows;
	int w = bImg.cols;

	//垂直投影，找阶跃点;
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

	//查找条状文字区域,定位左右坐标;
	int nExtend = 20;  //拓展20个像素;
	int nStepThre = struLTParam.nStepCountThre;
	int nTextWidThre = struLTParam.nTextBandWThre;
	for (int i = 0; i < w - 1;)
	{
		//查找具有一定阶跃点的坐标;
		if (nHist[i] > nStepThre) {
			int nStart = i;
			while (nHist[i++] > nStepThre);
			if ((i - nStart) > nTextWidThre)
			{
				Rect rr; 
				//左右边界;
				int x1 = (nStart - nExtend) > 0 ? (nStart - nExtend) : 0;
				int x2 = (i + nExtend) < w ? (i + nExtend) : (w - 1);

				//高度默认为全图高度,上下边界稍后定位;
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

	//定位每个区域的上下边界;
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

		//分别从上到下记录第一个非零的位置;
		int nTop = 0;
		int nBottom=h-1;
		while (pHHist[nTop++] == 0);
		while (pHHist[nBottom--] == 0);

		//稍微拓展下;
		nTop = (nTop - nExtend) > 0 ? (nTop - nExtend) : 0;
		nBottom = ((nBottom + nExtend) < h) ? (nBottom + nExtend) : (h - 1);
		vecRect[i].y = nTop;
		vecRect[i].height = nBottom - nTop;

		delete[] pHHist;
	}
}

//定位文字区域;
//struLTParam: 文字定位时用的参数;
//bImg:返回的二值图;
//vecLoc:返回的文字区域区间，仅限横坐标;
bool CMarkerFinder::LocateTextArea(Mat srcImg, LocTexParam struLTParam, Mat &bImg, vector<Rect> & vecLoc) {
	int nScale = 2; 

	//缩小一半进行处理;
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

	//查找文字区域;
	FindTextCord(bImg, struLTParam, vecLoc);
	//坐标切回到原图大小;
	for (int i = 0; i < vecLoc.size(); i++)
	{
		vecLoc[i].x *= nScale;
		vecLoc[i].y *= nScale;
		vecLoc[i].width *= nScale;
		vecLoc[i].height *= nScale;
	}

	//画区域;
	//for (int i = 0; i < vecLoc.size(); i++)
	//	rectangle(srcImg, vecLoc[i], Scalar(0, 0, 255), 8);

	if (ENABLE_TEST)
		imwrite("d:\\res.jpg", srcImg);

	//找不到文字区;
	if (vecLoc.size() == 0)
		return false;

	//改回原来的大小
	resize(bImg, bImg, cvSize(bImg.cols * nScale, bImg.rows * nScale));

	return true;
}

