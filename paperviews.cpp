#include <paperviews.h>

#define LINE 0
#define ARROW 1
#define CIRCLE 2
#define RECTANGLE 3
#define FLATTEXT 4
#define POPUPTEXT 5

MainScene::MainScene()
{
    document = new AutoDocument;
    QThread *thread = new QThread(this);
    document->moveToThread(thread);
    thread->start();
    QDesktopWidget *mydesk = QApplication::desktop();
    xres = mydesk->physicalDpiX();
    yres = mydesk->physicalDpiY();
}

void MainScene::loadFile(const QString &addr)
{
    document->document = Poppler::Document::load(addr);
    if (!document || document->document->isLocked()) {
        delete document;
        return;
    }

    for(int i=0; i<document->document->numPages(); i++)
    {
        indexes.append(i);
    }

    document->document->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    document->document->setRenderHint(Poppler::Document::Antialiasing, 1);

    for(int i=0; i<document->document->numPages(); i++)
    {
        QImage *image = new QImage(document->document->page(i)->renderToImage(xres, yres, -1, -1, -1, -1));
        if(i==0)
        {
            width = image->width();
            height = image->height()*document->document->numPages();
        }
        document->images[i] = image;
        QImage *tmptmp = *(&(document->images[0]));
        qDebug() << "testfor" << tmptmp->width() << tmptmp->height() << &(document->images[0]);
        pages.append(new PaperItem(i, (long)(&(document->images[0]))));
        pages.at(i)->setPos(0, image->height()*i);
        addItem(pages.at(i));
    }

    for(int pageidx=0; pageidx<document->document->numPages(); pageidx++)
    {
        for(int i=0;i<document->document->page(pageidx)->annotations().length();i++)
        {
            switch(document->document->page(pageidx)->annotations().at(i)->subType())
            {
            case 1:  // Text Annotation
            {
                Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                if(annotation->textType()==1)
                {
                    annotations.append(new PaperAnnotation::FlatTextAnnotation(pageidx, annotation, width, height/document->document->numPages()));
                    this->addItem(annotations.at(annotations.length()-1));
                }
                else {
                    annotations.append(new PaperAnnotation::PopupTextAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    this->addItem(annotations.at(annotations.length()-1));
                }
                break;
            }
            case 3:  // Geom Annotation
            {
                Poppler::GeomAnnotation *annotation = (Poppler::GeomAnnotation *)(document->document->page(pageidx)->annotations().at(i));

                annotations.append(new PaperAnnotation::GeomAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));
                break;
            }
            case 2:
            {
                Poppler::LineAnnotation *annotation = (Poppler::LineAnnotation *)(document->document->page(pageidx)->annotations().at(i));

                annotations.append(new PaperAnnotation::LineAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));

                QLinkedListIterator<QPointF> rwIterator(annotation->linePoints());
                startPoint = rwIterator.next();
                rwIterator.toBack();
                endPoint = rwIterator.previous();
                break;
            }
            case 6:  // Ink Annotation
            {
                Poppler::InkAnnotation *annotation = (Poppler::InkAnnotation *)(document->document->page(pageidx)->annotations().at(i));

                annotations.append(new PaperAnnotation::InkAnnotation(pageidx, annotation, width, height/document->document->numPages()));
                this->addItem(annotations.at(annotations.length()-1));
                break;
            }
            }
        }
    }

    setSceneRect(0, 0, width, height);
    document->startTimer();
}

void MainScene::updateSize()
{

}

