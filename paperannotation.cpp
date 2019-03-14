#include <paperannotation.h>


void PaperAnnotation::FlatTextAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::red);
    painter->setFont(annotation->textFont());
    painter->drawText(QRectF(annotation->boundary().x()*width*scale,
                             annotation->boundary().y()*height*scale,
                             annotation->boundary().width()*width*scale,
                             annotation->boundary().height()*height*scale), annotation->contents());
    setPos(annotation->boundary().x()*scale, annotation->boundary().y()*scale);

    setZValue(10);
}

PaperAnnotation::Annotation::Annotation() {setAcceptHoverEvents(true);}
