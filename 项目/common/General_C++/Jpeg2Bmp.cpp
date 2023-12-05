#include "Jpeg2Bmp.h"

#include <iostream>
#include <fstream>

using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
	char signature[2];
	int fileSize;
	int reserved;
	int dataOffset;
	int headerSize;
	int width;
	int height;
	short planes;
	short bitsPerPixel;
	int compression;
	int imageSize;
	int xPixelsPerMeter;
	int yPixelsPerMeter;
	int colorsUsed;
	int importantColors;
};
#pragma pack(pop)


#pragma pack(1)
/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

static  long iclip[1024];
static  long *iclp;
BYTE   And[9] = { 0, 1, 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 0xff };

#define OLD_BMP_PATH "\\oldBmp.bmp"
#define NEW_BMP_PATH "\\newBmp.bmp"

CBitmap::CBitmap(void)
{
}

CBitmap::~CBitmap(void)
{
}

void CBitmap::GetYUV(short flag)
{
	short H, VV;
	short i, j, k, h;
	int  *buf;
	int  *pQtZzMCU;
	buf = Y;
	pQtZzMCU = QtZzMCUBuffer;
	switch (flag) {
	case 0:
		H = SampRate_Y_H;
		VV = SampRate_Y_V;
		buf = Y;
		pQtZzMCU = QtZzMCUBuffer;
		break;
	case 1:
		H = SampRate_U_H;
		VV = SampRate_U_V;
		buf = U;
		pQtZzMCU = QtZzMCUBuffer + Y_in_MCU * 64;
		break;
	case 2:
		H = SampRate_V_H;
		VV = SampRate_V_V;
		buf = V;
		pQtZzMCU = QtZzMCUBuffer + (Y_in_MCU + U_in_MCU) * 64;
		break;
	}
	for (i = 0; i < VV; i++)
		for (j = 0; j < H; j++)
			for (k = 0; k < 8; k++)
				for (h = 0; h < 8; h++)
					buf[(i * 8 + k)*SampRate_Y_H * 8 + j * 8 + h] = *pQtZzMCU++;
}

void CBitmap::StoreBuffer()
{
	short i, j;
	unsigned char  *lpbmp;
	unsigned char R, G, B;
	int y, u, v, rr, gg, bb;

	for (i = 0; i < SampRate_Y_V * 8; i++) {
		if ((sizei + i) < ImgHeight) {
			lpbmp = ((unsigned char *)lpPtr + (DWORD)(ImgHeight - sizei - i - 1)*LineBytes + sizej * 3);
			for (j = 0; j < SampRate_Y_H * 8; j++) {
				if ((sizej + j) < ImgWidth) {
					y = Y[i * 8 * SampRate_Y_H + j];
					u = U[(i / V_YtoU) * 8 * SampRate_Y_H + j / H_YtoU];
					v = V[(i / V_YtoV) * 8 * SampRate_Y_H + j / H_YtoV];
					rr = ((y << 8) + 18 * u + 367 * v) >> 8;
					gg = ((y << 8) - 159 * u - 220 * v) >> 8;
					bb = ((y << 8) + 411 * u - 29 * v) >> 8;
					R = (unsigned char)rr;
					G = (unsigned char)gg;
					B = (unsigned char)bb;
					if (rr & 0xffffff00) if (rr > 255) R = 255; else if (rr < 0) R = 0;
					if (gg & 0xffffff00) if (gg > 255) G = 255; else if (gg < 0) G = 0;
					if (bb & 0xffffff00) if (bb > 255) B = 255; else if (bb < 0) B = 0;
					*lpbmp++ = B;
					*lpbmp++ = G;
					*lpbmp++ = R;


				}
				else  break;
			}
		}
		else break;
	}
}

void CBitmap::Fast_IDCT(int * block)
{
	short i;

	for (i = 0; i < 8; i++)
		idctrow(block + 8 * i);

	for (i = 0; i < 8; i++)
		idctcol(block + i);
}

string CBitmap::getModulePath()
{
	char szDir[2048] = { 0 };
	::GetModuleFileNameA(NULL, szDir, sizeof(szDir));

	string strResult = szDir;
	strResult = strResult.substr(0, strResult.find_last_of("\\"));
	return strResult;
}

