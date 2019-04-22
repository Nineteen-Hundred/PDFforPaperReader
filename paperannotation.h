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
};

class HighlightAnnotation : public Annotation
{
public:
    HighlightAnnotation(Poppler::HighlightAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::HighlightAnnotation *annotation;
    int height, width = 0;
};

class PopupTextAnnotation : public Annotation
{
public:
    PopupTextAnnotation(Poppler::TextAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::TextAnnotation *annotation;
    int height, width = 0;
};

class FlatTextAnnotation : public Annotation
{
public:
    FlatTextAnnotation(int index, Poppler::TextAnnotation *annotation, int width, int height) {
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

    Poppler::TextAnnotation *annotation;
    int height, width = 0;

private:
    bool isMoving = false;
    void setNewStyle(const QString &text, const QFont &font, const QColor &color);
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

private:
    Poppler::TextAnnotation *annotation;
    int height, width = 0;
};

class GeomAnnotation : public Annotation
{
public:
    GeomAnnotation(int index, Poppler::GeomAnnotation *annotation, int width, int height) {
        this->index = index;
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

    Poppler::GeomAnnotation *annotation;
    int height, width = 0;
};

class LinkAnnotation : public Annotation
{
public:
    LinkAnnotation(Poppler::LinkAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::LinkAnnotation *annotation;
    int height, width = 0;
};

class InkAnnotation : public Annotation
{
public:
    InkAnnotation(Poppler::InkAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::InkAnnotation *annotation;
    int height, width = 0;
};

class LineAnnotation : public Annotation
{
public:
    LineAnnotation(int index, Poppler::LineAnnotation *annotation, int width, int height);
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
    int height, width = 0;
    void setNewStyle(const QColor &color, int width);

private:
    QPointF startPoint;
    QPointF endPoint;
};
}
