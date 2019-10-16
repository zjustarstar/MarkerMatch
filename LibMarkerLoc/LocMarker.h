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

typedef struct AlgorithParams {
	//pattern定位函数相关参数;
	int  locpattern_bCheckLastNum; //最后一个数字的再次验证;
	int  locpattern_bVerticalNum;  //是否是垂直方向的数字版号;
	float locpattern_fRatio;       //最后一个数字的y坐标比率;

	bool  locpattern_bTwoStageLoc;  //开启二阶段式的检测.仅用于左右片亮度差异太大,Delta值也无法补充时;
									//采用第一次检测到区域后，就把该区域抹掉，再进行第二次检测;
	int   locpattern_nScDelta;      //二值化时的补偿;用于左右片亮度差异大时。默认为0.
	int   locpattern_nHcDelta;      
	float locpattern_fScMatchDegree;  //匹配阈值。默认0.5;
	float locpattern_fHcMatchDegree;

	//finetune函数相关参数;
	int finetune_nHcMargin;   //用于finetune函数,空心十字的margin;

	int refine_nScThickSize;        //精调refine时的实心十字bar厚度大小;
	int refine_nHcThickSize;        //精调refine时空心十字的厚度大小;

	//用于精调时的虚心十字框的精确定位;
	int refineHC_nMarginH;         //用于FT_RefineHollyCross函数的上下和左右扩展尺寸;
	int refineHC_nMarginV;         //默认都是6
	
	//默认参数;
	AlgorithParams() {
		//模板匹配时的二次验证：
		locpattern_bCheckLastNum = false; //默认不进行再次验证;
		locpattern_bVerticalNum = true;   //默认竖直方向;
		locpattern_fRatio = 0.8;         
		//用于模板匹配;
		locpattern_bTwoStageLoc = false;
		locpattern_nScDelta = 0;
		locpattern_nHcDelta = 0;
		locpattern_fScMatchDegree = 0.5;
		locpattern_fHcMatchDegree = 0.5;

		finetune_nHcMargin = 0;

		refine_nHcThickSize = 6;     //空心十字的厚度;
		refine_nScThickSize = 24;    //实心十字的bar厚度;

		refineHC_nMarginH = 6;
		refineHC_nMarginV = 6;
	}
};

/*检测器初始化
  hcMarkerImg: HollowCross(虚心十字)标记图;同时用hc表示虚心十字所在的暗场;
  scMarkerImg: SolidCross(实心 十字)标记图;同时用sc表示实心十字所在的明场;
  hcPatternImg: 暗场下的pattern图像;
  scPatternImg: 明场下的pattern图像; 
  输出：无
  返回：成功返回0,失败为非0;
*/
extern "C" _declspec(dllexport) int initDetector(ImgInfo hcMarkerImg, ImgInfo scMarkerImg, 
	                                             ImgInfo hcPatternImg, ImgInfo scPatternImg,
	                                             AlgorithParams param);

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

//微调下判断是否移动;
//其中，r1和r2表示两个虚十字框所在的ROI位置。这个位置可以很不精准，没关系。
extern "C" _declspec(dllexport) bool IsMoving_Finetune(ImgInfo preImg, ImgInfo curImg, LocRect r1, LocRect r2, int nThre, int * nCount);


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

/* 
 功能：精调阶段，检测空心十字和实心十字的位置;
       输入输出参数的LocRect结构体中的fConf参数，在本函数中没用，无需理会;
 输入：img表示图像,
       roiRect表示Region of Interest,指大致的空心十字在原图中的位置
 输出: pHRect,HollyCross Rect,空心十字坐标
       pSRect,SolidCross Rect,实心十字坐标
 返回: 返回值为0，表示正常；返回值非零，表示有错误;
*/
extern "C" _declspec(dllexport) int FineTune(ImgInfo img, LocRect roiRect, LocRect * pHRect, LocRect * pSRect);

/*
功能：精调阶段2，当实心和空心非常靠近时，需要调用该函数，检测实心更加准确的位置;
输入输出参数的LocRect结构体中的fConf参数，在本函数中没用，无需理会;
输入：img表示图像,
roiRect表示Region of Interest,指大致的空心十字在原图中的位置
输入：pSRect 为检测到的实心框位置;
输出: pSRect 为修改后的实心框位置;
返回: 返回值 无
*/
extern "C" _declspec(dllexport) void FineTune_RefineSRect(ImgInfo img, LocRect roiRect, LocRect * pSRect);

/*
功能：用于根据输入的图像生成模板图，并保存在chFileName指定的文件中;
输入输出参数的LocRect结构体中的fConf参数，在本函数中没用，无需理会;
输入：img表示图像,
      nDelta默认是0，可以是正值或者负值，可以改变生成的结果图
输出: 生成的模板图保存在chFileName所代表的文件中（带路径）;
返回: 返回值 无
*/
extern "C" _declspec(dllexport) void Util_GenerateTempFile(ImgInfo img, int nDelta, char * chFileName);