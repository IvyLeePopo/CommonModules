/*
 * @Description: Colro的功能
 * @Author: Ivy
 * @Date: 2022-04-25 09:34:43
 * @LastEditTime: 2023-02-25 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */
#pragma once
#include<QString>

namespace ColorDef
{
	// http://idoc.corp.4dstc.com/app/mjoTnZplSP/develop/design/yqmU-Kimd1
	// 说明：纯色只有一个值，渐变色有两个值，以L和R结尾
	extern const char* clr_primary_n;   // 主色默认状态

	// alpha:0~255, color的透明度被设为alpha的值
	QString combineAlpha(int alpha, const QString& color);
	// alpha:0~1,color的透明度被设为alpha的值
	QString combineAlphaF(qreal alpha, const QString& color);
	// 颜色叠加 alpha:0~255, color的透明度被设为alpha的值
	QString overlay(int alphaBackground, const QString& colorBackground, int alphaForeground, const QString& colorForeground);
	// 颜色叠加 alpha:0~1,color的透明度被设为alpha的值
	QString overlayF(qreal alphaBackground, const QString& colorBackground, qreal alphaForeground, const QString& colorForeground);
}