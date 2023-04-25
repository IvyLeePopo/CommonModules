#include "pch.h"
#include "imageutils.h"
#include <QSvgRenderer>
namespace imageutils
{
	QImage cutImage(const QImage &image)
	{
		int imgW = image.width();
		int imgH = image.height();
		int xStart = 0;
		int xEnd = imgW;
		int yStart = 0;
		int yEnd = imgH;
		// top
		for (int i = yStart; i < yEnd; ++i)
		{
			for (int j = xStart; j < xEnd; j++)
			{
				if (image.pixel(j, i) & 0xff000000)
				{
					yStart = i;
					i = yEnd;
					break;
				}
			}
		}
		// bottom
		for (int i = yEnd - 1; i >= yStart; --i)
		{
			for (int j = xStart; j < xEnd; j++)
			{
				if (image.pixel(j, i) & 0xff000000)
				{
					yEnd = i + 1;
					i = yStart - 1;
					break;
				}
			}
		}

		// left
		for (int i = xStart; i < xEnd; ++i)
		{
			for (int j = yStart; j < yEnd; j++)
			{
				if (image.pixel(i, j) & 0xff000000)
				{
					xStart = i;
					i = xEnd;
					break;
				}
			}
		}
		// right
		for (int i = xEnd - 1; i >= xStart; --i)
		{
			for (int j = yStart; j < yEnd; j++)
			{
				if (image.pixel(i, j) & 0xff000000)
				{
					xEnd = i + 1;
					i = xStart - 1;
					break;
				}
			}
		}
		QImage temp = image.copy(xStart, yStart, xEnd - xStart, yEnd - yStart);
		return temp;
	}

	QImage cutImage(const QString &imgPath)
	{
		QImageReader reader;
		reader.setFileName(imgPath);
		QImage image = reader.read();
		return cutImage(image);
	}

	QImage cutImage(const QPixmap &pixmap)
	{
		QImage image = pixmap.toImage();
		return cutImage(image);
	}

	QPixmap scaleKeepRatio(int width, int height, int margin, const QPixmap & pixmap)
	{
		if (width > pixmap.width() && height > pixmap.height())
		{
			return pixmap;
		}
		else if (pixmap.width() > 0 && width > 0)
		{
			if (1.0 * pixmap.height() / pixmap.width() < 1.0 * height / width)
			{
				return pixmap.scaledToWidth(width - margin * 2);
			}
			else
			{
				return pixmap.scaledToHeight(height - margin * 2);
			}
		}
		return pixmap;
	}

	QString getSvgResPath()
	{
		QString imgSvgPath = ":/svg";
		return imgSvgPath;
	}

	QString getSvgResPathNomal(const QString &svgNameNoExt)
	{
		QString imgSvgPath = ":/svg/" + svgNameNoExt + ".svg";
		return imgSvgPath;
	}

	QString getSvgResPathHover(const QString &svgNameNoExt)
	{
		QString imgSvgPath = ":/svg/" + svgNameNoExt + "-hover.svg";
		return imgSvgPath;
	}

	QString getSvgResPathCliecked(const QString &svgNameNoExt)
	{
		QString imgSvgPath = ":/svg/" + svgNameNoExt + "-clicked.svg";
		return imgSvgPath;
	}

	QString getSvgResPathDisable(const QString &svgNameNoExt)
	{
		QString imgSvgPath = ":/svg/" + svgNameNoExt + "-disable.svg";
		return imgSvgPath;
	}

	QPixmap getSvgResPixmap(const QString &svgNameNoExt, QSize size)
	{
		if (svgNameNoExt.isEmpty())
			return QPixmap();
		QString imgSvgPath = getSvgResPathNomal(svgNameNoExt);
		QPixmap pixmap(size.width(), size.height());
		pixmap.fill(Qt::transparent);
		QPainter painter(&pixmap);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
		QSvgRenderer svgRender;
		svgRender.load(imgSvgPath);
		svgRender.render(&painter);
		return pixmap;
	}

	QPixmap getSvgResPixmapNomal(const QString &svgNameNoExt, QSize size)
	{
		return getSvgResPixmap(svgNameNoExt, size);
	}

	QPixmap getSvgResPixmapHover(const QString &svgNameNoExt, QSize size)
	{
		QString svgNameNoExtHover = svgNameNoExt + "-hover";
		return getSvgResPixmap(svgNameNoExtHover, size);
	}

	QPixmap getSvgResPixmapCliecked(const QString &svgNameNoExt, QSize size)
	{
		QString svgNameNoExtCliecked = svgNameNoExt + "-clicked";
		return getSvgResPixmap(svgNameNoExtCliecked, size);
	}

	QPixmap getSvgResPixmapDisable(const QString &svgNameNoExt, QSize size)
	{
		QString svgNameNoExtHover = svgNameNoExt + "-disable";
		return getSvgResPixmap(svgNameNoExtHover, size);
	}
};
