#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

class CMarkerFinder
{
public:
	CMarkerFinder();
	virtual ~CMarkerFinder();
	//��λ��������;
	static bool LocateTextArea(Mat srcImg, Mat &bImg, vector<Point> & vecLoc);
	static bool LocateMarkerArea(Mat srcImg, vector<Rect> &vecMarkerRect);

private:
	Mat    m_OriImg;  
};

