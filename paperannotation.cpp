#define PAGE_SPACING 5

#include <paperannotation.h>

void PaperAnnotation::FlatTextAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(annotation->textColor());
    QFont font = annotation->textFont();
    font.setPointSize(font.pointSize()*scale);
    painter->setFont(font);
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
    connect(dialog, &AnnotationDialog::FlatTextDialog::itemDeleted, this, &PaperAnnotation::FlatTextAnnotation::readyForDelete);
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
    }
    emit this->posChanged();
}

void PaperAnnotation::FlatTextAnnotation::readyForDelete()
{
    emit this->deleteSelf();
}

Poppler::Annotation *PaperAnnotation::FlatTextAnnotation::return_annotation()
{
    return this->annotation;
}

void PaperAnnotation::FlatTextAnnotation::setNewStyle(const QString &text, const QFont &font, const QColor &color)
{
    annotation->setContents(text);
    annotation->setTextFont(font);
    annotation->setTextColor(color);
    emit this->posChanged();
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

void PaperAnnotation::Annotation::resetpos()
{
    setPos(return_annotation()->boundary().x()*width*scale,
           return_annotation()->boundary().y()*height*scale +index*scale*height);
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
    connect(dialog, &AnnotationDialog::GeomDialog::itemDeleted, this, &PaperAnnotation::GeomAnnotation::readyForDelete);
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

    emit this->posChanged();
}

void PaperAnnotation::GeomAnnotation::setNewStyle(const QColor &color, int width)
{
    Poppler::Annotation::Style style = Poppler::Annotation::Style(annotation->style());
    style.setColor(color);
    style.setWidth(width);
    annotation->setStyle(style);
    emit this->posChanged();
}

void PaperAnnotation::GeomAnnotation::readyForDelete()
{
    emit this->deleteSelf();
}

Poppler::Annotation *PaperAnnotation::GeomAnnotation::return_annotation()
{
    return this->annotation;
}

PaperAnnotation::LineAnnotation::LineAnnotation(int index, Poppler::LineAnnotation *annotation, int width, int height, double scalefactor)
{
    this->scale = scalefactor;
    this->index = index;
    this->annotation = annotation; this->width = width; this->height = height;

    QLinkedListIterator<QPointF> rwIterator(annotation->linePoints());
    startPoint = rwIterator.next();
    rwIterator.toBack();
    endPoint = rwIterator.previous();
    setPos(annotation->boundary().x()*scale*width, annotation->boundary().y()*scale*height +index*height*scale);
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
    qDebug() << boundingRect().width() << boundingRect().height() << boundingRect().x() << boundingRect().y();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor(annotation->style().color());
    pen.setWidth(annotation->style().width());
    painter->setPen(pen);
    QLineF line = QLineF(QPointF((startPoint.x()-annotation->boundary().left())*scale*width,
                                 (startPoint.y()-annotation->boundary().top())*scale*height),
                         QPointF((endPoint.x()-annotation->boundary().left())*scale*width,
                                 (endPoint.y()-annotation->boundary().top())*scale*height));

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
    connect(dialog, &AnnotationDialog::GeomDialog::itemDeleted, this, &PaperAnnotation::LineAnnotation::readyForDelete);
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
        startPoint = startPoint + QPointF(pos().x()/scale/width-annotation->boundary().x(), (pos().y()-index*scale*height)/scale/height-annotation->boundary().y());
        endPoint = endPoint + QPointF(pos().x()/scale/width-annotation->boundary().x(), (pos().y()-index*scale*height)/scale/height-annotation->boundary().y());
        annotation->linePoints().clear();
        annotation->linePoints().append(startPoint);
        annotation->linePoints().append(endPoint);
        QRectF boundary = QRectF(pos().x()/scale/width, (pos().y()-index*scale*height)/scale/height, annotation->boundary().width(), annotation->boundary().height());
        annotation->setBoundary(boundary);
        QGraphicsItem::mouseReleaseEvent(event);
    }
    emit this->posChanged();
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
    emit this->posChanged();
}

void PaperAnnotation::LineAnnotation::readyForDelete()
{
    emit this->deleteSelf();
}

Poppler::Annotation *PaperAnnotation::LineAnnotation::return_annotation()
{
    return this->annotation;
}

void PaperAnnotation::PopupTextAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(annotation->textColor());
    painter->setFont(annotation->textFont());
    painter->drawImage(QRectF(0, 0, annotation->boundary().width()*width*scale, annotation->boundary().height()*height*scale),
                       QImage(":/image/popup"));
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

