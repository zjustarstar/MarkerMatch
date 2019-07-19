
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
	int nW;
	int nH;
	int nStep;
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