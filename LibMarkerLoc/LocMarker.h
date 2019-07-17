
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

//�������ʼ��;�ɹ�Ϊtrue,ʧ��Ϊfalse;
extern "C" _declspec(dllexport) bool initDetector();

//��⵽��pattern����;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, int * nSize, LocRect * pRect);

//��⵽��ʮ��˿;
//img: ͼ����ص���Ϣ;
//bHollowCross: trueΪ����ʮ�֣�falseΪ����ʮ��;
//nSize:���ص�ʮ�ָ���;
//pRect:���ص�Rect��Ϣ;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect);