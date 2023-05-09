
#include "pch.h"
#include "fontdef.h"
#include "common/dpiscaled.h"
namespace FontDef
{
	int getPixelSize(FontSize size)
	{
		switch (size)
		{
		case FontDef::FontSize::T1:
			return _S(12);
		case FontDef::FontSize::T2:
			return _S(14);
		default:
			break;
		}
		return _S(12);
	}
	int getFontWeight(FontWeight weight)
	{
		switch (weight)
		{
		case FontDef::FontWeight::Regular:
			return QFont::Normal;
		case FontDef::FontWeight::Semibold:
			return QFont::DemiBold;
		default:
			break;
		}
		return QFont::Normal;
	}
	QFont getFont(FontSize size, FontWeight weight)
	{
		QFont font;
		font.setFamily(QStringLiteral("Microsoft Yahei"));
		// 未找到精准设置行高的办法
		int pixelSize = getPixelSize(size);
		font.setPixelSize(pixelSize);
		int fontWeight = getFontWeight(weight);
		font.setWeight(fontWeight);

		return font;
	}
	QString getFontString(FontSize size, FontWeight weight)
	{
		QString fontString;
		fontString += "font-family:Microsoft Yahei;";
		fontString += QString("font-size:%1px;").arg(getPixelSize(size));
		fontString += QString("font-weight:%1px;").arg(getFontWeight(weight));
		return fontString;
	}
}