bool CBitmap::BmpZoom(BITMAPFILEHEADER head, BITMAPINFOHEADER info, unsigned char** pszSrc, unsigned int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength)
{
	//printf("BmpZoom~~调用开始!\n");

	bool bReturn = false;
	try
	{
		FILE* file = fopen(string(getModulePath() + OLD_BMP_PATH).c_str(), "wb");
		if (file == NULL)
			throw - 1;

		fwrite(*pszSrc, 1, nSrcLength, file);
		fclose(file);

		FILE *fpr1 = fopen(string(getModulePath() + OLD_BMP_PATH).c_str(), "rb");
		FILE *fpw2 = fopen(string(getModulePath() + NEW_BMP_PATH).c_str(), "wb");
		if (fpr1 == NULL || fpw2 == NULL)
			throw - 2;

		//读取原照片的头信息
		fread(&head, sizeof(BITMAPFILEHEADER), 1, fpr1);
		fread(&info, sizeof(BITMAPINFOHEADER), 1, fpr1);

		unsigned int old_width = info.biWidth;//获取原图片的宽
		unsigned int old_height = info.biHeight;//获取原图片的高

		//获取原图片的位图数据

		//printf("BmpZoom~~old_width：%d \n", old_width);
		//printf("BmpZoom~~old_height：%d \n", old_height);

		LONG srcDataLen = old_width*old_height * 3;
		//printf("BmpZoom~~srcDataLen：%d \n", srcDataLen);

		if (srcDataLen <= 0)
			throw - 3;

		unsigned char *src_data = new unsigned char[srcDataLen];

		if (src_data == NULL)
			throw - 4;

		//printf("BmpZoom~~fseek!\n");

		{ // 测试文件的大小
			int errer = _fseeki64(fpr1, 0, SEEK_END);
			if (errer != 0)
			{	// 不等0，移动文件光标失败
				printf("Failed to move cursor!(SEEK_END)\n");
				return false;
			}
			// 获得当前文件指针位置，也就是间接获得文件大小
			int fileSize = _ftelli64(fpr1);
			//printf("fpr1文件的大小：%d\n ", fileSize);

			if (fileSize <= 54)
				throw - 5;

			errer = _fseeki64(fpr1, 0, SEEK_SET);
			if (errer != 0) 
			{	// 不等0，移动文件光标失败
				printf("Failed to move cursor!(SEEK_END)\n");
				return false;
			}
			fileSize = _ftelli64(fpr1);
		}

		fseek(fpr1, 54, SEEK_SET);//向后偏移，就算超出文件，也只是指在文件尾部，不会返回-1

		fread(src_data, old_width*old_height * 3, 1, fpr1);
		fclose(fpr1);

		//printf("原图片的宽:%d\n", old_width);
		//printf("原图片的高:%d\n", old_height);

		//修改原照片的宽高
		unsigned int new_Width, new_Height;
		old_width > 480 ? new_Width = 480 : new_Width = old_width;
		old_height > 300 ? new_Height = 300 : new_Height = old_height;

		unsigned int newSize = new_Width * new_Height * 3;
		head.bfSize = newSize + 54;
		info.biWidth = new_Width;
		info.biHeight = new_Height;

		//将修改过的头信息写进新照片
		fwrite(&head, sizeof(BITMAPFILEHEADER), 1, fpw2);
		fwrite(&info, sizeof(BITMAPINFOHEADER), 1, fpw2);

		unsigned int i = 0, j = 0;
		unsigned long dwsrcX, dwsrcY;
		unsigned char *pucDest;
		unsigned char *pucSrc;
		unsigned char *dest_data = new unsigned char[newSize];
		for (i = 0; i < new_Height; i++)
		{
			dwsrcY = i * old_height / new_Height;
			pucDest = dest_data + i * new_Width * 3;
			pucSrc = src_data + dwsrcY * old_width * 3;
			for (j = 0; j < new_Width; j++)
			{
				dwsrcX = j * old_width / new_Width;
				memcpy(pucDest + j * 3, pucSrc + dwsrcX * 3, 3);//数据拷贝
			}
		}

		fseek(fpw2, 54, SEEK_SET);

		fwrite(dest_data, newSize, 1, fpw2);
		//printf("生成 压缩bmp图片 成功!\n");
		fclose(fpw2);

		nDstLength = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + newSize;
		*pszDst = new unsigned char[nDstLength];

		unsigned char* tmpData = *pszDst;
		memcpy_s(tmpData, sizeof(BITMAPFILEHEADER), &head, sizeof(BITMAPFILEHEADER));
		tmpData += sizeof(BITMAPFILEHEADER);
		memcpy_s(tmpData, sizeof(BITMAPINFOHEADER), &info, sizeof(BITMAPINFOHEADER));
		tmpData += sizeof(BITMAPINFOHEADER);
		memcpy_s(tmpData, newSize, dest_data, newSize);

		//释放堆空间
		delete[] dest_data;
		dest_data = NULL;

		delete[] src_data;
		src_data = NULL;

		return true;
	}
	catch (int& iErrorCode)
	{
		switch (iErrorCode)
		{
		case -1:
			printf("生成old bmp文件 失败!\n");
			break;
		case -2:
			printf("打开old bmp文件 失败 or 生成new bmp文件 失败!\n");
			break;
		case -3:
			printf("BmpZoom~~srcDataLen <=0 返回!\n");
			break;
		case -4:
			printf("BmpZoom~~src_data 申请内存失败!\n");
			break;
		case -5:
			printf("fpr1文件的大小，小于54，返回\n");
			break;
		}
	}

	return bReturn;
}

bool CBitmap::Bmp2Binarization(const string& bmpFilePath, unsigned char** pszDst, unsigned int& nDstLength)
{
	// 读取BMP文件头信息
	ifstream file(bmpFilePath, std::ios::binary);

	if (!file) 
	{
		printf("无法打开BMP文件 \n");
		return -1;
	}

	BMPHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

	// 检查文件是否为24位真彩色BMP图像
	if (header.signature[0] != 'B' || header.signature[1] != 'M') 
	{
		printf("不支持的BMP文件格式 \n");
		return -1;
	}

	if (header.bitsPerPixel != 24) 
	{
		printf("不支持的像素位数 \n");
		return -1;
	}

	// 计算图像数据的行字节数
	int rowSize = ((header.width * 3 + 3) / 4) * 4;

	// 创建存储图像数据的数组
	unsigned char* image_data = new unsigned char[rowSize * header.height];

	// 读取图像数据
	file.read(reinterpret_cast<char*>(image_data), rowSize * header.height);

	// 关闭文件
	file.close();

	// 图像二值化处理
	int threshold_value = 128; // 设定阈值

	for (int y = 0; y < header.height; y++) {
		for (int x = 0; x < header.width; x++) {
			int index = y * rowSize + x * 3;
			unsigned char r = image_data[index + 2];
			unsigned char g = image_data[index + 1];
			unsigned char b = image_data[index];

			unsigned char gray = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);

			if (gray > threshold_value) {
				image_data[index + 2] = 255;
				image_data[index + 1] = 255;
				image_data[index] = 255;
			}
			else {
				image_data[index + 2] = 0;
				image_data[index + 1] = 0;
				image_data[index] = 0;
			}
		}
	}

	// 保存二值化后的图像数据
	ofstream output_file(getModulePath() + "\\binary_image.bmp", ios::binary);

	if (!output_file) 
	{
		printf("无法保存二值化图像文件 \n");
		delete[] image_data;
		return -1;
	}
	printf("成功保存图像文件 \n");
	output_file.write(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
	output_file.write(reinterpret_cast<char*>(image_data), rowSize * header.height);

	// 关闭文件
	output_file.close();

	// 释放内存
	delete[] image_data;

	return true;
}