void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(isPressing)
    {
        event->accept();
        switch(shape)
        {
        case LINE:
        {
            endPoint = event->scenePos();
            tmplineitem->setLine(QLineF(startPoint-startPoint, endPoint-startPoint));
            tmplineitem->update();
            tmplineitem->show();
            break;
        }
        case CIRCLE:
        {
            endPoint = event->scenePos();
            tmpellipseitem->setRect(0, 0, abs(startPoint.x()-endPoint.x()), abs(startPoint.y()-endPoint.y()));
            tmpellipseitem->update();
            tmpellipseitem->show();
            break;
        }
        case RECTANGLE:
        {
            endPoint = event->scenePos();
            tmprectitem->setRect(0, 0, abs(startPoint.x()-endPoint.x()), abs(startPoint.y()-endPoint.y()));
            tmprectitem->update();
            tmprectitem->show();
            break;
        }
        default:
            break;
        }
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void MainScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(isDrawing && event->button() == Qt::LeftButton)
    {
        isPressing = true;
        event->accept();
        QPen pen;
        pen.setColor(QColor(0, 160, 230));
        pen.setWidth(2);
        startPoint = event->scenePos();
        startPoint = event->scenePos();
        endPoint = event->scenePos();
        switch(shape)
        {
        case LINE:
        {
            tmplineitem = new QGraphicsLineItem;
            tmplineitem->setPen(pen);
            tmplineitem->setLine(QLineF(startPoint, endPoint));
            tmplineitem->setPos(event->scenePos());
            addItem(tmplineitem);
            break;
        }
        case FLATTEXT:
        {
            QFont font;
            font.setPointSize(10);
            AnnotationDialog::FlatTextDialog *dialog = new AnnotationDialog::FlatTextDialog("", font, Qt::red);
            connect(dialog, &AnnotationDialog::FlatTextDialog::configUpdated, this, &MainScene::newFlatText);
            dialog->exec();
            isPressing = false;
            break;
        }
        case POPUPTEXT:
        {
            QFont font;
            font.setPointSize(10);
            AnnotationDialog::FlatTextDialog *dialog = new AnnotationDialog::FlatTextDialog("", font, Qt::red);
            connect(dialog, &AnnotationDialog::FlatTextDialog::configUpdated, this, &MainScene::newPopupText);
            dialog->exec();
            isPressing = false;
            break;
        }
        case CIRCLE:
        {
            tmpellipseitem = new QGraphicsEllipseItem;
            tmpellipseitem->setPen(pen);
            tmpellipseitem->setPos(event->scenePos());
            addItem(tmpellipseitem);
            break;
        }
        case RECTANGLE:
        {
            tmprectitem = new QGraphicsRectItem;
            tmprectitem->setPen(pen);
            tmprectitem->setPos(event->scenePos());
            addItem(tmprectitem);
            break;
        }
        default:
        {
            tmplineitem = new QGraphicsLineItem;
            startPoint = event->scenePos();
            endPoint = event->scenePos();
            tmplineitem->setPen(pen);
            tmplineitem->setLine(QLineF(startPoint, endPoint));
            tmplineitem->setPos(event->scenePos());
            addItem(tmplineitem);
            break;
        }
        }
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void MainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(isPressing)
    {
        event->accept();
        isPressing = false;
        endPoint = event->scenePos();
        int imageheight = height/document->document->numPages();
        switch(shape)
        {
        case LINE:
        {
            int index = (int)(tmplineitem->pos().y()/scale/imageheight);
            Poppler::LineAnnotation *lineannotation = new Poppler::LineAnnotation(Poppler::LineAnnotation::StraightLine);
            QRectF boundary = QRectF(tmplineitem->pos().x()/scale/width, (((int)(tmplineitem->pos().y()))%(int)imageheight)/scale/imageheight,
                                     tmplineitem->boundingRect().width()/scale/width, tmplineitem->boundingRect().height()/scale/imageheight);
            lineannotation->setBoundary(boundary);
            QLinkedList<QPointF> points;
            points.append(QPointF(startPoint.x()/scale/width, ((int)startPoint.y())%(int)imageheight/scale/imageheight));
            points.append(QPointF(endPoint.x()/scale/width, ((int)endPoint.y())%(int)imageheight/scale/imageheight));
            lineannotation->setLinePoints(points);
            lineannotation->style().setWidth(4);
            document->document->page(index)->addAnnotation(lineannotation);
            qDebug() << startPoint.x()/scale/width << ((int)startPoint.y())%(int)imageheight/scale/imageheight;
            annotations.append(new PaperAnnotation::LineAnnotation(index, lineannotation, width, imageheight, scale));
            qDebug() << annotations.last()->pos().x() << annotations.last()->pos().y() << annotations.last()->boundingRect().width() << annotations.last()->boundingRect().height();
            annotations.at(annotations.length()-1)->scale = scale;
            qDebug() << scale;
            this->addItem(annotations.at(annotations.length()-1));
            this->removeItem(tmplineitem);
            break;
        }
        case CIRCLE:
        {
            int index = (int)(tmpellipseitem->pos().y()/scale/imageheight);
            Poppler::GeomAnnotation *geomannotation = new Poppler::GeomAnnotation();
            geomannotation->setGeomType(Poppler::GeomAnnotation::InscribedCircle);
            QRectF boundary = QRectF(tmpellipseitem->pos().x()/scale/width, (((int)(tmpellipseitem->pos().y()))%(int)imageheight)/scale/imageheight,
                                     tmpellipseitem->boundingRect().width()/scale/width, tmpellipseitem->boundingRect().height()/scale/imageheight);
            geomannotation->setBoundary(boundary);
            QLinkedList<QPointF> points;
            points.append(QPointF(startPoint.x()/scale/width, ((int)startPoint.y())%(int)imageheight/scale/imageheight));
            points.append(QPointF(endPoint.x()/scale/width, ((int)endPoint.y())%(int)imageheight/scale/imageheight));
            geomannotation->style().setWidth(4);
            document->document->page(index)->addAnnotation(geomannotation);
            annotations.append(new PaperAnnotation::GeomAnnotation(index, geomannotation, width, imageheight, scale));
            this->addItem(annotations.at(annotations.length()-1));
            this->removeItem(tmpellipseitem);
            break;
        }
        case RECTANGLE:
        {
            int index = (int)(tmprectitem->pos().y()/scale/imageheight);
            Poppler::GeomAnnotation *geomannotation = new Poppler::GeomAnnotation();
            geomannotation->setGeomType(Poppler::GeomAnnotation::InscribedSquare);
            QRectF boundary = QRectF(tmprectitem->pos().x()/scale/width, (((int)(tmprectitem->pos().y()))%(int)imageheight)/scale/imageheight,
                                     tmprectitem->boundingRect().width()/scale/width, tmprectitem->boundingRect().height()/scale/imageheight);
            geomannotation->setBoundary(boundary);
            QLinkedList<QPointF> points;
            points.append(QPointF(startPoint.x()/scale/width, ((int)startPoint.y())%(int)imageheight/scale/imageheight));
            points.append(QPointF(endPoint.x()/scale/width, ((int)endPoint.y())%(int)imageheight/scale/imageheight));
            geomannotation->style().setWidth(4);
            document->document->page(index)->addAnnotation(geomannotation);
            annotations.append(new PaperAnnotation::GeomAnnotation(index, geomannotation, width, imageheight, scale));
            this->addItem(annotations.at(annotations.length()-1));
            annotations.at(annotations.length()-1)->scale = scale;
            this->removeItem(tmprectitem);
            break;
        }
        default:
        {

        }
        }
    }
    else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void MainScene::changeIsDrawing(const QString &text, bool isDrawing)
{
    qDebug() << "changejici";
    if(text==tr("Line"))
    {
        shape = LINE;
    }
    else if(text==tr("Arrow"))
    {
        shape = ARROW;
    }
    else if(text==tr("Circle"))
    {
        shape = CIRCLE;
    }
    else if(text==tr("Rectangle"))
    {
        shape = RECTANGLE;
    }
    else if(text==tr("Flat Text"))
    {
        shape = FLATTEXT;
    }
    else if(text==tr("Popup Text"))
    {
        shape = POPUPTEXT;
    }
    else
    {
        shape = LINE;
    }
    this->isDrawing = isDrawing;
    if(isDrawing)
    {
        for(int i=0; i<pages.count();i++)
        {
            pages.at(i)->setCursor(Qt::CrossCursor);
        }
    }
    else
    {
        for(int i=0; i<pages.count();i++)
        {
            pages.at(i)->setCursor(Qt::ArrowCursor);
        }
    }
}

void MainScene::setCurrentShape(int i)
{
    shape = i;
}

void MainScene::newFlatText(const QString &text, QFont font, QColor color)
{
    int imageheight = height/document->document->numPages();
    int index = (int)(startPoint.y()/scale/imageheight);
    Poppler::TextAnnotation *annotation = new Poppler::TextAnnotation(Poppler::TextAnnotation::InPlace);
    annotation->setTextColor(color);
    annotation->setContents(text);
    annotation->setTextFont(font);
    int totallength = annotation->textFont().pointSize()*annotation->contents().length();
    int hdistance = scale*width-startPoint.x();
    int vdistance = (index+1)*scale*imageheight-startPoint.y();
    annotation->setBoundary(QRectF(startPoint.x()/scale/width, (((int)(startPoint.y()))%(int)imageheight)/scale/imageheight,
                                   (totallength/hdistance>1?hdistance:totallength)/scale/width,
                                   ((totallength/hdistance+1)*annotation->textFont().pointSize()+20>vdistance?vdistance:(totallength/hdistance+1)*scale*annotation->textFont().pointSize()+20)/scale/imageheight));
    document->document->page(index)->addAnnotation(annotation);
    annotations.append(new PaperAnnotation::FlatTextAnnotation(index, annotation, width, imageheight));
    this->addItem(annotations.at(annotations.length()-1));
}

void MainScene::newPopupText(const QString &text, QFont font, QColor color)
{
    int imageheight = height/document->document->numPages();
    int index = (int)(startPoint.y()/scale/imageheight);
    Poppler::TextAnnotation *annotation = new Poppler::TextAnnotation(Poppler::TextAnnotation::Linked);
    annotation->setTextColor(color);
    annotation->setContents(text);
    annotation->setTextFont(font);
    int totallength = annotation->textFont().pointSize()*annotation->contents().length();
    int hdistance = scale*width-startPoint.x();
    int vdistance = (index+1)*scale*imageheight-startPoint.y();
    annotation->setBoundary(QRectF(startPoint.x()/scale/width, (((int)(startPoint.y()))%(int)imageheight)/scale/imageheight,
                                   30/scale/width,
                                   30/scale/imageheight));
    document->document->page(index)->addAnnotation(annotation);
    annotations.append(new PaperAnnotation::PopupTextAnnotation(index, annotation, width, imageheight, scale));
    this->addItem(annotations.at(annotations.length()-1));
}

void MainScene::updateItem(int index)
{

}

MainFrame::MainFrame()
{
    graphicsview = new GraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(graphicsview);
    setLayout(layout);

    //graphicsview->setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsview->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsview->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsview->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

GraphicsView *MainFrame::view() const
{
    return graphicsview;
}

SideScene::SideScene()
{

}

GraphicsView::GraphicsView(MainFrame *frame) : QGraphicsView(), frame(frame)
{
    refreshtimer = new QTimer();
}

void GraphicsView::setScene(MainScene *scene)
{
    mainscene = scene;
    QGraphicsView::setScene(scene);
}

void GraphicsView::wheelEvent(QWheelEvent *ev)
{
    if(ev->modifiers() & Qt::ControlModifier)
    {
        mainscene->document->indexes.clear();
        if(!refreshtimer->isActive())
        {
            oldCenterPoint = mapToScene(QPoint(width()/2, height()/2));
            currentScenePoint = mapToScene(ev->pos());
            lengthPoint = oldCenterPoint-currentScenePoint;
            mainscene->currentpage = (int)(currentScenePoint.y()/height()/scalefactor);
        }

        if(ev->delta()>0)
        {
            refreshtimer->stop();
            point = mapToScene(QPoint(this->x()*1.2+this->width()/2*1.2, this->y()*1.2+this->height()/2*1.2));
            scale(1.2, 1.2);
            scalefactor*=1.2;
            mainscene->scale = scalefactor;
            centerPoint = currentScenePoint+lengthPoint/scalefactor*oldscalefactor;
            centerOn(centerPoint);
            refreshtimer->start(500);
        }
        else
        {
            refreshtimer->stop();
            point = mapToScene(QPoint(this->x()*0.8+this->width()/2*0.8, this->y()*0.8+this->height()/2*0.8));
            scale(0.8, 0.8);
            scalefactor*=0.8;
            mainscene->scale = scalefactor;
            QPointF lengthPoint = oldCenterPoint-currentScenePoint;
            centerPoint = currentScenePoint+lengthPoint/scalefactor*oldscalefactor;
            centerOn(centerPoint);
            refreshtimer->start(500);
        }
        ev->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(ev);
    }
}

void GraphicsView::updateSize()
{
    refreshtimer->stop();
    mainscene->annotations.clear();
    mainscene->document->scale = scalefactor;
    mainscene->scale = scalefactor;
    qDebug() << mainscene->scale;

    mainscene->document->indexes.clear();

    //sleep(1);

    mainscene->document->timer->stop();

    for(int i=0; i<mainscene->annotations.length(); i++)
    {
        mainscene->annotations.at(i)->scale = mainscene->scale;
    }

    int idx = mainscene->currentpage;
    QImage *image1 = new QImage(mainscene->document->document->page(idx)->renderToImage(mainscene->xres*mainscene->scale, mainscene->yres*mainscene->scale, -1, -1, -1, -1));
    QImage *tmpimage = mainscene->document->images[idx];
    //delete tmpimage;
    mainscene->document->images[idx] = image1;


    mainscene->width = image1->width();
    mainscene->height = image1->height()*mainscene->document->document->numPages();

    setSceneRect(0, 0, mainscene->width, mainscene->height);

    QMatrix matrix;
    matrix.scale(1, 1);
    matrix.rotate(0);
    setMatrix(matrix);
    centerPoint = currentScenePoint*scalefactor/oldscalefactor+lengthPoint;
    centerOn(centerPoint.x(), centerPoint.y());

    oldscalefactor = scalefactor;

    update();
    mainscene->update();

    for(int i=0; i<mainscene->document->document->numPages(); i++)
    {
        mainscene->pages.at(i)->setPos(0, image1->height()*i);
        if(i!=idx)
        {
            mainscene->document->indexes.append(i);
        }
    }

    mainscene->document->timer->start(500);

//    for(int i=0; i<mainscene->document->document->numPages(); i++)
//    {
//        if(i!=mainscene->currentpage)
//        {
//            QImage *image = new QImage(mainscene->document->document->page(i)->renderToImage(mainscene->xres*mainscene->scale, mainscene->yres*mainscene->scale, -1, -1, -1, -1));

//            mainscene->pages.append(new PaperItem(i, image));
//            mainscene->pages.at(i)->setPos(QPoint(0, image->height()*i));
//            mainscene->addItem(mainscene->pages.at(i));
//        }
//        else
//        {
//            mainscene->pages.append(iter1);
//        }
//    }
}

void GraphicsView::timeStopped()
{
    refreshtimer->stop();
}

QRectF PaperItem::boundingRect() const
{
    QImage **addr = (QImage **)(images + index*size);
    qDebug() << "all test" << (*addr)->width() << (*addr)->height() << index << addr;
    return QRectF(0, 0, (*addr)->width(), (*addr)->height());
}

void PaperItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QImage **addr = (QImage **)(images + index*size);
    qDebug() << "all test" << (*addr)->width() << (*addr)->height() << index << addr;
    painter->drawImage(0, 0, **addr);
}
