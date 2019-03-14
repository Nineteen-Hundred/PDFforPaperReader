#include <paperannotation.h>


void PaperAnnotation::FlatTextAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::red);
    painter->setFont(annotation->textFont());
    painter->drawText(QRectF(annotation->boundary().x()*width,
                             annotation->boundary().y()*height,
                             annotation->boundary().width()*width,
                             annotation->boundary().height()*height), annotation->contents());
    setPos(annotation->boundary().x(), annotation->boundary().y());
    setZValue(10);
}

PaperAnnotation::Annotation::Annotation() {setAcceptHoverEvents(true);}