void PaperAnnotation::PopupTextAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    AnnotationDialog::FlatTextDialog *dialog = new AnnotationDialog::FlatTextDialog(annotation->contents(), annotation->textFont(), annotation->textColor());
    connect(dialog, &AnnotationDialog::FlatTextDialog::configUpdated, this, &PaperAnnotation::PopupTextAnnotation::setNewStyle);
    connect(dialog, &AnnotationDialog::FlatTextDialog::itemDeleted, this, &PaperAnnotation::PopupTextAnnotation::readyForDelete);
    dialog->exec();
}

void PaperAnnotation::PopupTextAnnotation::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_bIsResizing || (isInResizeArea(event->pos()) && isSelected()))
        setCursor(Qt::SizeFDiagCursor);
    else
        setCursor(Qt::ArrowCursor);

    QGraphicsItem::hoverMoveEvent(event);
}

void PaperAnnotation::PopupTextAnnotation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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

void PaperAnnotation::PopupTextAnnotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isInResizeArea(event->pos()))
        m_bIsResizing = true;
    else
        QGraphicsItem::mousePressEvent(event);
}

void PaperAnnotation::PopupTextAnnotation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_bIsResizing)
        m_bIsResizing = false;
    else
    {
        QRectF boundary = QRectF(pos().x()/scale/width, (pos().y()-index*scale*height)/scale/height, annotation->boundary().width(), annotation->boundary().height());
        annotation->setBoundary(boundary);
        QGraphicsItem::mouseReleaseEvent(event);
    }
    emit this->posChanged();
}

Poppler::Annotation *PaperAnnotation::PopupTextAnnotation::return_annotation()
{
    return this->annotation;
}

void PaperAnnotation::PopupTextAnnotation::readyForDelete()
{
    emit this->deleteSelf();
}

void PaperAnnotation::PopupTextAnnotation::setNewStyle(const QString &text, const QFont &font, const QColor &color)
{
    annotation->setContents(text);
    annotation->setTextColor(color);
    emit this->posChanged();
}

void PaperAnnotation::InkAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;
    pen.setColor(annotation->style().color());
    pen.setWidth(annotation->style().width());
    painter->setPen(pen);
    for(int i=0; i<annotation->inkPaths().count(); i++)
    {
        QPainterPath path;
        QPainterPathStroker stroker;
        stroker.setWidth(4);
        QLinkedListIterator<QPointF> rwIterator(annotation->inkPaths().at(i));
        path.moveTo(rwIterator.next());
        while(rwIterator.hasNext())
        {
            QPointF point = rwIterator.next();
            path.lineTo(QPointF((point.x()-annotation->boundary().left())*scale*width, (point.y()-annotation->boundary().top())*scale*height));
        }
        painter->drawPath(path);
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

void PaperAnnotation::InkAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    AnnotationDialog::GeomDialog *dialog = new AnnotationDialog::GeomDialog(annotation->style().color(), (int)(annotation->style().width()));
    connect(dialog, &AnnotationDialog::GeomDialog::configUpdated, this, &PaperAnnotation::InkAnnotation::setNewStyle);
    dialog->exec();
}

void PaperAnnotation::InkAnnotation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    QList<QLinkedList<QPointF>> listpoints;
    for(int i=0; i<annotation->inkPaths().count(); i++)
    {
        QPainterPath path;
        QPainterPathStroker stroker;
        stroker.setWidth(4);
        QLinkedList<QPointF> points;
        QLinkedListIterator<QPointF> rwIterator(annotation->inkPaths().at(i));
        while(rwIterator.hasNext())
        {
            points.append(rwIterator.next() + QPointF(pos().x()/scale/width-annotation->boundary().x(), (pos().y()-index*scale*height)/scale/height-annotation->boundary().y()));
        }
        listpoints.append(points);
    }
    annotation->setInkPaths(listpoints);

    QRectF boundary = QRectF(pos().x()/scale/width, (pos().y()-index*scale*height)/scale/height, annotation->boundary().width(), annotation->boundary().height());
    annotation->setBoundary(boundary);
}

void PaperAnnotation::InkAnnotation::setNewStyle(const QColor &color, int width)
{
    Poppler::Annotation::Style style = Poppler::Annotation::Style(annotation->style());
    style.setColor(color);
    style.setWidth(width);
    annotation->setStyle(style);
}

Poppler::Annotation *PaperAnnotation::InkAnnotation::return_annotation()
{
    return this->annotation;
}