bool CBitmap::Jpeg2Bmp(const char* pszSrc, int nSrcLength, unsigned char** pszDst, unsigned int& nDstLength)
{
	FILE*      hfjpg;//打开原本jpg格式的文件
	DWORD      ImgSize;
	DWORD      BufSize, JpegBufSize;
	FILE*      hfbmp;//生成目标bmp文件
	unsigned char * hJpegBuf = NULL;//存储原本jpg格式的文件数据
	unsigned char * hImgData = NULL;//存储新的数据的变量

	int        funcret;
	LPBITMAPINFOHEADER lpImgData;
	BITMAPFILEHEADER   bf;//描述位图文件头的结构体
	BITMAPINFOHEADER   bi;//描述位图文件头信息的结构体


	//printf("传进来的图片路径：%s\n", pszSrc);
	errno_t err = fopen_s(&hfjpg, pszSrc, "rb");//正确返回0，不正确返回非0
	if (err != 0)
	{
		printf("文件打开失败\n");
	}

	//get jpg file length
	fseek(hfjpg, 0L, SEEK_END);
	JpegBufSize = ftell(hfjpg);
	//printf("原本图片数据（JpegBufSize）大小:%d\n", JpegBufSize);

	//rewind to the beginning of the file
	fseek(hfjpg, 0L, SEEK_SET);

	hJpegBuf = new unsigned char[JpegBufSize];

	if (hJpegBuf == NULL)
	{
		printf("存储原本jpg格式的文件数据（hJpegBuf） 分配内存失败，返回\n");
		return false;
	}

	lpJpegBuf = hJpegBuf;//多一个变量指向这块内存

	fread(hJpegBuf, sizeof(char), JpegBufSize, hfjpg);
	fclose(hfjpg);

	InitTable();

	if ((funcret = InitTag()) != FUNC_OK)
	{
		delete[] hJpegBuf;
		hJpegBuf = NULL;
		return false;
	}

	//create new bitmapfileheader and bitmapinfoheader
	memset((char *)&bf, 0, sizeof(BITMAPFILEHEADER));
	memset((char *)&bi, 0, sizeof(BITMAPINFOHEADER));

	bi.biSize = (DWORD)sizeof(BITMAPINFOHEADER);
	bi.biWidth = (LONG)(ImgWidth);
	bi.biHeight = (LONG)(ImgHeight);
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bi.biCompression = BI_RGB;
	NumColors = 0;
	LineBytes = (DWORD)WIDTHBYTES(bi.biWidth*bi.biBitCount);
	ImgSize = (DWORD)LineBytes*bi.biHeight;
	bf.bfType = 0x4d42;

	//printf("描述位图文件头的结构体（BITMAPFILEHEADER）：%d\n", sizeof(BITMAPFILEHEADER));//如果没有#pragma pack(1)，sizeof(BITMAPFILEHEADER)是16
	//printf("描述位图文件头信息的结构体（BITMAPINFOHEADER）：%d\n", sizeof(BITMAPINFOHEADER));
	//printf("NumColors * sizeof(RGBQUAD)：%d\n", NumColors * sizeof(RGBQUAD));

	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD) + ImgSize;
	bf.bfOffBits = 54;//位图数据的偏移量，即位图文件头和位图信息头之后的字节数
	BufSize = bf.bfSize - sizeof(BITMAPFILEHEADER); //描述位图文件头的结构体，文件头的结构体的大小是固定的，所以先减去，后面直接复制文件头的结构体即可

	//printf("文件大小(bf.bfSize)：%d\n", bf.bfSize);
	//printf("文件大小 - 位图文件头的结构体 = BufSize：%d\n", BufSize);

	if (BufSize <= 0)
	{
		delete[] hJpegBuf;
		hJpegBuf = NULL;
		return FALSE;
	}

	hImgData = new unsigned char[BufSize];
	if (hImgData == NULL)
	{
		printf("hImgData 申请内存失败！\n");

		delete[] hJpegBuf;
		hJpegBuf = NULL;
		return FALSE;
	}

	//printf("存储新的数据的变量（hImgData） 申请内存成功！\n");
	lpImgData = (LPBITMAPINFOHEADER)hImgData;

	//printf("sizeof(BITMAPINFOHEADER)：%d\n", sizeof(BITMAPINFOHEADER));
	//printf("sizeof(bi)：%d\n", sizeof(bi));
	memcpy(lpImgData, (char *)&bi, sizeof(BITMAPINFOHEADER));//描述位图文件头信息的结构体

	if (lpImgData==NULL)
	{
		printf("lpImgData = NULL 返回 \n");
		return false;
	}
	//printf("sizeof(lpImgData)：%d\n", sizeof(lpImgData));

	lpPtr = (char *)lpImgData + sizeof(BITMAPINFOHEADER);//mencpy完，指针往后移，并赋给新的全局指针（其他函数做一些格式转换处理）

	if ((SampRate_Y_H == 0) || (SampRate_Y_V == 0))
	{
		delete[] hJpegBuf;
		hJpegBuf = NULL;

		delete[] hImgData;
		hImgData = NULL;

		printf("(SampRate_Y_H == 0) || (SampRate_Y_V == 0) 失败，返回\n");
		return false;
	}

	//printf("Decode接口，调用开始\n");
	funcret = Decode();//格式转换的主要函数
	//printf("Decode接口，调用结束\n");

	bool retOK = false;
	if (funcret == FUNC_OK)
	{
		//生成bmp图片（可以不生成这个文件的，只是为了看）
		fopen_s(&hfbmp, string(getModulePath() + "\\jpeg2bmp.bmp").c_str(), "wb");
		fwrite((LPSTR)&bf, sizeof(BITMAPFILEHEADER), 1, hfbmp);
		fwrite((LPSTR)lpImgData, sizeof(char), BufSize, hfbmp);
		fclose(hfbmp);
		//printf(" 生成目标 bmp 图片成功 \n");

		{
			//bmp图片转为二进制
			unsigned int nSrcLength = BufSize + sizeof(BITMAPFILEHEADER);//生成目标bmp文件的大小
			//printf("生成目标bmp文件的大小(nSrcLength)：%d\n", nSrcLength);

			unsigned char *pszSrcZoom = new unsigned char[nSrcLength]; //申请内存，主要是想把bmp文件的数据读出来
			if (pszSrcZoom == NULL)
			{
				printf("pszSrcZoom 申请内存失败！\n");

				delete[] hJpegBuf;
				hJpegBuf = NULL;

				delete[] hImgData;
				hImgData = NULL;

				return false;
			}

			unsigned char* tmpData = pszSrcZoom;

			memcpy_s(tmpData, sizeof(BITMAPFILEHEADER), &bf, sizeof(BITMAPFILEHEADER));//位图的文件头结构体
			tmpData += sizeof(BITMAPFILEHEADER);//指针往后移

			memcpy_s(tmpData, BufSize, lpImgData, BufSize);//复制bmp的数据

			//printf("测试 ~~~~aaaaaaaaaaaaa\n");

			unsigned char* pszDstZoom = NULL;//存bmp图片压缩后的数据
			bool retBmpZoom = BmpZoom(bf, *lpImgData, &pszSrcZoom, nSrcLength, &pszDstZoom, nDstLength);

			if (pszSrcZoom != NULL)
			{
				delete[] pszSrcZoom;
				pszSrcZoom = NULL;
			}

			if (retBmpZoom && pszDstZoom != NULL)
			{
				*pszDst = new unsigned char[nDstLength];
				if (*pszDst == NULL)
				{
					printf("*pszDst 申请内存失败！\n");

					delete[] hJpegBuf;
					hJpegBuf = NULL;
					delete[] hImgData;
					hImgData = NULL;

					return false;
				}

				memcpy_s(*pszDst, nDstLength, pszDstZoom, nDstLength);
				delete[] pszDstZoom;
				pszDstZoom = NULL;

				retOK = true;
			}
		}

	}

	delete[] hJpegBuf;
	hJpegBuf = NULL;

	delete[] hImgData;
	hImgData = NULL;

	//retOK ? printf("retBmpZoom压缩成功!\n") : printf("retBmpZoom压缩失败!\n");
	return retOK;
}

