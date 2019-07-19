
#define  ERROR_NO             0
#define  ERROR_EMPTY_MARKER  -1
#define  ERROR_FAIL_LOADPARM -2

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

/*检测器初始化
  hcMarkerImg: HollowCross(虚心十字)标记图;同时用hc表示虚心十字所在的暗场;
  scMarkerImg: SolidCross(实心 十字)标记图;同时用sc表示实心十字所在的明场;
  hcPatternImg: 暗场下的pattern图像;
  scPatternImg: 明场下的pattern图像; 
  输出：无
  返回：成功返回0,失败为非0;
*/
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcMarkerImg, ImgInfo scMarkerImg, ImgInfo hcPatternImg, ImgInfo scPatternImg);

/* 检测到的pattern个数以及坐标;
   img: 输入的原图;
   bHollowCross: 表示明场或者暗场;true为暗场十字，false为明场十字
   输出：nSize表示检测到的pattern个数,pRect表示每个rect的坐标;
   返回: false表示函数失败;
*/
extern "C" _declspec(dllexport) bool LocatePattern(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect);

//检测到的十字丝;
//img: 图像相关的信息;
//bHollowCross: true为暗场十字，false为明场十字;
//nSize:返回的十字个数;
//pRect:返回的Rect信息;
extern "C" _declspec(dllexport) bool LocateCross(ImgInfo img, bool bHollowCross, int * nSize, LocRect * pRect);