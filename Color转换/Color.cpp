#include "Color.h"


#include "pch.h"
#include <QColor>

namespace ColorDef
{
	// 主色
	const char* clr_primary_n = "#5C39D9";  // 主色默认状态

	QString combineAlpha(int alpha, const QString & color)
	{
		QColor c(color);
		c.setAlpha(alpha);
		return c.name(QColor::HexArgb);
	}
	QString combineAlphaF(qreal alpha, const QString & color)
	{
		QColor c(color);
		c.setAlphaF(alpha);
		return c.name(QColor::HexArgb);
	}
	QString overlay(int alphaBackground, const QString & colorBackground, int alphaForeground, const QString & colorForeground)
	{
		// https://zhuanlan.zhihu.com/p/406769781
		QColor fore(combineAlpha(alphaForeground, colorForeground));
		QColor back(combineAlpha(alphaBackground, colorBackground));
		qreal a = fore.alphaF() + back.alphaF() * (1 - fore.alphaF());
		const int r = static_cast<int>(fore.red() * fore.alphaF() + back.red() * back.alphaF() * (1 - fore.alphaF()) / a);
		const int g = static_cast<int>(fore.green() * fore.alphaF() + back.green() * back.alphaF() * (1 - fore.alphaF()) / a);
		const int b = static_cast<int>(fore.blue() * fore.alphaF() + back.blue() * back.alphaF() * (1 - fore.alphaF()) / a);
		QColor ret;
		ret.setAlphaF(a);
		ret.setRed(r);
		ret.setGreen(g);
		ret.setBlue(b);
		return ret.name(QColor::HexArgb);
	}
	QString overlayF(qreal alphaBackground, const QString & colorBackground, qreal alphaForeground, const QString & colorForeground)
	{
		// https://zhuanlan.zhihu.com/p/406769781
		QColor fore(combineAlphaF(alphaForeground, colorForeground));
		QColor back(combineAlphaF(alphaBackground, colorBackground));
		qreal a = fore.alphaF() + back.alphaF() * (1 - fore.alphaF());
		const int r = static_cast<int>(fore.red() * fore.alphaF() + back.red() * back.alphaF() * (1 - fore.alphaF()) / a);
		const int g = static_cast<int>(fore.green() * fore.alphaF() + back.green() * back.alphaF() * (1 - fore.alphaF()) / a);
		const int b = static_cast<int>(fore.blue() * fore.alphaF() + back.blue() * back.alphaF() * (1 - fore.alphaF()) / a);
		QColor ret;
		ret.setAlphaF(a);
		ret.setRed(r);
		ret.setGreen(g);
		ret.setBlue(b);
		return ret.name(QColor::HexArgb);
	}
}

