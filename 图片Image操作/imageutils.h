/*
 * @Description: Image相关操作
 * @Author: Ivy
 * @Date: 2022-04-25 09:34:43
 * @LastEditTime: 2023-02-25 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
class QString;
namespace imageutils
{
	// 拼接图片：剪裁图片，把图片透明的部分去掉
	QImage cutImage(const QImage &imgPath);//进行等比缩放
	QImage cutImage(const QString &imgPath);
	QImage cutImage(const QPixmap &pixmap);

	// 根据传入的QPixmap和目标窗口大小计算合适的QPixmap
	QPixmap scaleKeepRatio(int width, int height, int margin, const QPixmap &pixmap);
	QString getSvgResPath();

	// 获取svg资源路径
	QString getSvgResPathNomal(const QString &svgNameNoExt);
	QString getSvgResPathHover(const QString &svgNameNoExt);
	QString getSvgResPathCliecked(const QString &svgNameNoExt);
	QString getSvgResPathDisable(const QString &svgNameNoExt);

	// 转换svg资源
	QPixmap getSvgResPixmap(const QString &svgNameNoExt, QSize size);
	QPixmap getSvgResPixmapNomal(const QString &svgNameNoExt, QSize size);
	QPixmap getSvgResPixmapHover(const QString &svgNameNoExt, QSize size);
	QPixmap getSvgResPixmapCliecked(const QString &svgNameNoExt, QSize size);
	QPixmap getSvgResPixmapDisable(const QString &svgNameNoExt, QSize size);
};

