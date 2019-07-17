
typedef struct LocRect {
	int x; //左上角x坐标;
	int y; //左上角y坐标;
	int w; //rect宽度;
	int h; //rect高度;
	float fConf;  //该rect的可信度;
	LocRect() {
		x = y = h = w = 0;
		fConf = 0.0f;
	}
};

typedef struct ImgInfo {
	unsigned char * pData;  //图像数据;
	int nW;
	int nH;
	int nStep;
};

//检测器初始化;成功为true,失败为false;
extern "C" _declspec(dllexport) bool initDetector();

//检测到的pattern个数;
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, int * nSize, LocRect * pRect);

//检测到的十字丝;
//img: 图像相关的信息;
//bHollowCross: true为暗场十字，false为明场十字;
//nSize:返回的十字个数;
//pRect:返回的Rect信息;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect);