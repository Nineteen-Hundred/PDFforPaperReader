#include <paperannotation.h>


void PaperAnnotation::FlatTextAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(annotation->textColor());
    painter->setFont(annotation->textFont());
    painter->drawText(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale),
                      annotation->contents());
    setZValue(10);

    if(option->state & QStyle::State_Selected)
    {
        const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
        if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
            return;

        const QRectF mbrect = painter->transform().mapRect(boundingRect());
        if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
            return;

        qreal itemPenWidth;
        itemPenWidth = 1;
        const qreal pad = itemPenWidth / 2;

        const qreal penWidth = 0; // cosmetic pen

        const QColor fgcolor = option->palette.windowText().color();
        const QColor bgcolor( // ensure good contrast against fgcolor
                              fgcolor.red()   > 127 ? 0 : 255,
                              fgcolor.green() > 127 ? 0 : 255,
                              fgcolor.blue()  > 127 ? 0 : 255);

        painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));

        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));
    }
}

void PaperAnnotation::FlatTextAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    AnnotationDialog::FlatTextDialog *dialog = new AnnotationDialog::FlatTextDialog(annotation->contents(), annotation->textFont(), annotation->textColor());
    connect(dialog, &AnnotationDialog::FlatTextDialog::configUpdated, this, &PaperAnnotation::FlatTextAnnotation::setNewStyle);
    dialog->exec();
}

void PaperAnnotation::FlatTextAnnotation::setNewStyle(const QString &text, const QFont &font, const QColor &color)
{
    annotation->setContents(text);
    annotation->setTextFont(font);
    annotation->setTextColor(color);

}

PaperAnnotation::Annotation::Annotation() {
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
}

void PaperAnnotation::GeomAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;
    pen.setWidth(2);
    pen.setColor(annotation->style().color());
    pen.setWidth(annotation->style().lineStyle()+2);
    painter->setPen(pen);

    if(annotation->geomType()==0)
    {
        painter->drawRect(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
    }
    else
    {
        painter->drawEllipse(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
    }
    setZValue(10);

    if(option->state & QStyle::State_Selected)
    {
        const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
        if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
            return;

        const QRectF mbrect = painter->transform().mapRect(boundingRect());
        if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
            return;

        qreal itemPenWidth;
        itemPenWidth = 1;
        const qreal pad = itemPenWidth / 2;

        const qreal penWidth = 0; // cosmetic pen

        const QColor fgcolor = option->palette.windowText().color();
        const QColor bgcolor( // ensure good contrast against fgcolor
                              fgcolor.red()   > 127 ? 0 : 255,
                              fgcolor.green() > 127 ? 0 : 255,
                              fgcolor.blue()  > 127 ? 0 : 255);

        painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));

        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));
    }
}

QPainterPath PaperAnnotation::GeomAnnotation::shape() const
{
    QPainterPath painter;
    if(annotation->geomType()==0)
    {
        painter.addRect(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
    }
    else
    {
        painter.addEllipse(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
    }
    return painter;
}

void PaperAnnotation::GeomAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    AnnotationDialog::GeomDialog *dialog = new AnnotationDialog::GeomDialog(annotation->style().color());
    dialog->exec();
}

PaperAnnotation::LineAnnotation::LineAnnotation(int index, Poppler::LineAnnotation *annotation, int width, int height)
{
    this->index = index;
    this->annotation = annotation; this->width = width; this->height = height;

    QLinkedListIterator<QPointF> rwIterator(annotation->linePoints());
    startPoint = rwIterator.next();
    rwIterator.toBack();
    endPoint = rwIterator.previous();
    setPos(annotation->boundary().x()*scale*width, annotation->boundary().y()*scale*height +index*height);
}


void PaperAnnotation::LineAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;
    //pen.setColor(Qt::red);
    pen.setColor(annotation->style().color());
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawLine(QLineF(QPointF((startPoint.x()-annotation->boundary().left())*scale*width, (startPoint.y()-annotation->boundary().top())*scale*height),
                             QPointF((endPoint.x()-annotation->boundary().left())*scale*width, (endPoint.y()-annotation->boundary().top())*scale*height)));

    setZValue(10);

    if(option->state & QStyle::State_Selected)
    {
        const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
        if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
            return;

        const QRectF mbrect = painter->transform().mapRect(boundingRect());
        if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
            return;

        qreal itemPenWidth;
        itemPenWidth = 1;
        const qreal pad = itemPenWidth / 2;

        const qreal penWidth = 0; // cosmetic pen

        const QColor fgcolor = option->palette.windowText().color();
        const QColor bgcolor( // ensure good contrast against fgcolor
                              fgcolor.red()   > 127 ? 0 : 255,
                              fgcolor.green() > 127 ? 0 : 255,
                              fgcolor.blue()  > 127 ? 0 : 255);

        painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));

        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(pad, pad, -pad, -pad));
    }
}