PaperAnnotation::PreviewAnnotation::PreviewAnnotation(int index, Poppler::TextAnnotation *annotation, int width, int height, double scalefactor)
{
    this->textlength = annotation->contents().length();

    for(int i=0; i<annotation->contents().length(); i++)
    {
        if(!(annotation->contents().at(i).toLatin1()>0))
        {
            textlength++;
        }
    }

    qDebug() << textlength << textperline << textpointsize;
    practical_linenum = textlength / 2 / textperline + 1;

    this->width = width;
    this->height = height;
    this->scale = scalefactor;
    this->index = index;
    this->annotation = annotation;

    this->resetpos1();

    shadow_effect = new QGraphicsDropShadowEffect;
    no_shadow_effect = new QGraphicsDropShadowEffect;
    no_shadow_effect->setOffset(4, 4);
    shadow_effect->setColor(Qt::white);
    no_shadow_effect->setBlurRadius(10);
    this->setGraphicsEffect(no_shadow_effect);
//    this->setGraphicsEffect(shadow_effect);
}

QRectF PaperAnnotation::PreviewAnnotation::boundingRect() const
{
    if(isSelected)
    {
        if(isLeft)
        {
            return QRectF(0, 0,
                          (textpointsize*textperline+annotation->boundary().x()*width)+8,
                          ((textpointsize+textlinespacing)*practical_linenum));
        }
        else
        {
            return QRectF(0, 0,
                          (textpointsize*textperline+(1-annotation->boundary().x())*width)+8,
                          ((textpointsize+textlinespacing)*practical_linenum));
        }
    }
    else
    {
        if(isLeft)
        {
            return QRectF(0, 0,
                          (textpointsize*textperline+annotation->boundary().x()*width)+8,
                          ((textpointsize+textlinespacing)*(practical_linenum>linenum?linenum:practical_linenum)));
        }
        else
        {
            return QRectF(0, 0,
                          (textpointsize*textperline+(1-annotation->boundary().x())*width)+8,
                          ((textpointsize+textlinespacing)*(practical_linenum>linenum?linenum:practical_linenum)));
        }
    }
}

void PaperAnnotation::PreviewAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QFont font;
    font.setPixelSize(textpointsize);
    painter->setFont(font);

    if(isSelected)
    {
        if(isLeft)
        {
            QRectF rectbox = QRectF(QPointF(0, 0),
                                    startPoint-QPointF(annotation->boundary().x()*width*scale,0)-pos());
            QRectF textbox = QRectF(QPointF(0, 0)+QPointF(4, 4),
                                    startPoint-QPointF(annotation->boundary().x()*width*scale,0)-pos()-QPointF(4,4));
            painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawEllipse(startPoint-pos(), 3, 3);
            painter->drawLine(startPoint-pos(), endPoint-pos());
            painter->setBrush(QBrush(QColor(0xb0, 0x88, 0xff),Qt::SolidPattern));
            painter->setPen(QPen(QColor(0xb0, 0x88, 0xff), 1, Qt::SolidLine));
            painter->drawRect(rectbox);
            painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
            painter->drawText(textbox, annotation->contents());
            painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
            painter->drawEllipse(startPoint-pos(), 3, 3);
        }
        else
        {
            QRectF rectbox = QRectF(endPoint-pos(), boundingRect().bottomRight());
            QRectF textbox = QRectF(endPoint-pos()+QPointF(4,4), boundingRect().bottomRight()-QPointF(4,4));
            painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawEllipse(startPoint-pos(), 3, 3);
            painter->drawLine(startPoint-pos(), endPoint-pos());
            painter->setBrush(QBrush(QColor(0xb0, 0x88, 0xff),Qt::SolidPattern));
            painter->setPen(QPen(QColor(0xb0, 0x88, 0xff), 1, Qt::SolidLine));
            painter->drawRect(rectbox);
            painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
            painter->drawText(textbox, annotation->contents());
            painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
            painter->drawEllipse(startPoint-pos(), 3, 3);
        }
        this->setZValue(100);
    }
    else
    {
        if(isLeft)
        {
            QRectF rectbox = QRectF(QPointF(0, 0),
                                    startPoint-QPointF(annotation->boundary().x()*width*scale,0)-pos());
            QRectF textbox = QRectF(QPointF(0, 0)+QPointF(4, 4),
                                    startPoint-QPointF(annotation->boundary().x()*width*scale,0)-pos()-QPointF(4,4));
            painter->setPen(QPen(Qt::gray, 2, Qt::SolidLine));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawEllipse(startPoint-pos(), 3, 3);
            painter->drawLine(startPoint-pos(), endPoint-pos());
            painter->setBrush(QBrush(Qt::white,Qt::SolidPattern));
            painter->setBrush(QBrush(QColor(0xcc, 0xdd, 0xff),Qt::SolidPattern));
            painter->setPen(QPen(QColor(0xcc, 0xdd, 0xff), 2, Qt::SolidLine));
            painter->drawRect(rectbox);
            painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
            painter->drawText(textbox, practicalText);
            painter->setBrush(QBrush(Qt::gray, Qt::SolidPattern));
            painter->drawEllipse(startPoint-pos(), 3, 3);
        }
        else
        {
            QRectF rectbox = QRectF(endPoint-pos(), boundingRect().bottomRight());
            QRectF textbox = QRectF(endPoint-pos()+QPointF(4,4), boundingRect().bottomRight()-QPointF(4,4));
            painter->setPen(QPen(Qt::gray, 2, Qt::SolidLine));
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawEllipse(startPoint-pos(), 3, 3);
            painter->drawLine(startPoint-pos(), endPoint-pos());
            painter->setBrush(QBrush(QColor(0xcc, 0xdd, 0xff),Qt::SolidPattern));
            painter->setPen(QPen(QColor(0xcc, 0xdd, 0xff), 2, Qt::SolidLine));
            painter->drawRect(rectbox);
            painter->setPen(QPen(Qt::black, 2, Qt::SolidLine));
            painter->drawText(textbox, practicalText);
            painter->setBrush(QBrush(Qt::gray, Qt::SolidPattern));
            painter->drawEllipse(startPoint-pos(), 3, 3);
        }
        this->setZValue(0);
    }
}

