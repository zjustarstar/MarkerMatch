#pragma once
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

void FindFiles(string cate_dir, vector<string> & vecFiles);
void startTrain();
void startTest(Mat srcImg);
