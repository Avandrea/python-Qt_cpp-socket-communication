// imageitem.cpp
#include "imageitem.h"
#include <iostream>

ImageItem::ImageItem(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    this->current_image = QImage(":/images/no_image.png");
    this->setFillColor(QColor(150,150,150,127));
}

void ImageItem::paint(QPainter *painter)
{
    QRectF bounding_rect = boundingRect();
//    QImage scaled = this->current_image.scaled(this->width(), this->height(), Qt::KeepAspectRatio);; //.scaledToHeight(320/*bounding_rect.height()*/);
    QImage scaled = this->current_image.scaled(this->width(), this->height());
    QPointF center = bounding_rect.center() - scaled.rect().center();

//    if (scaled.size().width() != 0){
//        std::cout << "Printing all pixels --- " << scaled.bitPlaneCount() << std::endl;
//        const uchar* bits = scaled.bits();
//        for (int i = 0; i < 5000; ++i){
//            std::cout << +bits[i] << " "; // std::endl;
//            if ((i + 1) % 26 == 0)
//                std::cout << std::endl;
//        }
//    }

    if(center.x() < 0)
        center.setX(0);
    if(center.y() < 0)
        center.setY(0);
   painter->drawImage(center, scaled, bounding_rect, Qt::AutoColor);
}

QImage ImageItem::image() const
{    return this->current_image;
}

void ImageItem::setImage(const QImage &image)
{
//    std::cout << "Printing all pixels --- " << image.bitPlaneCount() << std::endl;
//    const uchar* bits = image.bits();
//    for (int i = 0; i < 338; ++i){
//        std::cout << +bits[i] << " "; // std::endl;
//    }

    this->current_image = image;
    update();
}
