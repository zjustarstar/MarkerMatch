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
	int nChannels;          //图像通道数; nChannels=1表示灰度图,nChannels=3表示RGB彩色图;
	int nW;
	int nH;
	int nStep;
};

//一条线段的定义;x1和y1构成一个点，x2和y2构成另外一个点;
typedef struct MyLines {
	int x1;
	int y1;
	int x2;
	int y2;
};

typedef struct LocRectArray {
	LocRect   lr[20];
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

//判断是否移动;
//nThre:阈值参数。该值越大，返回的nCount越少。建议粗调设为50，精调设为25。该值可以根据不同设备，设置不同的值;
//输出：nCount表示移动量，未移动nCount应该为0，移动的越多，该值越大;但是灯光变化也会同样导致该值变大;
//返回：成功返回true,失败返回false;
extern "C" _declspec(dllexport) bool IsMoving(ImgInfo preImg, ImgInfo curImg, int nThre, int * nCount);

//判断是否是黑图
//输出：nMean表示全图的平均值,nMax表示全图的最大值:可以通过这两个值判断是否为黑图;
//返回：成功返回true,失败返回false;
extern "C" _declspec(dllexport) bool IsBlack(ImgInfo img, int * nMean, int * nMax);

/*
功能：检测图中的对齐线;
输入: img表示图像，
      nThre表示阈值。该阈值根据不同的图像有不同的调整。取值约为:100-300之间。该值越大，返回的线越少;
输出：nSize表示返回的线的个数，最多不超过30条线;
      pLines表示返回的线的信息;
返回：由于阈值对于检测结果影响很大,如果输入的阈值过大，则检测不到直线;如果输入的阈值过小,则返回的线太多。
      算法会自动测算阈值，并返回一个相对理想的阈值，作为参考;
*/
extern "C" _declspec(dllexport) int FindAlignment(ImgInfo img, int nThre, int * nSize, MyLines * pLines);