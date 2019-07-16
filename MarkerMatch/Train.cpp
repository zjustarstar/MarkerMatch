#include "Train.h"
#include <cstring>
#include <direct.h>
#include <io.h>
#include <fstream>
#include <opencv2/imgproc/types_c.h>

using namespace std;
using namespace cv;
using namespace cv::ml;

void FindFiles(string cate_dir, vector<string> & vecFiles)
{
	_finddata_t file;
	long lf;
	//输入文件夹路径
	if ((lf = _findfirst(cate_dir.c_str(), &file)) == -1) {
		cout << cate_dir << " not found!!!" << endl;
	}
	else {
		while (_findnext(lf, &file) == 0) {
			//输出文件名
			//cout<<file.name<<endl;
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
				continue;
			
			string strTemp(file.name);
			string strExt;
			strExt = strTemp.substr(strTemp.length() - 3, 3);
			if (strcmp(strExt.c_str(),"jpg") != 0 )
				continue;

			string strFileName;
			strFileName = cate_dir.substr(0, cate_dir.length() - 1);
			strFileName = strFileName + string(file.name);
			vecFiles.push_back(strFileName);
		}
	}
	_findclose(lf);
}

void get_svm_detector(const Ptr< SVM >& svm, vector< float > & hog_detector)
{
	// get the support vectors  
	Mat sv = svm->getSupportVectors();
	const int sv_total = sv.rows;
	// get the decision function  
	Mat alpha, svidx;
	double rho = svm->getDecisionFunction(0, alpha, svidx);

	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);   //括号中的条件不满足时，返回错误  
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);
	hog_detector.clear();

	hog_detector.resize(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));  //memcpy指的是c和c++使用的内存拷贝函数，memcpy函数的功能是从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中。  
	hog_detector[sv.cols] = (float)-rho;
}

