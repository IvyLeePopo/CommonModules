/*
 * @Description: QTextStreama的常用用法
 * @Author: Ivy
 * @Date: 2022-05-09 09:34:43
 * @LastEditTime: 2022-05-09 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

/**
* QTextStream 是一个用于读写文本的 Qt 类，可以从文件、字节数组、标准输入输出设备等读取和写入文本。
	它提供了一系列方便的方法来读取和写入文本数据，例如 readLine()、readAll()、write()、operator<< 等。
*/

//1、构造函数
void newQTextStream();

//2、读取操作
void read();

//3、写入操作
void write();

//4、格式化输出
void cout();

//5、读写位置控制
void ctrPos();

//6、字符编码设置
void setCode();