int CBitmap::InitTag()
{
	BOOL finish = FALSE;
	BYTE id;
	short  llength;
	short  i, j, k;
	short  huftab1, huftab2;
	short  huftabindex;
	BYTE hf_table_index;
	BYTE qt_table_index;
	BYTE comnum;

	unsigned char  *lptemp;
	short  ccount;

	lp = lpJpegBuf + 2;

	while (!finish) {
		id = *(lp + 1);
		lp += 2;
		switch (id) {
		case M_APP0:
			llength = MAKEWORD(*(lp + 1), *lp);
			lp += llength;
			break;
		case M_DQT:
			llength = MAKEWORD(*(lp + 1), *lp);
			qt_table_index = (*(lp + 2)) & 0x0f;
			lptemp = lp + 3;
			if (llength < 80) {
				for (i = 0; i < 64; i++)
					qt_table[qt_table_index][i] = (short)*(lptemp++);
			}
			else {
				for (i = 0; i < 64; i++)
					qt_table[qt_table_index][i] = (short)*(lptemp++);
				qt_table_index = (*(lptemp++)) & 0x0f;
				for (i = 0; i < 64; i++)
					qt_table[qt_table_index][i] = (short)*(lptemp++);
			}
			lp += llength;
			break;
		case M_SOF0:
			llength = MAKEWORD(*(lp + 1), *lp);
			ImgHeight = MAKEWORD(*(lp + 4), *(lp + 3));
			ImgWidth = MAKEWORD(*(lp + 6), *(lp + 5));
			comp_num = *(lp + 7);
			if ((comp_num != 1) && (comp_num != 3))
				return FUNC_FORMAT_ERROR;
			if (comp_num == 3) {
				comp_index[0] = *(lp + 8);
				SampRate_Y_H = (*(lp + 9)) >> 4;
				SampRate_Y_V = (*(lp + 9)) & 0x0f;
				YQtTable = (short *)qt_table[*(lp + 10)];

				comp_index[1] = *(lp + 11);
				SampRate_U_H = (*(lp + 12)) >> 4;
				SampRate_U_V = (*(lp + 12)) & 0x0f;
				UQtTable = (short *)qt_table[*(lp + 13)];

				comp_index[2] = *(lp + 14);
				SampRate_V_H = (*(lp + 15)) >> 4;
				SampRate_V_V = (*(lp + 15)) & 0x0f;
				VQtTable = (short *)qt_table[*(lp + 16)];
			}
			else {
				comp_index[0] = *(lp + 8);
				SampRate_Y_H = (*(lp + 9)) >> 4;
				SampRate_Y_V = (*(lp + 9)) & 0x0f;
				YQtTable = (short *)qt_table[*(lp + 10)];

				comp_index[1] = *(lp + 8);
				SampRate_U_H = 1;
				SampRate_U_V = 1;
				UQtTable = (short *)qt_table[*(lp + 10)];

				comp_index[2] = *(lp + 8);
				SampRate_V_H = 1;
				SampRate_V_V = 1;
				VQtTable = (short *)qt_table[*(lp + 10)];
			}
			lp += llength;
			break;
		case M_DHT:
			llength = MAKEWORD(*(lp + 1), *lp);
			if (llength < 0xd0) {
				huftab1 = (short)(*(lp + 2)) >> 4;     //huftab1=0,1
				huftab2 = (short)(*(lp + 2)) & 0x0f;   //huftab2=0,1
				huftabindex = huftab1 * 2 + huftab2;
				lptemp = lp + 3;
				for (i = 0; i < 16; i++)
					code_len_table[huftabindex][i] = (short)(*(lptemp++));
				j = 0;
				for (i = 0; i < 16; i++)
					if (code_len_table[huftabindex][i] != 0) {
						k = 0;
						while (k < code_len_table[huftabindex][i]) {
							code_value_table[huftabindex][k + j] = (short)(*(lptemp++));
							k++;
						}
						j += k;
					}
				i = 0;
				while (code_len_table[huftabindex][i] == 0)
					i++;
				for (j = 0; j < i; j++) {
					huf_min_value[huftabindex][j] = 0;
					huf_max_value[huftabindex][j] = 0;
				}
				huf_min_value[huftabindex][i] = 0;
				huf_max_value[huftabindex][i] = code_len_table[huftabindex][i] - 1;
				for (j = i + 1; j < 16; j++) {
					huf_min_value[huftabindex][j] = (huf_max_value[huftabindex][j - 1] + 1) << 1;
					huf_max_value[huftabindex][j] = huf_min_value[huftabindex][j] + code_len_table[huftabindex][j] - 1;
				}
				code_pos_table[huftabindex][0] = 0;
				for (j = 1; j < 16; j++)
					code_pos_table[huftabindex][j] = code_len_table[huftabindex][j - 1] + code_pos_table[huftabindex][j - 1];
				lp += llength;
			}  //if
			else {
				hf_table_index = *(lp + 2);
				lp += 2;
				while (hf_table_index != 0xff) {
					huftab1 = (short)hf_table_index >> 4;     //huftab1=0,1
					huftab2 = (short)hf_table_index & 0x0f;   //huftab2=0,1
					huftabindex = huftab1 * 2 + huftab2;
					lptemp = lp + 1;
					ccount = 0;
					for (i = 0; i < 16; i++) {
						code_len_table[huftabindex][i] = (short)(*(lptemp++));
						ccount += code_len_table[huftabindex][i];
					}
					ccount += 17;
					j = 0;
					for (i = 0; i < 16; i++)
						if (code_len_table[huftabindex][i] != 0) {
							k = 0;
							while (k < code_len_table[huftabindex][i])
							{
								code_value_table[huftabindex][k + j] = (short)(*(lptemp++));
								k++;
							}
							j += k;
						}
					i = 0;
					while (code_len_table[huftabindex][i] == 0)
						i++;
					for (j = 0; j < i; j++) {
						huf_min_value[huftabindex][j] = 0;
						huf_max_value[huftabindex][j] = 0;
					}
					huf_min_value[huftabindex][i] = 0;
					huf_max_value[huftabindex][i] = code_len_table[huftabindex][i] - 1;
					for (j = i + 1; j < 16; j++) {
						huf_min_value[huftabindex][j] = (huf_max_value[huftabindex][j - 1] + 1) << 1;
						huf_max_value[huftabindex][j] = huf_min_value[huftabindex][j] + code_len_table[huftabindex][j] - 1;
					}
					code_pos_table[huftabindex][0] = 0;
					for (j = 1; j < 16; j++)
						code_pos_table[huftabindex][j] = code_len_table[huftabindex][j - 1] + code_pos_table[huftabindex][j - 1];
					lp += ccount;
					hf_table_index = *lp;
				}  //while
			}  //else
			break;
		case M_DRI:
			llength = MAKEWORD(*(lp + 1), *lp);
			restart = MAKEWORD(*(lp + 3), *(lp + 2));
			lp += llength;
			break;
		case M_SOS:
			llength = MAKEWORD(*(lp + 1), *lp);
			comnum = *(lp + 2);
			if (comnum != comp_num)
				return FUNC_FORMAT_ERROR;
			lptemp = lp + 3;
			for (i = 0; i < comp_num; i++) {
				if (*lptemp == comp_index[0]) {
					YDcIndex = (*(lptemp + 1)) >> 4;   //Y
					YAcIndex = ((*(lptemp + 1)) & 0x0f) + 2;
				}
				else {
					UVDcIndex = (*(lptemp + 1)) >> 4;   //U,V
					UVAcIndex = ((*(lptemp + 1)) & 0x0f) + 2;
				}
				lptemp += 2;
			}
			lp += llength;
			finish = TRUE;
			break;
		case M_EOI:
			return FUNC_FORMAT_ERROR;
			break;
		default:
			if ((id & 0xf0) != 0xd0) {
				llength = MAKEWORD(*(lp + 1), *lp);
				lp += llength;
			}
			else lp += 2;
			break;
		}  //switch
	} //while
	return FUNC_OK;
}

