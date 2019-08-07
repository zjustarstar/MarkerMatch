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

typedef struct LocRectArray {
	LocRect   lr[20];
};

/*�������ʼ��
  hcMarkerImg: HollowCross(����ʮ��)���ͼ;ͬʱ��hc��ʾ����ʮ�����ڵİ���;
  scMarkerImg: SolidCross(ʵ�� ʮ��)���ͼ;ͬʱ��sc��ʾʵ��ʮ�����ڵ�����;
  hcPatternImg: �����µ�patternͼ��;
  scPatternImg: �����µ�patternͼ��; 
  �������
  ���أ��ɹ�����0,ʧ��Ϊ��0;
*/
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcMarkerImg, ImgInfo scMarkerImg, ImgInfo hcPatternImg, ImgInfo scPatternImg);

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

//�ж��Ƿ��Ǻ�ͼ
//�����nMean��ʾȫͼ��ƽ��ֵ,nMax��ʾȫͼ�����ֵ:����ͨ��������ֵ�ж��Ƿ�Ϊ��ͼ;
//���أ��ɹ�����true,ʧ�ܷ���false;
extern "C" _declspec(dllexport) bool IsBlack(ImgInfo img, int * nMean, int * nMax);