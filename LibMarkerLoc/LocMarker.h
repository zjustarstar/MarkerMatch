#define  ERROR_NO             0
#define  ERROR_EMPTY_MARKER  -1
#define  ERROR_FAIL_LOADPARM -2

typedef struct LocRect {
	int x; //���Ͻ�x����;
	int y; //���Ͻ�y����;
	int w; //rect���;
	int h; //rect�߶�;
	float fConf;  //��rect�Ŀ��Ŷ�;
	LocRect() {
		x = y = h = w = 0;
		fConf = 0.0f;
	}
};

typedef struct ImgInfo {
	unsigned char * pData;  //ͼ������;
	int nChannels;          //ͼ��ͨ����; nChannels=1��ʾ�Ҷ�ͼ,nChannels=3��ʾRGB��ɫͼ;
	int nW;
	int nH;
	int nStep;
};

//һ���߶εĶ���;x1��y1����һ���㣬x2��y2��������һ����;
typedef struct MyLines {
	int x1;
	int y1;
	int x2;
	int y2;
};

typedef struct AlgorithParams {
	//pattern��λ������ز���;
	int  locpattern_bCheckLastNum; //���һ�����ֵ��ٴ���֤;
	int  locpattern_bVerticalNum;  //�Ƿ��Ǵ�ֱ��������ְ��;
	float locpattern_fRatio;       //���һ�����ֵ�y�������;

	bool  locpattern_bTwoStageLoc;  //�������׶�ʽ�ļ��.����������Ƭ���Ȳ���̫��,DeltaֵҲ�޷�����ʱ;
									//���õ�һ�μ�⵽����󣬾ͰѸ�����Ĩ�����ٽ��еڶ��μ��;
	int   locpattern_nScDelta;      //��ֵ��ʱ�Ĳ���;��������Ƭ���Ȳ����ʱ��Ĭ��Ϊ0.
	int   locpattern_nHcDelta;      
	float locpattern_fScMatchDegree;  //ƥ����ֵ��Ĭ��0.5;
	float locpattern_fHcMatchDegree;

	//finetune������ز���;
	int finetune_nHcMargin;   //����finetune����,����ʮ�ֵ�margin;

	int refine_nScThickSize;        //����refineʱ��ʵ��ʮ��bar��ȴ�С;
	int refine_nHcThickSize;        //����refineʱ����ʮ�ֵĺ�ȴ�С;

	//���ھ���ʱ������ʮ�ֿ�ľ�ȷ��λ;
	int refineHC_nMarginH;         //����FT_RefineHollyCross���������º�������չ�ߴ�;
	int refineHC_nMarginV;         //Ĭ�϶���6
	
	//Ĭ�ϲ���;
	AlgorithParams() {
		//ģ��ƥ��ʱ�Ķ�����֤��
		locpattern_bCheckLastNum = false; //Ĭ�ϲ������ٴ���֤;
		locpattern_bVerticalNum = true;   //Ĭ����ֱ����;
		locpattern_fRatio = 0.8;         
		//����ģ��ƥ��;
		locpattern_bTwoStageLoc = false;
		locpattern_nScDelta = 0;
		locpattern_nHcDelta = 0;
		locpattern_fScMatchDegree = 0.5;
		locpattern_fHcMatchDegree = 0.5;

		finetune_nHcMargin = 0;

		refine_nHcThickSize = 6;     //����ʮ�ֵĺ��;
		refine_nScThickSize = 24;    //ʵ��ʮ�ֵ�bar���;

		refineHC_nMarginH = 6;
		refineHC_nMarginV = 6;
	}
};

/*�������ʼ��
  hcMarkerImg: HollowCross(����ʮ��)���ͼ;ͬʱ��hc��ʾ����ʮ�����ڵİ���;
  scMarkerImg: SolidCross(ʵ�� ʮ��)���ͼ;ͬʱ��sc��ʾʵ��ʮ�����ڵ�����;
  hcPatternImg: �����µ�patternͼ��;
  scPatternImg: �����µ�patternͼ��; 
  �������
  ���أ��ɹ�����0,ʧ��Ϊ��0;
*/
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcMarkerImg, ImgInfo scMarkerImg, 
	                                             ImgInfo hcPatternImg, ImgInfo scPatternImg,
	                                             AlgorithParams param);

/* ��⵽��pattern�����Լ�����;
   img: �����ԭͼ;
   bHollowCross: ��ʾ�������߰���;trueΪ����ʮ�֣�falseΪ����ʮ��
   �����nSize��ʾ��⵽��pattern����,pRect��ʾÿ��rect������;
   ����: false��ʾ����ʧ��;
*/
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect);

