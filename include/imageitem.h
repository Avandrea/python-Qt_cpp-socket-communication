/**
 *  BMCR_SpectralClassification
 *  @file imageitem.h
 *  @brief this class manages the representation of the incoming data as images.
 *  @author Andrea Avigni
 *  @version 1.0 11/06/2019
*/

#ifndef IMAGEITEM_H
#define IMAGEITEM_H
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>

class ImageItem : public QQuickPaintedItem
{
Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)
public:
    ImageItem(QQuickItem *parent = nullptr);
    Q_INVOKABLE void setImage(const QImage &image);
    void paint(QPainter *painter);
    QImage image() const;
signals:
    void imageChanged();
private:
    QImage current_image;
};

#endif // IMAGEITEM_H
