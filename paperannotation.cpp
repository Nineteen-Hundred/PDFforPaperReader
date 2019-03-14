#include <paperannotation.h>


void PaperAnnotation::FlatTextAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::red);
    painter->setFont(annotation->textFont());
    painter->drawText(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().width()*height*scale),
                      annotation->contents());
    setPos(annotation->boundary().x()*width*scale, annotation->boundary().y()*height*scale);
}

PaperAnnotation::Annotation::Annotation() {setAcceptHoverEvents(true);}
