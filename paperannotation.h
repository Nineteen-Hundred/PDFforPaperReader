#ifndef PAPERANNOTATION_H
#define PAPERANNOTATION_H

#endif // PAPERANNOTATION_H

#include <QGraphicsItem>
#include <poppler-annotation.h>
#include <poppler-qt5.h>
#include <QPainter>
#include <QRect>
#include <QDebug>
#include <annotationdialog.h>
#include <QContextMenuEvent>
#include <QMutableLinkedListIterator>
#include <QObject>
#include <QGraphicsSceneHoverEvent>
#include <qmath.h>
#include <QFontMetrics>

namespace PaperAnnotation {
class Annotation : public QGraphicsItem, public QObject
{
public:
    Annotation();
    virtual QRectF boundingRect()const override = 0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;
    qreal scale = 1;
    int index = 0;
    bool isInResizeArea(const QPointF& pos);
    bool m_bIsResizing = false;
    virtual Poppler::Annotation * return_annotation() = 0;
    int width, height = 0;
    void resetpos();
};

class HighlightAnnotation : public Annotation
{
public:
    HighlightAnnotation(Poppler::HighlightAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    Poppler::Annotation * return_annotation() override;

private:
    Poppler::HighlightAnnotation *annotation;
};

class PopupTextAnnotation : public Annotation
{
    Q_OBJECT
public:
    PopupTextAnnotation(int index, Poppler::TextAnnotation *annotation, int width, int height, double scalefactor) {
        this->scale = scalefactor;
        this->index = index;
        this->annotation = annotation; this->width = width; this->height = height;
        setPos(annotation->boundary().x()*width*scale,
               annotation->boundary().y()*height*scale +index*scale*height);
    }
    QRectF boundingRect()const override
    {return QRectF(0, 0, annotation->boundary().width()*width*scale,
                   annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    Poppler::Annotation * return_annotation() override;

    Poppler::TextAnnotation *annotation;

    void readyForDelete();

private:
    bool isMoving = false;
    void setNewStyle(const QString &text, const QFont &font, const QColor &color);

signals:
    void deleteSelf();
    void posChanged();
};

class FlatTextAnnotation : public Annotation
{
    Q_OBJECT
public:
    FlatTextAnnotation(int index, Poppler::TextAnnotation *annotation, int width, int height, double scalefactor) {
        this->index = index;
        this->scale = scalefactor;
        this->annotation = annotation; this->width = width; this->height = height;
        setPos(annotation->boundary().x()*width*scale,
               annotation->boundary().y()*height*scale +index*scale*height);
    }
    QRectF boundingRect()const override
    {return QRectF(0, 0, annotation->boundary().width()*width*scale,
                   annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    Poppler::TextAnnotation *annotation;
    void readyForDelete();
    Poppler::Annotation * return_annotation() override;

private:
    bool isMoving = false;
    void setNewStyle(const QString &text, const QFont &font, const QColor &color);

signals:
    void deleteSelf();
    void posChanged();
};

class LinkedTextAnnotation : public Annotation
{
public:
    LinkedTextAnnotation(Poppler::TextAnnotation *annotation, int width, int height) {
        setPos(annotation->boundary().x()*width*scale, annotation->boundary().y()*height*scale);
        this->annotation = annotation; this->width = width; this->height = height;
    }
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    Poppler::Annotation * return_annotation() override;

private:
    Poppler::TextAnnotation *annotation;
};

class GeomAnnotation : public Annotation
{
    Q_OBJECT
public:
    GeomAnnotation(int index, Poppler::GeomAnnotation *annotation, int width, int height, double scalefactor) {
        this->index = index;
        this->scale = scalefactor;
        this->annotation = annotation; this->width = width; this->height = height;
        setPos(annotation->boundary().x()*width*scale,
               annotation->boundary().y()*height*scale +index*scale*height);
    }
    QRectF boundingRect()const override
    {return QRectF(0, 0, annotation->boundary().width()*width*scale,
                   annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void setNewStyle(const QColor &color, int width);
    void readyForDelete();
    Poppler::Annotation * return_annotation() override;

    Poppler::GeomAnnotation *annotation;

signals:
    void deleteSelf();
    void posChanged();
};

class LinkAnnotation : public Annotation
{
public:
    LinkAnnotation(Poppler::LinkAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    Poppler::Annotation * return_annotation() override;

private:
    Poppler::LinkAnnotation *annotation;
};

class InkAnnotation : public Annotation
{
public:
    InkAnnotation(int index, Poppler::InkAnnotation *annotation, int width, int height, double scalefactor) {
        this->index = index;
        this->scale = scalefactor;
        this->annotation = annotation; this->width = width; this->height = height;
        setPos(annotation->boundary().x()*width*scale,
               annotation->boundary().y()*height*scale +index*scale*height);
    }

    QRectF boundingRect() const override
    {return QRectF(0, 0, annotation->boundary().width()*width*scale,
                   annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    Poppler::InkAnnotation *annotation;
    void setNewStyle(const QColor &color, int width);
    Poppler::Annotation * return_annotation() override;

private:
    QPointF startPoint;
    QPointF endPoint;
};

class LineAnnotation : public Annotation
{
    Q_OBJECT
public:
    LineAnnotation(int index, Poppler::LineAnnotation *annotation, int width, int height, double scalefactor);
    QPainterPath shape() const override;
    QRectF boundingRect()const override
    {return QRectF(0, 0, annotation->boundary().width()*width*scale,
                   annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    bool isInResizeArea(const QPointF& pos);

    Poppler::LineAnnotation *annotation;
    void setNewStyle(const QColor &color, int width);
    void readyForDelete();
    Poppler::Annotation * return_annotation() override;

private:
    QPointF startPoint;
    QPointF endPoint;

signals:
    void deleteSelf();
    void posChanged();
};

class PreviewAnnotation : public Annotation
{
    Q_OBJECT
public:
    PreviewAnnotation(int index, Poppler::TextAnnotation *annotation, int width, int height, double scalefactor);
    //QPainterPath shape() const override;
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Poppler::TextAnnotation *annotation;
    bool isLeft = true;
    bool isZoom = false;
    int textwidthnum = 10;
    int textheight = 0;
    bool isSelected = false;
    QString practicalText;
    QPointF startPoint, endPoint, rect_width, rect_height;
    int textpointsize = 16;
    int textperline = 12;
    int textlinespacing = 10;
    int linenum = 4;
    void resetpos1();
    int textlength;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    Poppler::Annotation * return_annotation() override;
};
}
