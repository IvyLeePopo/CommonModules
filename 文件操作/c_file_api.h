/*
 * @Description: C文件常用的函数（ C 标准库函数）
 * @Author: Ivy
 * @Date: 2022-04-24 09:34:43
 * @LastEditTime: 2023-02-24 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#include <sys/stat.h>
#include <stdio.h>

 /**
	1. fopen() 打开流
	2. fclose() 关闭流
	3. fputc() 写一个字符到流中
	4. fgetc() 从流中读一个字符
	5. fseek() 在流中定位到指定的字符
	6. fputs() 写字符串到流
	7. fgets() 从流中读一行或指定个字符
	8. fprintf() 按格式输出到流
	9. fscanf() 从流中按格式读取
	10. feof() 到达文件尾时返回真值
	11. ferror() 发生错误时返回其值
	12. rewind() 复位文件定位器到文件开始处
	13. remove() 删除文件
	14. fread() 从流中读指定个数的字符：
	15. fwrite() 向流中写指定个数的字符
	16. tmpfile() 生成一个临时文件流
	17. tmpnam() 生成一个唯一的文件名
	18. ftell() 在文件流中获取当前文件指针的位置
 */

 /*
	 定制api
	 1. 获取文件的大小
 */

//用于打开文件并返回一个文件指针
FILE *fopen(const char *filename, const char *mode);

//读文件：从由fp指定的文件中，按二进制形式将sife*count个数据读到由buffer指出的数据区中。
int fread(void *buffer, unsigned sife, unsigned count, FILE *fp);

//写文件：按二进制形式，将由buffer指定的数据缓冲区内的sife*count个数据写入由fp指定的文件中去
int fwrite(void *buffer, unsigned sife, unsigned count, FILE *fp);

//用于在文件中定位指针的位置。它可以将文件指针移动到文件的任何位置，以便读取或写入数据
int fseek(FILE *stream, long int offset, int whence);

//在文件流中获取当前文件指针的位置
long int ftell(FILE *stream);

//获取文件名为filename的文件大小。
int file_size_stat(char* filename);//用C语言自带的stat函数
int file_size_fseek(char* filename);//利用fseek函数
