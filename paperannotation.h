#ifndef PAPERANNOTATION_H
#define PAPERANNOTATION_H

#endif // PAPERANNOTATION_H

#include <QGraphicsItem>
#include <poppler-annotation.h>
#include <poppler-qt5.h>
#include <QPainter>
#include <QRect>
#include <QDebug>

namespace PaperAnnotation {
class Annotation : public QGraphicsItem
{
public:
    Annotation();
    virtual QRectF boundingRect()const override = 0;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;
};

class HighlightAnnotation : public Annotation
{
public:
    HighlightAnnotation(Poppler::HighlightAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::HighlightAnnotation *annotation;
    int height, width = 0;
};

class PopupTextAnnotation : public Annotation
{
public:
    PopupTextAnnotation(Poppler::TextAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::TextAnnotation *annotation;
    int height, width = 0;
};

class FlatTextAnnotation : public Annotation
{
public:
    FlatTextAnnotation(Poppler::TextAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x()*width, annotation->boundary().y()*height, annotation->boundary().width()*width, annotation->boundary().height()*height);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Poppler::TextAnnotation *annotation;
    int height, width = 0;
};

class LinkedTextAnnotation : public Annotation
{
public:
    LinkedTextAnnotation(Poppler::TextAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::TextAnnotation *annotation;
    int height, width = 0;
};

class GeomAnnotation : public Annotation
{
public:
    GeomAnnotation(Poppler::GeomAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:
    Poppler::GeomAnnotation *annotation;
    int height, width = 0;
};

class LinkAnnotation : public Annotation
{
public:
    LinkAnnotation(Poppler::LinkAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::LinkAnnotation *annotation;
    int height, width = 0;
};

class InkAnnotation : public Annotation
{
public:
    InkAnnotation(Poppler::InkAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::InkAnnotation *annotation;
    int height, width = 0;
};

class LineAnnotation : public Annotation
{
public:
    LineAnnotation(Poppler::LineAnnotation *annotation, int width, int height) {this->annotation = annotation; this->width = width; this->height = height;}
    QRectF boundingRect()const override {return QRectF(annotation->boundary().x(), annotation->boundary().y(), annotation->boundary().width(), annotation->boundary().height());}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::LineAnnotation *annotation;
    int height, width = 0;
};
}
