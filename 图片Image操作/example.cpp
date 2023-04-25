#include "imageutils.h"

#include <QPixmap>
/**
	实例演示：
*/
//图片放在控件上，进行等比缩放
//1、使用
QImage cutImg = imageutils::cutImage(imagePath);
QPixmap pix;
pix.convertFromImage(cutImg);
pix = imageutils::scaleKeepRatio(pImageContent->width(), pImageContent->height(), _S(2), pix, Qt::SmoothTransformation);
pImageContent->setPixmap(pix);