void CBitmap::InitTable()
{
	short i, j;
	sizei = sizej = 0;
	ImgWidth = ImgHeight = 0;
	rrun = vvalue = 0;
	BitPos = 0;
	CurByte = 0;
	IntervalFlag = FALSE;
	restart = 0;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 64; j++)
			qt_table[i][j] = 0;
	comp_num = 0;
	HufTabIndex = 0;
	for (i = 0; i < 3; i++)
		comp_index[i] = 0;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 16; j++) {
			code_len_table[i][j] = 0;
			code_pos_table[i][j] = 0;
			huf_max_value[i][j] = 0;
			huf_min_value[i][j] = 0;
		}
	for (i = 0; i < 4; i++)
		for (j = 0; j < 256; j++)
			code_value_table[i][j] = 0;

	for (i = 0; i < 10 * 64; i++) {
		MCUBuffer[i] = 0;
		QtZzMCUBuffer[i] = 0;
	}
	for (i = 0; i < 64; i++) {
		Y[i] = 0;
		U[i] = 0;
		V[i] = 0;
		BlockBuffer[i] = 0;
	}
	ycoef = ucoef = vcoef = 0;
}

int CBitmap::Decode()
{
	int funcret;

	Y_in_MCU = SampRate_Y_H * SampRate_Y_V;
	U_in_MCU = SampRate_U_H * SampRate_U_V;
	V_in_MCU = SampRate_V_H * SampRate_V_V;
	H_YtoU = SampRate_Y_H / SampRate_U_H;
	V_YtoU = SampRate_Y_V / SampRate_U_V;
	H_YtoV = SampRate_Y_H / SampRate_V_H;
	V_YtoV = SampRate_Y_V / SampRate_V_V;
	Initialize_Fast_IDCT();
	while ((funcret = DecodeMCUBlock()) == FUNC_OK) {
		interval++;
		if ((restart) && (interval % restart == 0))
			IntervalFlag = TRUE;
		else
			IntervalFlag = FALSE;
		IQtIZzMCUComponent(0);
		IQtIZzMCUComponent(1);
		IQtIZzMCUComponent(2);
		GetYUV(0);
		GetYUV(1);
		GetYUV(2);
		StoreBuffer();
		sizej += SampRate_Y_H * 8;
		if (sizej >= ImgWidth) {
			sizej = 0;
			sizei += SampRate_Y_V * 8;
		}
		if ((sizej == 0) && (sizei >= ImgHeight))
			break;
	}
	return funcret;
}

