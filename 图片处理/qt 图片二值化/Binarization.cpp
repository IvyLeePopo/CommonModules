#include "Binarization.h"
 
Binarization::Binarization()
    : m_threshold(0)
{
    m_Image = new QImage();
}

//图片二值化
int Binarization::GetBinaryzationImage(QString filePath)
{
    if(access(filePath.toLocal8Bit().data(), F_OK))
    {
        qDebug() << __LINE__ << __FUNCTION__ << "file not found:" << filePath;
        return -1;
    }
    QFileInfo info(filePath);
    if(info.size() <= 0)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "file size:" << info.size();
        return -1;
    }

    if(m_Image != NULL)
    {
        m_Image->load(filePath);
    }

    QImage* grayImage = grayScaleImg();
    if (grayImage == NULL)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "转灰度图失败";
        return -1;    //error
    }
    //grayImage->save("gray.jpg");

    int threshold = Otsu(grayImage);    //计算阈值
    if (threshold == -1)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "threshold value:" << threshold;
        return -1;    //error
    }
    m_threshold = threshold;
    QImage* binaryImg = process(grayImage);
    if (binaryImg == NULL)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "转二值化图失败";
        return -1;    //error
    }
    binaryImg->save("binary.jpg");

    return 0;
}

 
// 生成灰度图像
QImage* Binarization::grayScaleImg()
{
    if (m_Image == NULL)
    {
        return NULL;
    }
    int nWidth = m_Image->width();
    int nHeight = m_Image->height();
 
    //这里使用了QImage::Format_Grayscale8格式的图片，这种图片每个像素只用了8bit存储灰度颜色值
    QImage* grayImg = new QImage(nWidth, nHeight, QImage::Format_Grayscale8);
    QRgb rgbVal = 0;
    int grayVal = 0;
 
    for (int x = 0; x < nWidth; ++x)
    {
        for (int y = 0; y < nHeight; ++y)
        {
            rgbVal = m_Image->pixel(x, y);
 
            grayVal = qGray(rgbVal);    //这里调用Qt的函数，使用(R * 11 + G * 16 + B * 5)/32的方法计算
 
            grayImg->setPixel(x, y, QColor(grayVal, grayVal, grayVal).rgb());
        }
    }
 
    return grayImg;
}
 
int Binarization::Otsu(QImage* img)
{
    if (img == NULL)
    {
        return -1;
    }
    vector<int> histogram = Histogram(img);
    int total = 0;
    for (int i = 0; i != histogram.size(); ++i)
    {
        total += histogram[i];
    }
 
    double sum = 0.0;
    for (unsigned int i = 1; i < histogram.size(); ++i)
        sum += i * histogram[i];
    double sumB = 0.0;
    double wB = 0.0;
    double wF = 0.0;
    double mB = 0.0;
    double mF = 0.0;
    double max = 0.0;
    double between = 0.0;
    double threshold = 0.0;
    for (int i = 0; i != 256; ++i)
    {
        wB += histogram[i];
        if (wB == 0)
            continue;
        wF = total - wB;
        if (wF == 0)
            break;
        sumB += i * histogram[i];
        mB = sumB / wB;
        mF = (sum - sumB) / wF;
        between = wB * wF * (mB - mF) * (mB - mF);
        if ( between > max )
        {
            threshold = i;
            max = between;
        }
    }
    return threshold;
}
 
QImage* Binarization::process(QImage* img)
{
    if (img == NULL)
    {
        return NULL;
    }
    int width = img->width();
    int height = img->height();
    int bytePerLine = img->bytesPerLine();    // 每一行的字节数
    unsigned char *data = img->bits();
    unsigned char *binarydata = new unsigned char[bytePerLine * height];
 
    unsigned char g = 0;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            g = *(data + i * bytePerLine + j);
            if(int(g) >= m_threshold)
            {
                binarydata[ i * bytePerLine + j] = 0xFF;
            }
            else
            {
                binarydata[ i * bytePerLine + j] = 0x00;
            }
        }
    }
    QImage *ret = new QImage(binarydata, width, height, bytePerLine, QImage::Format_Grayscale8);
    return ret;
}
 
std::vector<int> Binarization::Histogram( QImage* img )
{
    unsigned char* graydata = img->bits();
    vector<int> hist(256);    // 256色
    for (int i = 0; i != img->width(); ++i)
    {
        for (int j = 0; j != img->height(); ++j)
        {
            int index = int(*graydata);
            hist[index] += 1;
            graydata += 1;    //step
        }
    }
    graydata = NULL;
    return hist;
}
