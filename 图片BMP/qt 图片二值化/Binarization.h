#ifndef BINARIZATION_H
#define BINARIZATION_H

#include <QDebug>
#include <QImage>
#include <QColor>
#include <QFileInfo>
#include <unistd.h>
 
using namespace std;
 
class Binarization
{
public:
    Binarization();
    int GetBinaryzationImage(QString filePath);
    QImage* grayScaleImg();
    int Otsu(QImage* img);
    QImage* process(QImage* img);
    int m_threshold;
 
private:
    QImage* m_Image;
    vector<int> Histogram(QImage* img);
};
 
#endif // BINARIZATION_H
