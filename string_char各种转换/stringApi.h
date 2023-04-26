/*
 * @Description: C语言中string字符串api，以及字符串输出格式的api
 * @Author: Ivy
 * @Date: 2022-04-26 09:34:43
 * @LastEditTime: 2023-02-24 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


//用于在给定字符串中查找指定字符的第一个出现位置
char *strchr(const char *str, int c);

//可以方便地将格式化的数据输出到指定的缓冲区中，避免了输出到标准输出流的问题，并且可以自动处理缓冲区溢出的情况。
int snprintf(char *str, size_t size, const char *format, ...);