int CBitmap::DecodeMCUBlock()
{
	short *lpMCUBuffer;
	short i, j;
	int funcret;

	if (IntervalFlag) {
		lp += 2;
		ycoef = ucoef = vcoef = 0;
		BitPos = 0;
		CurByte = 0;
	}
	switch (comp_num) {
	case 3:
		lpMCUBuffer = MCUBuffer;
		for (i = 0; i < SampRate_Y_H*SampRate_Y_V; i++)  //Y
		{
			funcret = HufBlock(YDcIndex, YAcIndex);
			if (funcret != FUNC_OK)
				return funcret;
			BlockBuffer[0] = BlockBuffer[0] + ycoef;
			ycoef = BlockBuffer[0];
			for (j = 0; j < 64; j++)
				*lpMCUBuffer++ = BlockBuffer[j];
		}
		for (i = 0; i < SampRate_U_H*SampRate_U_V; i++)  //U
		{
			funcret = HufBlock(UVDcIndex, UVAcIndex);
			if (funcret != FUNC_OK)
				return funcret;
			BlockBuffer[0] = BlockBuffer[0] + ucoef;
			ucoef = BlockBuffer[0];
			for (j = 0; j < 64; j++)
				*lpMCUBuffer++ = BlockBuffer[j];
		}
		for (i = 0; i < SampRate_V_H*SampRate_V_V; i++)  //V
		{
			funcret = HufBlock(UVDcIndex, UVAcIndex);
			if (funcret != FUNC_OK)
				return funcret;
			BlockBuffer[0] = BlockBuffer[0] + vcoef;
			vcoef = BlockBuffer[0];
			for (j = 0; j < 64; j++)
				*lpMCUBuffer++ = BlockBuffer[j];
		}
		break;
	case 1:
		lpMCUBuffer = MCUBuffer;
		funcret = HufBlock(YDcIndex, YAcIndex);
		if (funcret != FUNC_OK)
			return funcret;
		BlockBuffer[0] = BlockBuffer[0] + ycoef;
		ycoef = BlockBuffer[0];
		for (j = 0; j < 64; j++)
			*lpMCUBuffer++ = BlockBuffer[j];
		for (i = 0; i < 128; i++)
			*lpMCUBuffer++ = 0;
		break;
	default:
		return FUNC_FORMAT_ERROR;
	}
	return FUNC_OK;
}

int CBitmap::HufBlock(BYTE dchufindex, BYTE achufindex)
{
	short count = 0;
	short i;
	int funcret;

	//dc
	HufTabIndex = dchufindex;
	funcret = DecodeElement();
	if (funcret != FUNC_OK)
		return funcret;

	BlockBuffer[count++] = vvalue;
	//ac
	HufTabIndex = achufindex;
	while (count < 64) {
		funcret = DecodeElement();
		if (funcret != FUNC_OK)
			return funcret;
		if ((rrun == 0) && (vvalue == 0)) {
			for (i = count; i < 64; i++)
				BlockBuffer[i] = 0;
			count = 64;
		}
		else {
			for (i = 0; i < rrun; i++)
				BlockBuffer[count++] = 0;
			BlockBuffer[count++] = vvalue;
		}
	}
	return FUNC_OK;
}

int CBitmap::DecodeElement()
{
	int thiscode, tempcode;
	unsigned short temp, valueex;
	short codelen;
	BYTE hufexbyte, runsize, tempsize, sign;
	BYTE newbyte, lastbyte;

	if (BitPos >= 1) {
		BitPos--;
		thiscode = (BYTE)CurByte >> BitPos;
		CurByte = CurByte & And[BitPos];
	}
	else {
		lastbyte = ReadByte();
		BitPos--;
		newbyte = CurByte & And[BitPos];
		thiscode = lastbyte >> 7;
		CurByte = newbyte;
	}
	codelen = 1;
	while ((thiscode < huf_min_value[HufTabIndex][codelen - 1]) ||
		(code_len_table[HufTabIndex][codelen - 1] == 0) ||
		(thiscode > huf_max_value[HufTabIndex][codelen - 1]))
	{
		if (BitPos >= 1) {
			BitPos--;
			tempcode = (BYTE)CurByte >> BitPos;
			CurByte = CurByte & And[BitPos];
		}
		else {
			lastbyte = ReadByte();
			BitPos--;
			newbyte = CurByte & And[BitPos];
			tempcode = (BYTE)lastbyte >> 7;
			CurByte = newbyte;
		}
		thiscode = (thiscode << 1) + tempcode;
		codelen++;
		if (codelen > 16)
			return FUNC_FORMAT_ERROR;
	}  //while
	temp = thiscode - huf_min_value[HufTabIndex][codelen - 1] + code_pos_table[HufTabIndex][codelen - 1];
	hufexbyte = (BYTE)code_value_table[HufTabIndex][temp];
	rrun = (short)(hufexbyte >> 4);
	runsize = hufexbyte & 0x0f;
	if (runsize == 0) {
		vvalue = 0;
		return FUNC_OK;
	}
	tempsize = runsize;
	if (BitPos >= runsize) {
		BitPos -= runsize;
		valueex = (BYTE)CurByte >> BitPos;
		CurByte = CurByte & And[BitPos];
	}
	else {
		valueex = CurByte;
		tempsize -= BitPos;
		while (tempsize > 8) {
			lastbyte = ReadByte();
			valueex = (valueex << 8) + (BYTE)lastbyte;
			tempsize -= 8;
		}  //while
		lastbyte = ReadByte();
		BitPos -= tempsize;
		valueex = (valueex << tempsize) + (lastbyte >> BitPos);
		CurByte = lastbyte & And[BitPos];
	}  //else
	sign = valueex >> (runsize - 1);
	if (sign)
		vvalue = valueex;
	else {
		valueex = valueex ^ 0xffff;
		temp = 0xffff << runsize;
		vvalue = -(short)(valueex^temp);
	}
	return FUNC_OK;
}

