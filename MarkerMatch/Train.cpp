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
	//�����ļ���·��
	if ((lf = _findfirst(cate_dir.c_str(), &file)) == -1) {
		cout << cate_dir << " not found!!!" << endl;
	}
	else {
		while (_findnext(lf, &file) == 0) {
			//����ļ���
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

	CV_Assert(alpha.total() == 1 && svidx.total() == 1 && sv_total == 1);   //�����е�����������ʱ�����ش���  
	CV_Assert((alpha.type() == CV_64F && alpha.at<double>(0) == 1.) ||
		(alpha.type() == CV_32F && alpha.at<float>(0) == 1.f));
	CV_Assert(sv.type() == CV_32F);
	hog_detector.clear();

	hog_detector.resize(sv.cols + 1);
	memcpy(&hog_detector[0], sv.ptr(), sv.cols * sizeof(hog_detector[0]));  //memcpyָ����c��c++ʹ�õ��ڴ濽��������memcpy�����Ĺ����Ǵ�Դsrc��ָ���ڴ��ַ����ʼλ�ÿ�ʼ����n���ֽڵ�Ŀ��dest��ָ���ڴ��ַ����ʼλ���С�  
	hog_detector[sv.cols] = (float)-rho;
}

void startTrain(){
	int nDetWinW = 32;
	int nDetWinH = 32;
	int nLevel = 6;

	/*********************************��ȡ���������������Ϣ********************************/
	//����������������ͼ���ļ�·��;
	vector<string> vecPosFiles;
	vector<string> vecNegFiles;
	int iPosCount, iNegCount, nTotalCount;       //������������;

	string strPosPath = "E:\\MyProject\\MarkerMatch\\����\\HollowCross\\*";

	string strNegPath = "F:\\Sindyware\\����ѵ����ʶ��\\Samples\\Negtive-street\\*";
	string strNegPath1 = "E:\\MyProject\\MarkerMatch\\����\\negative\\*";

	//�ҵ�����ѵ������������;
	FindFiles(strPosPath, vecPosFiles);

	FindFiles(strNegPath, vecNegFiles);
	FindFiles(strNegPath1, vecNegFiles);
	
	iPosCount = vecPosFiles.size();
	iNegCount = vecNegFiles.size();
	nTotalCount = iPosCount + iNegCount;

	/*******************************Hog���Ӽ���Ӧ�Ĵ洢����**********************************/
	//HOG���ӵĴ�С;
	CvSize detWindow = cvSize(nDetWinW, nDetWinH);  //��Ҫ����Ĳ���;
	CvSize blockSize = cvSize(16, 16);
	CvSize blockStride = cvSize(8, 8);
	CvSize cellSize = cvSize(8, 8);

	int    nHogSize;
	cv::HOGDescriptor hog(detWindow, blockSize, blockStride, cellSize, 9, 1, -1, HOGDescriptor::L2Hys, 0.2, true, nLevel);
	nHogSize = hog.getDescriptorSize();

	//���������Լ������ǩ������
	Mat sampleFeaturesMat = Mat(nTotalCount, nHogSize, CV_32FC1);
	Mat sampleLabelMat = Mat(nTotalCount, 1, CV_32SC1);
	
	/*******************************��������������Hog����**********************************/
	//������;
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

	//������;
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

	/********************************************��ʼѵ��**********************************/
	cv::Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);//���Դ�������Էָ������
	svm->setKernel(SVM::LINEAR);//���������
	svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, 1000, 1e-3));
	svm->setC(0.01);
	svm->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
	
	//��SVM���Է�����ѵ��
	sampleFeaturesMat.convertTo(sampleFeaturesMat, CV_32F);
	Ptr<TrainData> td = TrainData::create(sampleFeaturesMat, ROW_SAMPLE, sampleLabelMat);
	svm->trainAuto(td);
	
	vector<float> hog_detector; //����hog�����
	get_svm_detector(svm, hog_detector);

	//���������;
	HOGDescriptor my_hog;
	my_hog.winSize = detWindow;
	my_hog.setSVMDetector(hog_detector);
	//my_hog.save("d:\\clf.xml");     //��������� 
	
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

		//��ʾmaxvalue
		char msg[10];
		sprintf_s(msg, "%.2f",foundWeights[j]);
		putText(resizeSrcImg, string(msg), cvPoint(detections[j].x, detections[j].y - 10), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 255), 2);
	}

	namedWindow("srcimg");
	imshow("srcimg", resizeSrcImg);

	waitKey(0);
}