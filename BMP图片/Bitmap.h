#pragma once
#include "SingleTon.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string> 
#include <windows.h>//此头文件包含bmp图片的文件头和位图信息头

/**
//1、文件头：bitmap file header
typedef struct tagBITMAPFILEHEADER {
	WORD  bfType;//文件类型，必须为"BM"，占用2个字节
	DWORD bfSize;//文件大小，以字节为单位，占用4个字节
	WORD  bfReserved1;//bfReserved1和bfReserved2：保留字段，占用各2个字节，通常设置为0。
	WORD  bfReserved2;
	DWORD bfOffBits;//bfOffBits：位图数据的偏移量，即位图文件头和位图信息头之后的字节数，占用4个字节。
}  BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

//2、位图信息头 ： bitmap info header
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;//结构体的大小，以字节为单位，必须为40。
    LONG  biWidth;//位图的宽度，以像素为单位。
    LONG  biHeight;//位图的高度，以像素为单位。如果值为正数，则表示位图是从上到下的；如果值为负数，则表示位图是从下到上的。
    WORD  biPlanes;//目标设备的位平面数，必须为1
    WORD  biBitCount;//每个像素的位数，可以是1、4、8、16、24或32。
    DWORD biCompression;//指定位图压缩类型。常用的有BI_RGB、BI_RLE8和BI_RLE4。
    DWORD biSizeImage;//位图数据的大小，以字节为单位。
    LONG  biXPelsPerMeter;//目标设备水平方向上每米像素数。
    LONG  biYPelsPerMeter;//目标设备垂直方向上每米像素数。
    DWORD biClrUsed;//位图实际使用的颜色表中的颜色数。
    DWORD biClrImportant;//对图像显示有重要影响的颜色索引数。
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD * LPRGBQUAD;

*/

using namespace std;

#define M_SOF0  0xc0
#define M_DHT   0xc4
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT   0xdb
#define M_DRI   0xdd
#define M_APP0  0xe0

static int Zig_Zag[8][8] = { { 0, 1, 5, 6, 14, 15, 27, 28 },
{ 2, 4, 7, 13, 16, 26, 29, 42 },
{ 3, 8, 12, 17, 25, 30, 41, 43 },
{ 9, 11, 18, 24, 37, 40, 44, 53 },
{ 10, 19, 23, 32, 39, 45, 52, 54 },
{ 20, 22, 33, 38, 46, 51, 55, 60 },
{ 21, 34, 37, 47, 50, 56, 59, 61 },
{ 35, 36, 48, 49, 57, 58, 62, 63 }
};

#define W1 2841 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565  /* 2048*sqrt(2)*cos(7*pi/16) */

//*************************************************************************************
typedef char CHAR;
typedef short SHORT;
typedef long LONG;

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

typedef int HFILE;
typedef CHAR *LPSTR, *PSTR;

#define FALSE 0
#define TRUE 1

//macro definitio
#define WIDTHBYTES(i)    ((i+31)/32*4)
#define PI 3.1415926535
#define FUNC_OK 0
#define FUNC_MEMORY_ERROR 1
#define FUNC_FILE_ERROR 2
#define FUNC_FORMAT_ERROR 3


class CBitmap : public SingleTon<CBitmap>
{
	friend class SingleTon<CBitmap>;

public:
	CBitmap(void);
	virtual ~CBitmap(void);

public:
	bool Jpeg2Bmp(const char* pszSrc, int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength);

private:
	int  InitTag();
	void InitTable();
	int  Decode();
	int  DecodeMCUBlock();
	int  HufBlock(BYTE dchufindex, BYTE achufindex);
	int  DecodeElement();
	void IQtIZzMCUComponent(short flag);
	void IQtIZzBlock(short  *s, int * d, short flag);
	void GetYUV(short flag);
	void StoreBuffer();
	BYTE ReadByte();
	void Initialize_Fast_IDCT();
	void Fast_IDCT(int * block);
	void idctrow(int * blk);
	void idctcol(int * blk);
	string getModulePath();
	void BmpZoom(BITMAPFILEHEADER head, BITMAPINFOHEADER info, unsigned char** pszSrc, unsigned int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength);

private:
	//global variable declaration
	char *            hImgData = NULL;
	DWORD              NumColors;
	DWORD              LineBytes;
	DWORD              ImgWidth = 0, ImgHeight = 0;
	char*             lpPtr;
	//
	//variables used in jpeg function
	short   SampRate_Y_H, SampRate_Y_V;
	short   SampRate_U_H, SampRate_U_V;
	short   SampRate_V_H, SampRate_V_V;
	short   H_YtoU, V_YtoU, H_YtoV, V_YtoV;
	short   Y_in_MCU, U_in_MCU, V_in_MCU;
	unsigned char   *lpJpegBuf;
	unsigned char   *lp;
	short   qt_table[3][64];
	short   comp_num;
	BYTE   comp_index[3];
	BYTE      YDcIndex, YAcIndex, UVDcIndex, UVAcIndex;
	BYTE   HufTabIndex;
	short      *YQtTable, *UQtTable, *VQtTable;
	short      code_pos_table[4][16], code_len_table[4][16];
	unsigned short code_value_table[4][256];
	unsigned short huf_max_value[4][16], huf_min_value[4][16];
	short   BitPos, CurByte;
	short   rrun, vvalue;
	short   MCUBuffer[10 * 64];
	int    QtZzMCUBuffer[10 * 64];
	short   BlockBuffer[64];
	short   ycoef, ucoef, vcoef;
	BOOL   IntervalFlag;
	short   interval = 0;
	int    Y[4 * 64], U[4 * 64], V[4 * 64];
	DWORD      sizei, sizej;
	short    restart;
};

#define Bitmap CBitmap::getInstance()