#ifndef PAPERANNOTATION_H
#define PAPERANNOTATION_H

#endif // PAPERANNOTATION_H

#include <QGraphicsItem>
#include <poppler-annotation.h>
#include <poppler-qt5.h>

namespace PaperAnnotation {
class Annotation : public QGraphicsItem
{
public:
    Annotation();
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class HighlightAnnotation : public Annotation
{
public:
    HighlightAnnotation(Poppler::HighlightAnnotation *annotation) {this->annotation = annotation;}
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::HighlightAnnotation *annotation;
};

class TextAnnotation : public Annotation
{
public:
    TextAnnotation(Poppler::TextAnnotation *annotation) {this->annotation = annotation;}
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::TextAnnotation *annotation;
};

class GeomAnnotation : public Annotation
{
public:
    GeomAnnotation(Poppler::GeomAnnotation *annotation) {this->annotation = annotation;}
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;


private:
    Poppler::GeomAnnotation *annotation;
};

class LinkAnnotation : public Annotation
{
public:
    LinkAnnotation(Poppler::LinkAnnotation *annotation) {this->annotation = annotation;}
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::LinkAnnotation *annotation;
};

class InkAnnotation : public Annotation
{
public:
    InkAnnotation(Poppler::InkAnnotation *annotation) {this->annotation = annotation;}
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::InkAnnotation *annotation;
};

class LineAnnotation : public Annotation
{
public:
    LineAnnotation(Poppler::LineAnnotation *annotation) {this->annotation = annotation;}
    QRectF boundingRect()const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Poppler::LineAnnotation *annotation;
};
}