void PaperAnnotation::PreviewAnnotation::resetpos1()
{
    int limit = (textperline*linenum)*2;
    int practical = 0;
    int i;
    for(i=0; i<annotation->contents().length(); i++)
    {
        if(annotation->contents().at(i).toLatin1()>0)
        {
            practical++;
        }
        else
        {
            practical++;
            practical++;
        }

        if(practical>limit)
        {
            break;
        }
    }
    if(i<annotation->contents().length())
    {
        practicalText = QString("%1%2").arg(
                    annotation->contents().mid(
                        0,
                        i-4), "......");
    }
    else
    {
        practicalText = annotation->contents();
    }

    isLeft = annotation->boundary().x()>0.5?false:true;

    if(isLeft)
    {
        if(textlength>textperline*linenum)
        {
            startPoint = QPointF(annotation->boundary().x()*width*this->scale,
                                 annotation->boundary().y()*scale*height + index*height*scale);
            endPoint = QPointF(0,
                               ((annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)* linenum>0)?
                                    (annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*linenum):0)
                               +index*height*scale);
            setPos(startPoint.x()-boundingRect().width(), startPoint.y()-boundingRect().height());
        }
        else
        {
            int pract_linenum = practical_linenum;;
            startPoint = QPointF(annotation->boundary().x()*width*this->scale,
                                 annotation->boundary().y()*scale*height + index*height*scale);
            endPoint = QPointF(0, ((annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*pract_linenum>0)?
                                       (annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*pract_linenum):0)
                               +index*height*scale);
            setPos(startPoint.x()-boundingRect().width(), startPoint.y()-boundingRect().height());
        }
    }
    else
    {
        if(textlength>textperline*linenum)
        {
            startPoint = QPointF(annotation->boundary().x()*width*this->scale,
                                 annotation->boundary().y()*scale*height + index*height*scale);
            endPoint = QPointF(width*scale,
                               ((annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*linenum>0)?
                                    (annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*linenum):0)
                               +index*height*scale);
            setPos(startPoint.x(), startPoint.y()-boundingRect().height());
        }
        else
        {
            int pract_linenum = practical_linenum;
            qDebug() << practical_linenum;
            startPoint = QPointF(annotation->boundary().x()*width*this->scale,
                                 annotation->boundary().y()*scale*height + index*height*scale);
            endPoint = QPointF(width*scale,
                               ((annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*pract_linenum>0)?
                                    (annotation->boundary().y()*scale*height-(textpointsize+textlinespacing)*pract_linenum):0)
                               +index*height*scale);
            setPos(startPoint.x(), startPoint.y()-boundingRect().height());
        }
    }


    setFlag(QGraphicsItem::ItemIsMovable, false);
}

void PaperAnnotation::PreviewAnnotation::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    isSelected = true;
    emit this->needRefresh();
    //this->setGraphicsEffect(shadow_effect);
    update();
}

void PaperAnnotation::PreviewAnnotation::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    isSelected = false;
    emit this->needRefresh();
    //this->setGraphicsEffect(no_shadow_effect);
    update();
}

Poppler::Annotation *PaperAnnotation::PreviewAnnotation::return_annotation()
{
    return this->annotation;
}

Poppler::Annotation *PaperAnnotation::HighlightAnnotation::return_annotation()
{
    return this->annotation;
}

Poppler::Annotation *PaperAnnotation::LinkedTextAnnotation::return_annotation()
{
    return this->annotation;
}

Poppler::Annotation *PaperAnnotation::LinkAnnotation::return_annotation()
{
    return this->annotation;
}