void startTrain(){
	int nDetWinW = 32;
	int nDetWinH = 32;
	int nLevel = 6;

	/*********************************读取正负样本的相关信息********************************/
	//储存所有正负样例图的文件路径;
	vector<string> vecPosFiles;
	vector<string> vecNegFiles;
	int iPosCount, iNegCount, nTotalCount;       //正负样本个数;

	string strPosPath = "E:\\MyProject\\MarkerMatch\\样本\\HollowCross\\*";

	string strNegPath = "F:\\Sindyware\\车辆训练及识别\\Samples\\Negtive-street\\*";
	string strNegPath1 = "E:\\MyProject\\MarkerMatch\\样本\\negative\\*";

	//找到所有训练的正负样本;
	FindFiles(strPosPath, vecPosFiles);

	FindFiles(strNegPath, vecNegFiles);
	FindFiles(strNegPath1, vecNegFiles);
	
	iPosCount = vecPosFiles.size();
	iNegCount = vecNegFiles.size();
	nTotalCount = iPosCount + iNegCount;

	/*******************************Hog算子及相应的存储矩阵**********************************/
	//HOG算子的大小;
	CvSize detWindow = cvSize(nDetWinW, nDetWinH);  //需要读入的参数;
	CvSize blockSize = cvSize(16, 16);
	CvSize blockStride = cvSize(8, 8);
	CvSize cellSize = cvSize(8, 8);

	int    nHogSize;
	cv::HOGDescriptor hog(detWindow, blockSize, blockStride, cellSize, 9, 1, -1, HOGDescriptor::L2Hys, 0.2, true, nLevel);
	nHogSize = hog.getDescriptorSize();

	//保存特征以及分类标签的向量
	Mat sampleFeaturesMat = Mat(nTotalCount, nHogSize, CV_32FC1);
	Mat sampleLabelMat = Mat(nTotalCount, 1, CV_32SC1);
	
	/*******************************计算正负样本的Hog特征**********************************/
	//正样本;
	vector<float> featureVec;
	Mat img;
	Mat ResizedImg;
	Mat grayImg;

	cout << "Start to read positive samples..." << endl;
	for (int i = 0; i < iPosCount; i++)
	{
		img = imread(vecPosFiles[i]);
		resize(img, ResizedImg, detWindow);
		cvtColor(ResizedImg, grayImg, CV_BGR2GRAY);

		hog.compute(grayImg, featureVec, blockStride);
		int featureVecSize = featureVec.size();

		for (int j = 0; j < featureVecSize; j++)
		{
			//CV_MAT_ELEM(sampleFeaturesMat, float, i, j) = featureVec[j];
			sampleFeaturesMat.at<float>(i, j) = featureVec[j];
		}
		sampleLabelMat.at<int>(i,0) = 1;
	}

	cout << "end of read for positive samples..." << endl;
	cout << "start to read negative samples..." << endl;

	//负样本;
	for (int i = 0; i < iNegCount; i++)
	{
		img = imread(vecNegFiles[i]);
		resize(img, ResizedImg,detWindow);
		cvtColor(ResizedImg, grayImg, CV_BGR2GRAY);

		hog.compute(grayImg, featureVec, blockStride);
		int featureVecSize = featureVec.size();

		for (int j = 0; j < featureVecSize; j++)
		{
			//CV_MAT_ELEM(*sampleFeaturesMat, float, i + iPosCount, j) = featureVec[j];
			sampleFeaturesMat.at<float>(i+iPosCount, j) = featureVec[j];
		}
		//sampleLabelMat->data.fl[i + iPosCount] = -1;
		sampleLabelMat.at<int>(i+iPosCount, 0) = -1;
	}

	cout << "end of read for negative samples." << endl;
	cout << "start to train for SVM classifier." << endl;

	/********************************************开始训练**********************************/
	cv::Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);//可以处理非线性分割的问题
	svm->setKernel(SVM::LINEAR);//径向基函数
	svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 1e-3));
	svm->setC(0.01);
	svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
	
	//用SVM线性分类器训练
	sampleFeaturesMat.convertTo(sampleFeaturesMat, CV_32F);
	Ptr<TrainData> td = TrainData::create(sampleFeaturesMat, ROW_SAMPLE, sampleLabelMat);
	svm->trainAuto(td);
	
	vector<float> hog_detector; //定义hog检测器
	get_svm_detector(svm, hog_detector);

	//保存分类器;
	HOGDescriptor my_hog;
	my_hog.winSize = detWindow;
	my_hog.setSVMDetector(hog_detector);
	//my_hog.save("d:\\clf.xml");     //保存分类器 
	
	cout << "end of train..." << endl;
}

void startTest(Mat srcImg) {

	HOGDescriptor hog;
	hog.blockSize = cvSize(16, 16);
	hog.blockStride = cvSize(8, 8);
	hog.cellSize = cvSize(8, 8);
	hog.winSize = cvSize(48, 48);
	hog.nlevels = 3;

	vector<float> x;
	ifstream fileIn;
	fileIn.open("d:\\clf.mod", ios::in);
	float val = 0.0f;
	while (!fileIn.eof())
	{
		fileIn >> val;
		x.push_back(val);
	}
	fileIn.close();
	hog.setSVMDetector(x);
	if (hog.checkDetectorSize())
		cout << "detector size checked" << endl;

	Mat resizeSrcImg;
	resize(srcImg, resizeSrcImg,cvSize(srcImg.cols/4,srcImg.rows/4));
	Mat graySrcImg;
	cvtColor(resizeSrcImg, graySrcImg, CV_BGR2GRAY);

	clock_t s, e;
	vector< Rect > detections;
	vector< double > foundWeights;
	s = clock();
	hog.detectMultiScale(graySrcImg, detections, foundWeights,-0.7);
	e = clock();
	cout << "The detection time is: " << (double)(e - s) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
	for (size_t j = 0; j < detections.size(); j++)
	{
		Scalar color = Scalar(0, 0, 255);
		rectangle(resizeSrcImg, detections[j], color, 4);

		//显示maxvalue
		char msg[10];
		sprintf_s(msg, "%.2f",foundWeights[j]);
		putText(resizeSrcImg, string(msg), cvPoint(detections[j].x, detections[j].y - 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 255), 2);
	}

	namedWindow("srcimg");
	imshow("srcimg", resizeSrcImg);

	waitKey(0);
}