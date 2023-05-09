/*
 * @Description: 自定义字体
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once


#pragma once
#include <QFont>
namespace FontDef
{
	// http://idoc.corp.4dstc.com/app/mjoTnZplSP/comment/design/Msc2uUQqaG
	enum class FontSize
	{
		T1, // 常规文本12
		T2, // 14
	};
	enum class FontWeight
	{
		Regular,    // 全局页面字体
		Semibold    // 用于指定的标题字体
	};
	QFont getFont(FontSize size = FontSize::T1, FontWeight weight = FontWeight::Regular);
	QString getFontString(FontSize size = FontSize::T1, FontWeight weight = FontWeight::Regular);
}