void CBitmap::IQtIZzMCUComponent(short flag)
{
	short H, VV;
	short i, j;
	int *pQtZzMCUBuffer;
	short  *pMCUBuffer;
	pMCUBuffer = MCUBuffer;
	pQtZzMCUBuffer = QtZzMCUBuffer;
	switch (flag) {
	case 0:
		H = SampRate_Y_H;
		VV = SampRate_Y_V;
		pMCUBuffer = MCUBuffer;
		pQtZzMCUBuffer = QtZzMCUBuffer;
		break;
	case 1:
		H = SampRate_U_H;
		VV = SampRate_U_V;
		pMCUBuffer = MCUBuffer + Y_in_MCU * 64;
		pQtZzMCUBuffer = QtZzMCUBuffer + Y_in_MCU * 64;
		break;
	case 2:
		H = SampRate_V_H;
		VV = SampRate_V_V;
		pMCUBuffer = MCUBuffer + (Y_in_MCU + U_in_MCU) * 64;
		pQtZzMCUBuffer = QtZzMCUBuffer + (Y_in_MCU + U_in_MCU) * 64;
		break;
	}
	for (i = 0; i < VV; i++)
		for (j = 0; j < H; j++)
			IQtIZzBlock(pMCUBuffer + (i*H + j) * 64, pQtZzMCUBuffer + (i*H + j) * 64, flag);
}

void CBitmap::IQtIZzBlock(short  *s, int * d, short flag)
{
	short i, j;
	short tag;
	short *pQt;
	int buffer2[8][8];
	int *buffer1;
	short offset;
	pQt = YQtTable;
	switch (flag) {
	case 0:
		pQt = YQtTable;
		offset = 128;
		break;
	case 1:
		pQt = UQtTable;
		offset = 0;
		break;
	case 2:
		pQt = VQtTable;
		offset = 0;
		break;
	}

	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++) {
			tag = Zig_Zag[i][j];
			buffer2[i][j] = (int)s[tag] * (int)pQt[tag];
		}
	buffer1 = (int *)buffer2;
	Fast_IDCT(buffer1);
	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
			d[i * 8 + j] = buffer2[i][j] + offset;
}

BYTE CBitmap::ReadByte()
{
	BYTE  i;

	i = *(lp++);
	if (i == 0xff)
		lp++;
	BitPos = 8;
	CurByte = i;
	return i;
}

void CBitmap::Initialize_Fast_IDCT()
{
	short i;

	iclp = iclip + 512;
	for (i = -512; i < 512; i++)
		iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
}

void CBitmap::idctrow(int * blk)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	//intcut
	if (!((x1 = blk[4] << 11) | (x2 = blk[6]) | (x3 = blk[2]) |
		(x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
	{
		blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
		return;
	}
	x0 = (blk[0] << 11) + 128; // for proper rounding in the fourth stage 
	//first stage
	x8 = W7 * (x4 + x5);
	x4 = x8 + (W1 - W7)*x4;
	x5 = x8 - (W1 + W7)*x5;
	x8 = W3 * (x6 + x7);
	x6 = x8 - (W3 - W5)*x6;
	x7 = x8 - (W3 + W5)*x7;
	//second stage
	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2);
	x2 = x1 - (W2 + W6)*x2;
	x3 = x1 + (W2 - W6)*x3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;
	//third stage
	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;
	//fourth stage
	blk[0] = (x7 + x1) >> 8;
	blk[1] = (x3 + x2) >> 8;
	blk[2] = (x0 + x4) >> 8;
	blk[3] = (x8 + x6) >> 8;
	blk[4] = (x8 - x6) >> 8;
	blk[5] = (x0 - x4) >> 8;
	blk[6] = (x3 - x2) >> 8;
	blk[7] = (x7 - x1) >> 8;
}

void CBitmap::idctcol(int * blk)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	//intcut
	if (!((x1 = (blk[8 * 4] << 8)) | (x2 = blk[8 * 6]) | (x3 = blk[8 * 2]) |
		(x4 = blk[8 * 1]) | (x5 = blk[8 * 7]) | (x6 = blk[8 * 5]) | (x7 = blk[8 * 3])))
	{
		blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] = blk[8 * 5]
			= blk[8 * 6] = blk[8 * 7] = iclp[(blk[8 * 0] + 32) >> 6];
		return;
	}
	x0 = (blk[8 * 0] << 8) + 8192;
	//first stage
	x8 = W7 * (x4 + x5) + 4;
	x4 = (x8 + (W1 - W7)*x4) >> 3;
	x5 = (x8 - (W1 + W7)*x5) >> 3;
	x8 = W3 * (x6 + x7) + 4;
	x6 = (x8 - (W3 - W5)*x6) >> 3;
	x7 = (x8 - (W3 + W5)*x7) >> 3;
	//second stage
	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2) + 4;
	x2 = (x1 - (W2 + W6)*x2) >> 3;
	x3 = (x1 + (W2 - W6)*x3) >> 3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;
	//third stage
	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;
	//fourth stage
	blk[8 * 0] = iclp[(x7 + x1) >> 14];
	blk[8 * 1] = iclp[(x3 + x2) >> 14];
	blk[8 * 2] = iclp[(x0 + x4) >> 14];
	blk[8 * 3] = iclp[(x8 + x6) >> 14];
	blk[8 * 4] = iclp[(x8 - x6) >> 14];
	blk[8 * 5] = iclp[(x0 - x4) >> 14];
	blk[8 * 6] = iclp[(x3 - x2) >> 14];
	blk[8 * 7] = iclp[(x7 - x1) >> 14];
}

