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

        const qreal penWidth = 0;

        const QColor fgcolor = option->palette.windowText().color();
        const QColor bgcolor(
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

void PaperAnnotation::FlatTextAnnotation::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_bIsResizing || (isInResizeArea(event->pos()) && isSelected()))
        setCursor(Qt::SizeFDiagCursor);
    else
        setCursor(Qt::ArrowCursor);

    QGraphicsItem::hoverMoveEvent(event);
}

void PaperAnnotation::FlatTextAnnotation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_bIsResizing)
    {
        qreal w = event->pos().x();
        qreal h = event->pos().y();
        qDebug() << w;
        QRectF boundary = QRectF(annotation->boundary());
        boundary.setWidth(w/scale/width);
        boundary.setHeight(h/scale/height);
        annotation->setBoundary(boundary);
        prepareGeometryChange();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void PaperAnnotation::FlatTextAnnotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isInResizeArea(event->pos()))
        m_bIsResizing = true;
    else
        QGraphicsItem::mousePressEvent(event);
}

void PaperAnnotation::FlatTextAnnotation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_bIsResizing)
        m_bIsResizing = false;
    else
    {
        QRectF boundary = QRectF(pos().x()/scale/width, (pos().y()-index*scale*height)/scale/height, annotation->boundary().width(), annotation->boundary().height());
        annotation->setBoundary(boundary);
        QGraphicsItem::mouseReleaseEvent(event);
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void PaperAnnotation::FlatTextAnnotation::setNewStyle(const QString &text, const QFont &font, const QColor &color)
{
    annotation->setContents(text);
    annotation->setTextFont(font);
    annotation->setTextColor(color);

}

PaperAnnotation::Annotation::Annotation() {
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);
}

bool PaperAnnotation::Annotation::isInResizeArea(const QPointF &pos)
{
    const qreal g_cResizePos[] = {9, 6, 3};
    return (pos.x() - boundingRect().width() + g_cResizePos[0]) > (boundingRect().height() - pos.y());
}

void PaperAnnotation::GeomAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setWidth((int)(annotation->style().width()));
    pen.setColor(annotation->style().color());
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
    QPainterPathStroker stroker;
    stroker.setWidth(10);
    if(isSelected())
    {
        if(annotation->geomType()==0)
        {
            painter.addRect(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
        }
        else
        {
            painter.addRect(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
        }
        return painter;
    }
    else
    {
        if(annotation->geomType()==0)
        {
            painter.addRect(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
        }
        else
        {
            painter.addEllipse(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale));
        }
        return stroker.createStroke(painter);
    }

}

void PaperAnnotation::GeomAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    AnnotationDialog::GeomDialog *dialog = new AnnotationDialog::GeomDialog(annotation->style().color(), (int)(annotation->style().width()));
    connect(dialog, &AnnotationDialog::GeomDialog::configUpdated, this, &PaperAnnotation::GeomAnnotation::setNewStyle);
    dialog->exec();
}

void PaperAnnotation::GeomAnnotation::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_bIsResizing || (isInResizeArea(event->pos()) && isSelected()))
        setCursor(Qt::SizeFDiagCursor);
    else
        setCursor(Qt::ArrowCursor);

    QGraphicsItem::hoverMoveEvent(event);
}

void PaperAnnotation::GeomAnnotation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_bIsResizing)
    {
        qreal w = event->pos().x();
        qreal h = event->pos().y();
        QRectF boundary = QRectF(annotation->boundary());
        boundary.setWidth(w/scale/width);
        boundary.setHeight(h/scale/height);
        annotation->setBoundary(boundary);
        prepareGeometryChange();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void PaperAnnotation::GeomAnnotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isInResizeArea(event->pos()))
        m_bIsResizing = true;
    else
        QGraphicsItem::mousePressEvent(event);
}