//��⵽��ʮ��˿;
//img: ͼ����ص���Ϣ;
//bHollowCross: trueΪ����ʮ�֣�falseΪ����ʮ��;
//nSize:���ص�ʮ�ָ���;
//pRect:���ص�Rect��Ϣ;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect);

//�ж��Ƿ��ƶ�;
//nThre:��ֵ��������ֵԽ�󣬷��ص�nCountԽ�١�����ֵ���Ϊ50��������Ϊ25����ֵ���Ը��ݲ�ͬ�豸�����ò�ͬ��ֵ;
//�����nCount��ʾ�ƶ�����δ�ƶ�nCountӦ��Ϊ0���ƶ���Խ�࣬��ֵԽ��;���ǵƹ�仯Ҳ��ͬ�����¸�ֵ���;
//���أ��ɹ�����true,ʧ�ܷ���false;
extern "C" _declspec(dllexport) bool IsMoving(ImgInfo preImg, ImgInfo curImg, int nThre, int * nCount);

//΢�����ж��Ƿ��ƶ�;
//���У�r1��r2��ʾ������ʮ�ֿ����ڵ�ROIλ�á����λ�ÿ��Ժܲ���׼��û��ϵ��
extern "C" _declspec(dllexport) bool IsMoving_Finetune(ImgInfo preImg, ImgInfo curImg, LocRect r1, LocRect r2, int nThre, int * nCount);


//�ж��Ƿ��Ǻ�ͼ
//�����nMean��ʾȫͼ��ƽ��ֵ,nMax��ʾȫͼ�����ֵ:����ͨ��������ֵ�ж��Ƿ�Ϊ��ͼ;
//���أ��ɹ�����true,ʧ�ܷ���false;
extern "C" _declspec(dllexport) bool IsBlack(ImgInfo img, int * nMean, int * nMax);

/*
���ܣ����ͼ�еĶ�����;
����: img��ʾͼ��
      nThre��ʾ��ֵ������ֵ���ݲ�ͬ��ͼ���в�ͬ�ĵ�����ȡֵԼΪ:100-300֮�䡣��ֵԽ�󣬷��ص���Խ��;
�����nSize��ʾ���ص��ߵĸ�������಻����30����;
      pLines��ʾ���ص��ߵ���Ϣ;
���أ�������ֵ���ڼ����Ӱ��ܴ�,����������ֵ�������ⲻ��ֱ��;����������ֵ��С,�򷵻ص���̫�ࡣ
      �㷨���Զ�������ֵ��������һ������������ֵ����Ϊ�ο�;
*/
extern "C" _declspec(dllexport) int FindAlignment(ImgInfo img, int nThre, int * nSize, MyLines * pLines);

/* 
 ���ܣ������׶Σ�������ʮ�ֺ�ʵ��ʮ�ֵ�λ��;
       �������������LocRect�ṹ���е�fConf�������ڱ�������û�ã��������;
 ���룺img��ʾͼ��,
       roiRect��ʾRegion of Interest,ָ���µĿ���ʮ����ԭͼ�е�λ��
 ���: pHRect,HollyCross Rect,����ʮ������
       pSRect,SolidCross Rect,ʵ��ʮ������
 ����: ����ֵΪ0����ʾ����������ֵ���㣬��ʾ�д���;
*/
extern "C" _declspec(dllexport) int FineTune(ImgInfo img, LocRect roiRect, LocRect * pHRect, LocRect * pSRect);

/*
���ܣ������׶�2����ʵ�ĺͿ��ķǳ�����ʱ����Ҫ���øú��������ʵ�ĸ���׼ȷ��λ��;
�������������LocRect�ṹ���е�fConf�������ڱ�������û�ã��������;
���룺img��ʾͼ��,
roiRect��ʾRegion of Interest,ָ���µĿ���ʮ����ԭͼ�е�λ��
���룺pSRect Ϊ��⵽��ʵ�Ŀ�λ��;
���: pSRect Ϊ�޸ĺ��ʵ�Ŀ�λ��;
����: ����ֵ ��
*/
extern "C" _declspec(dllexport) void FineTune_RefineSRect(ImgInfo img, LocRect roiRect, LocRect * pSRect);

/*
���ܣ����ڸ��������ͼ������ģ��ͼ����������chFileNameָ�����ļ���;
�������������LocRect�ṹ���е�fConf�������ڱ�������û�ã��������;
���룺img��ʾͼ��,
      nDeltaĬ����0����������ֵ���߸�ֵ�����Ըı����ɵĽ��ͼ
���: ���ɵ�ģ��ͼ������chFileName��������ļ��У���·����;
����: ����ֵ ��
*/
extern "C" _declspec(dllexport) void Util_GenerateTempFile(ImgInfo img, int nDelta, char * chFileName);