#include "Binarization.h"

Binarization binarization;
if (!binarization.GetBinaryzationImage("small.jpg"))
{
	qDebug() << __LINE__ << __FUNCTION__ << "转二值化图成功";
}