void PaperAnnotation::GeomAnnotation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_bIsResizing)
        m_bIsResizing = false;
    else
    {
        QRectF boundary = QRectF(pos().x()/scale/width, (pos().y()-index*scale*height)/scale/height, annotation->boundary().width(), annotation->boundary().height());
        annotation->setBoundary(boundary);
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void PaperAnnotation::GeomAnnotation::setNewStyle(const QColor &color, int width)
{
    Poppler::Annotation::Style style = Poppler::Annotation::Style(annotation->style());
    style.setColor(color);
    style.setWidth(width);
    annotation->setStyle(style);
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

QPainterPath PaperAnnotation::LineAnnotation::shape() const
{
    QPainterPath painter;
    QPainterPathStroker stroker;
    stroker.setWidth(10);
    painter.moveTo(QPointF((startPoint.x()-annotation->boundary().left())*scale*width, (startPoint.y()-annotation->boundary().top())*scale*height));
    painter.lineTo(QPointF((endPoint.x()-annotation->boundary().left())*scale*width, (endPoint.y()-annotation->boundary().top())*scale*height));
    QLineF line = QLineF(QPointF((startPoint.x()-annotation->boundary().left())*scale*width, (startPoint.y()-annotation->boundary().top())*scale*height),
                    QPointF((endPoint.x()-annotation->boundary().left())*scale*width, (endPoint.y()-annotation->boundary().top())*scale*height));

    QLineF v = line.unitVector();
    double length = annotation->style().width()*5;
    v.setLength(length);
    double ratio = length / line.length();
    v.translate(QPointF(line.dx()*(1-ratio), line.dy()*(1-ratio)));

    QLineF n = v.normalVector();
    n.setLength(n.length() * 0.5);
    QLineF n2 = n.normalVector().normalVector();

    QPointF p1 = v.p2();
    QPointF p2 = n.p2();
    QPointF p3 = n2.p2();
    QPolygonF polygon;
    polygon.append(p1);
    polygon.append(p2);
    polygon.append(p3);
    painter.addPolygon(polygon);
    return stroker.createStroke(painter);
}


void PaperAnnotation::LineAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor(annotation->style().color());
    pen.setWidth(annotation->style().width());
    painter->setPen(pen);
    QLineF line = QLineF(QPointF((startPoint.x()-annotation->boundary().left())*scale*width, (startPoint.y()-annotation->boundary().top())*scale*height),
                    QPointF((endPoint.x()-annotation->boundary().left())*scale*width, (endPoint.y()-annotation->boundary().top())*scale*height));

    painter->drawLine(line);

    QLineF v = line.unitVector();
    double length = annotation->style().width()*5;
    v.setLength(length);
    double ratio = length / line.length();
    v.translate(QPointF(line.dx()*(1-ratio), line.dy()*(1-ratio)));

    QLineF n = v.normalVector();
    n.setLength(n.length() * 0.5);
    QLineF n2 = n.normalVector().normalVector();

    QPointF p1 = v.p2();
    QPointF p2 = n.p2();
    QPointF p3 = n2.p2();
    QPolygonF polygon;
    polygon.append(p1);
    polygon.append(p2);
    polygon.append(p3);
    painter->setBrush(annotation->style().color());
    painter->drawPolygon(polygon);
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
        painter->drawLine(QLineF(QPointF((startPoint.x()-annotation->boundary().left())*scale*width, (startPoint.y()-annotation->boundary().top())*scale*height),
                                 QPointF((endPoint.x()-annotation->boundary().left())*scale*width, (endPoint.y()-annotation->boundary().top())*scale*height)));

        painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(QLineF(QPointF((startPoint.x()-annotation->boundary().left())*scale*width, (startPoint.y()-annotation->boundary().top())*scale*height),
                                 QPointF((endPoint.x()-annotation->boundary().left())*scale*width, (endPoint.y()-annotation->boundary().top())*scale*height)));
    }
}

void PaperAnnotation::LineAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    AnnotationDialog::GeomDialog *dialog = new AnnotationDialog::GeomDialog(annotation->style().color(), (int)(annotation->style().width()));
    connect(dialog, &AnnotationDialog::GeomDialog::configUpdated, this, &PaperAnnotation::LineAnnotation::setNewStyle);
    dialog->exec();
}

void PaperAnnotation::LineAnnotation::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_bIsResizing || (isInResizeArea(event->pos()) && isSelected()))
        setCursor(Qt::SizeAllCursor);
    else
        setCursor(Qt::ArrowCursor);

    QGraphicsItem::hoverMoveEvent(event);
}

void PaperAnnotation::LineAnnotation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_bIsResizing)
    {
        endPoint = QPointF(event->pos().x()/scale/width+annotation->boundary().left(), event->pos().y()/scale/height+annotation->boundary().top());
        QLinkedList<QPointF> points;
        points.append(startPoint);
        points.append(endPoint);
        annotation->setLinePoints(points);
        double lefttopx = startPoint.x()<endPoint.x()?startPoint.x():endPoint.x();
        double lefttopy = startPoint.y()<endPoint.y()?startPoint.y():endPoint.y();
        double rightbottomx = startPoint.x()>=endPoint.x()?startPoint.x():endPoint.x();
        double rightbottomy = startPoint.y()>=endPoint.y()?startPoint.y():endPoint.y();
        annotation->setBoundary(QRectF(QPointF(lefttopx-0.01, lefttopy-0.01), QPointF(rightbottomx+0.01, rightbottomy+0.01)));
        setPos(annotation->boundary().x()*width*scale,
               annotation->boundary().y()*height*scale +index*scale*height);
        prepareGeometryChange();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void PaperAnnotation::LineAnnotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->button() == Qt::LeftButton && isInResizeArea(event->pos()))
    {
        m_bIsResizing = true;

    }else
    {
        QGraphicsItem::mousePressEvent(event);

    }
}

void PaperAnnotation::LineAnnotation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_bIsResizing)
    {
        m_bIsResizing = false;
    }
    else
    {
        qDebug() << "lineannotation";
        startPoint = startPoint + QPointF(pos().x()/scale/width-annotation->boundary().x(), (pos().y()-index*scale*height)/scale/height-annotation->boundary().y());
        endPoint = endPoint + QPointF(pos().x()/scale/width-annotation->boundary().x(), (pos().y()-index*scale*height)/scale/height-annotation->boundary().y());
        annotation->linePoints().clear();
        annotation->linePoints().append(startPoint);
        annotation->linePoints().append(endPoint);
        QRectF boundary = QRectF(pos().x()/scale/width, (pos().y()-index*scale*height)/scale/height, annotation->boundary().width(), annotation->boundary().height());
        annotation->setBoundary(boundary);
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

bool PaperAnnotation::LineAnnotation::isInResizeArea(const QPointF &pos)
{
    return abs(pos.x()-(endPoint.x()-annotation->boundary().left())*scale*width)<30 && abs(pos.y()-(endPoint.y()-annotation->boundary().top())*scale*width)<30;
}

void PaperAnnotation::LineAnnotation::setNewStyle(const QColor &color, int width)
{
    Poppler::Annotation::Style style = Poppler::Annotation::Style(annotation->style());
    style.setColor(color);
    style.setWidth(width);
    annotation->setStyle(style);
}
