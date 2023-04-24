#include "CFile.h"

/**
*	常见的文件打开模式mode包括：
	“r”：只读模式，打开文件后只能读取文件内容，不能进行修改操作。
	“w”：写入模式，打开文件后可以写入文件内容，如果文件不存在则会创建新文件，如果文件已存在则会清空文件内容。
	“a”：追加模式，打开文件后可以在文件末尾追加内容，如果文件不存在则会创建新文件。
	“x”：独占写入模式，打开文件后只能进行写入操作，如果文件已存在则会报错。
	“b”：二进制模式，打开文件后以二进制方式读取或写入文件内容。
	“+”：读写模式，打开文件后既可以读取文件内容也可以写入文件内容。
	其中，“r”、“w”、“a”三种模式可以与“b”、“+”组合使用，例如“rb”表示以二进制方式读取文件内容。
*/


/**
	用于打开文件并返回一个文件指针:
	@ filename 是要打开的文件名,可以是相对路径或绝对路径；
	@ mode 是打开文件的模式:
		"r"：只读模式，打开一个已存在的文本文件，如果文件不存在则返回 NULL。
		"w"：写入模式，打开一个文本文件，如果文件不存在则创建新文件，如果文件已存在则清空文件内容。
		"a"：追加模式，打开一个文本文件，如果文件不存在则创建新文件，如果文件已存在则在文件末尾追加内容。
		"rb"、"wb"、"ab"：与上述模式类似，但是用于二进制文件。
	@返回一个指向 FILE 结构体的指针，该结构体包含了文件的描述信息和文件指针等属性。
		如果打开文件失败，则返回 NULL。
*/
FILE * fopen(const char * filename, const char * mode)
{
	FILE *fpr1 = fopen("newbmp.bmp", "rb"); // 打开文件
	fclose(file); //在使用完文件后，应该使用 fclose() 函数关闭文件，以释放文件资源。
}

// 例子：
/**
 *  1、读文件：从由fp指定的文件中，按二进制形式将sife*count个数据读到由buffer指出的数据区中。
 *
 * @buffer：这是一个void型指针，指出要将读入数据存放在其中的存储区首地址
 * @sife：指出一个数据块的字节数，即一个数据块的大小尺寸
 * @count：指出一次读入多少个数据块（sife）
 * @fp：这是个文件指针，指出要从其中读出数据的文件。
 * @返回值
　　正常返回：实际读取数据块的个数，即count。
　　异常返回：如果文件中剩下的数据块个数少于参数中count指出的个数，或者发生了错误，返回0值。此时可以用feof()和ferror()来
*/
int fread(void * buffer, unsigned sife, unsigned count, FILE * fp)
{
	FILE *fpr1 = fopen("newbmp.bmp", "rb"); // 打开文件
	int head;								// 定义一个buffer
	fread(&head, sizeof(int), 1, fpr1);
	fclose(file);
}

/**
 *  2、写文件：按二进制形式，将由buffer指定的数据缓冲区内的sife*count个数据写入由fp指定的文件中去。
 *
 * @buffer：这是一个void型指针，指出要将其中数据输出到文件的缓冲区首地址。
 * @sife：指出一个数据块的字节数，即一个数据块的大小尺寸
 * @count：指出一次读入多少个数据块（sife）
 * @fp：这是个文件指针，指出要写进数据的文件。
 * 返回值
　　正常返回：实际输出数据块的个数，即count。
　　异常返回：返回0值，表示输出结束或发生了错误。
*/
int fwrite(void * buffer, unsigned sife, unsigned count, FILE * fp)
{
	FILE *file = fopen("ok.bmp", "wb");// 打开文件
	fwrite(&head, sizeof(BITMAPFILEHEADER), 1, file);
	fclose(file);
}

/***
 * 3、fseek 是一个文件操作函数，用于在文件中定位指针的位置。它可以将文件指针移动到文件的任何位置，以便读取或写入数据。
 * @stream 是指向文件的指针，
 * @offset 是要移动的字节数，
 * @whence 是指定起始位置的标识符。
 * 		whence 可以取以下三个值：
		SEEK_SET：从文件开头开始偏移 offset 个字节。
		SEEK_CUR：从当前位置开始偏移 offset 个字节。
		SEEK_END：从文件末尾开始偏移 offset 个字节。
 * @返回值为 0 表示成功，非零值表示失败。
*/
int fseek(FILE * stream, long int offset, int whence)
{
	FILE *file = fopen("ok.bmp", "wb");// 打开文件
	fseek(file, 54, SEEK_SET);
	fseek(file, 0L, SEEK_SET);//文件指针移动到文件末尾，其第二个参数0L表示偏移量为0，该语句常用于获取文件大小。
	fclose(file);
}

/***
* 在文件流中获取当前文件指针的位置
* 注意：每次读写文件都会改变文件指针的位置。在使用 ftell 之前，需要先打开文件流并进行读写操作
		在处理大型文件时，建议使用更加安全的函数 fseek 和 ftello。
*/
long int ftell(FILE * stream)
{
	fseek(file, 0L, SEEK_END);//该语句将文件指针hfjpg移动到文件末尾，其第二个参数0L表示偏移量为0
	long int JpegBufSize = ftell(hfjpg);
}

//获取文件名为filename的文件大小。
//1、用C语言自带的stat函数：结构体里面涵盖文件的所有数据，有文件大小
int file_size_stat(char* filename)
{
	struct stat statbuf;
	int ret;
	ret = stat(filename, &statbuf);//调用stat函数
	if (ret != 0) return -1;//获取失败。
	return statbuf.st_size;//返回文件大小。
}

//	以fopen打开的文件，通过fseek可以定位到文件尾，这时使用ftell函数，返回的文件指针偏移值，就是文件的实际大小。
int file_size_fseek(char * filename)
{
	FILE *fp = fopen(filename, "rb");//打开文件。
	int size;
	if (fp == NULL) // 打开文件失败
		return -1;
	fseek(fp, 0, SEEK_END);//定位文件指针到文件尾。
	size = ftell(fp);//获取文件指针偏移量，即文件大小。
	fclose(fp);//关闭文件。
	return size;
}