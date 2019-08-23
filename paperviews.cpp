#include <paperviews.h>

#define LINE 0
#define ARROW 1
#define CIRCLE 2
#define RECTANGLE 3
#define FLATTEXT 4
#define POPUPTEXT 5
#define SPACING 5

MainScene::MainScene()
{
    document = new AutoDocument;
    QThread *thread = new QThread(this);
    document->moveToThread(thread);
    thread->start();
    QDesktopWidget *mydesk = QApplication::desktop();
    xres = mydesk->physicalDpiX();
    yres = mydesk->physicalDpiY();
    connect(document, &AutoDocument::imageCompleted, this, &MainScene::updateScene);

}

void MainScene::loadFile(const QString &addr)
{
//    this->filename = addr;
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

    /* set hidden */
    for(int pageidx=0; pageidx<document->document->numPages(); pageidx++)
    {
        for(int i=0;i<document->document->page(pageidx)->annotations().length();i++)
        {
            int type = document->document->page(pageidx)->annotations().at(i)->subType();
            if(type==1 || type==2 || type==3 || type==6)
            {
                document->document->page(pageidx)->annotations().at(i)->setFlags(Poppler::Annotation::Hidden);
            }
        }
    }


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
        pages.append(new PaperItem(i, (long)(&(document->images[0]))));
        pages.at(i)->setPos(0, image->height()*i+SPACING*i);
        addItem(pages.at(i));
    }

    setSceneRect(0, 0, width, height);
    document->startTimer();

    for(int pageidx=0; pageidx<document->document->numPages(); pageidx++)
    {
        for(int i=0;i<document->document->page(pageidx)->annotations().length();i++)
        {
            switch(document->document->page(pageidx)->annotations().at(i)->subType())
            {
            case 1:  // Text Annotation
            {
                Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                if(annotation->textType()==1)
                {
                    annotations.append(new PaperAnnotation::FlatTextAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    this->addItem(annotations.at(annotations.length()-1));
                    // 添加删除接口
                    connect((PaperAnnotation::FlatTextAnnotation *)(annotations.last()), &PaperAnnotation::FlatTextAnnotation::deleteSelf,
                            this, &MainScene::removeCertainItem);
                    //                    this->removeItem(tmprectitem);
                }
                else {
                    annotations.append(new PaperAnnotation::PopupTextAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    this->addItem(annotations.at(annotations.length()-1));
                    connect((PaperAnnotation::PopupTextAnnotation *)(annotations.last()), &PaperAnnotation::PopupTextAnnotation::deleteSelf,
                            this, &MainScene::removeCertainItem);
                    annotations.append(new PaperAnnotation::PreviewAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    this->addItem(annotations.last());
                }
                break;
            }
            case 3:  // Geom Annotation
            {
                Poppler::GeomAnnotation *annotation = (Poppler::GeomAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                annotations.append(new PaperAnnotation::GeomAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));
                // 添加删除接口
                connect((PaperAnnotation::GeomAnnotation *)(annotations.last()), &PaperAnnotation::GeomAnnotation::deleteSelf,
                        this, &MainScene::removeCertainItem);
                break;
            }
            case 2:  // LineAnnotation
            {
                Poppler::LineAnnotation *annotation = (Poppler::LineAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                annotations.append(new PaperAnnotation::LineAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));

                // 添加删除接口
                connect((PaperAnnotation::LineAnnotation *)(annotations.last()), &PaperAnnotation::LineAnnotation::deleteSelf,
                        this, &MainScene::removeCertainItem);

                QLinkedListIterator<QPointF> rwIterator(annotation->linePoints());
                startPoint = rwIterator.next();
                rwIterator.toBack();
                endPoint = rwIterator.previous();
                break;
            }
            case 6:  // Ink Annotation
            {
                Poppler::InkAnnotation *annotation = (Poppler::InkAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                annotations.append(new PaperAnnotation::InkAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));
                break;
            }
            }
        }
    }
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
            //document->document->page(index)->addAnnotation(lineannotation);
            annotations.append(new PaperAnnotation::LineAnnotation(index, lineannotation, width, imageheight, scale));
            annotations.at(annotations.length()-1)->scale = scale;
            this->addItem(annotations.at(annotations.length()-1));
            // 添加删除接口
            connect((PaperAnnotation::LineAnnotation *)(annotations.last()), &PaperAnnotation::LineAnnotation::deleteSelf,
                    this, &MainScene::removeCertainItem);
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
            //document->document->page(index)->addAnnotation(geomannotation);
            annotations.append(new PaperAnnotation::GeomAnnotation(index, geomannotation, width, imageheight, scale));
            this->addItem(annotations.at(annotations.length()-1));
            // 添加删除接口
            connect((PaperAnnotation::GeomAnnotation *)(annotations.last()), &PaperAnnotation::GeomAnnotation::deleteSelf,
                    this, &MainScene::removeCertainItem);
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
            // 添加删除接口
            connect((PaperAnnotation::GeomAnnotation *)(annotations.last()), &PaperAnnotation::GeomAnnotation::deleteSelf,
                    this, &MainScene::removeCertainItem);
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
    if(text==tr("箭头"))
    {
        shape = LINE;
    }
    else if(text==tr("箭头"))
    {
        shape = ARROW;
    }
    else if(text==tr("圆形"))
    {
        shape = CIRCLE;
    }
    else if(text==tr("方形"))
    {
        shape = RECTANGLE;
    }
    else if(text==tr("文字"))
    {
        shape = FLATTEXT;
    }
    else if(text==tr("注释"))
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
    //document->document->page(index)->addAnnotation(annotation);
    annotations.append(new PaperAnnotation::FlatTextAnnotation(index, annotation, width/scale, imageheight/scale, scale));
    this->addItem(annotations.at(annotations.length()-1));
    // 添加删除接口
    connect((PaperAnnotation::FlatTextAnnotation *)(annotations.last()), &PaperAnnotation::FlatTextAnnotation::deleteSelf,
            this, &MainScene::removeCertainItem);
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
    //document->document->page(index)->addAnnotation(annotation);
    annotations.append(new PaperAnnotation::PopupTextAnnotation(index, annotation, width, imageheight, scale));
    this->addItem(annotations.at(annotations.length()-1));
    // 添加删除接口
    connect((PaperAnnotation::PopupTextAnnotation *)(annotations.last()), &PaperAnnotation::PopupTextAnnotation::deleteSelf,
            this, &MainScene::removeCertainItem);
}

void MainScene::updateItem(int index)
{

}

void MainScene::updateScene()
{
    update();
}

void MainScene::removeCertainItem()
{
    qDebug() << "成功删除该项目";
    this->removeItem((PaperAnnotation::Annotation *)sender());
    for(int i=0; i<this->annotations.length();i++)
    {
        if(this->annotations.at(i)==sender())
        {
            this->annotations.remove(i);
        }
    }

    delete sender();
}

void MainScene::savePDF()
{
    QFile *file = new QFile(this->filename);
//    if(!file->open(QIODevice::WriteOnly))
//    {
//        qDebug() << "open failed";
//    }
//    else
//    {
        qDebug() << "open successed";
        this->document->document->pdfConverter()->setOutputFileName("test.pdf");
        this->document->document->pdfConverter()->setPDFOptions(Poppler::PDFConverter::WithChanges);
        bool flag = this->document->document->pdfConverter()->convert();
        int i = document->document->pdfConverter()->lastError();
        qDebug() << i;
//    }
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
    this->setStyleSheet("background-color:gray;");
    this->setWindowFlags(Qt::FramelessWindowHint);
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
            point = mapToScene(QPoint(this->x()/1.2+this->width()/2/1.2, this->y()/1.2+this->height()/2/1.2));
            scale(1/1.2, 1/1.2);
            scalefactor*=1/1.2;
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
    mainscene->document->scale = scalefactor;
    mainscene->scale = scalefactor;

    //    for(int i=0; i<mainscene->annotations.length();i++)
    //    {
    //        mainscene->removeItem(mainscene->annotations.at(i));
    //        delete mainscene->annotations.at(i);
    //    }
    //    mainscene->annotations.clear();
    //    mainscene->annotations.clear();
    mainscene->document->indexes.clear();

    mainscene->document->timer->stop();

    for(int i=0; i<mainscene->annotations.length(); i++)
    {
        mainscene->annotations.at(i)->scale = mainscene->scale;
    }

    int idx = mainscene->currentpage;
    QImage *image1 = new QImage(mainscene->document->document->page(idx)->renderToImage(mainscene->xres*mainscene->scale, mainscene->yres*mainscene->scale, -1, -1, -1, -1));
    QImage *tmpimage = mainscene->document->images[idx];
    delete tmpimage;
    mainscene->document->images[idx] = image1;

    mainscene->width = image1->width();
    mainscene->height = image1->height()*mainscene->document->document->numPages();

    setSceneRect(-200, 0, mainscene->width+200, mainscene->height);

    update();
    mainscene->update();
    mainscene->pages.at(idx)->update();

    QMatrix matrix;
    matrix.scale(1, 1);
    matrix.rotate(0);
    setMatrix(matrix);
    centerPoint = currentScenePoint*scalefactor/oldscalefactor+lengthPoint;
    centerOn(centerPoint.x(), centerPoint.y());

    oldscalefactor = scalefactor;

    for(int i=0; i<mainscene->document->document->numPages(); i++)
    {
        mainscene->pages.at(i)->setPos(0, image1->height()*i+SPACING*i);
        if(i!=idx)
        {
            mainscene->document->indexes.append(i);
        }
    }

    for(int i=0; i<this->mainscene->annotations.length(); i++)
    {
        this->mainscene->annotations.at(i)->width = mainscene->width/scalefactor;
        this->mainscene->annotations.at(i)->height = mainscene->height/mainscene->document->document->numPages()/scalefactor;
        this->mainscene->annotations.at(i)->scale = scalefactor;
        this->mainscene->annotations.at(i)->resetpos();
        this->mainscene->annotations.at(i)->update();
    }

    for(int i=0; i<this->items().length(); i++)
    {
        if(QString(typeid(*(this->items().at(i))).name()).contains("Annotation"))
        {
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->width = mainscene->width/scalefactor;
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->height = mainscene->height/mainscene->document->document->numPages()/scalefactor;
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->scale = scalefactor;
            if((QString(typeid(*(this->items().at(i))).name()).contains("Preview")))
            {
                ((PaperAnnotation::PreviewAnnotation *)(this->items().at(i)))->resetpos1();
            }
            else
            {
                ((PaperAnnotation::Annotation *)(this->items().at(i)))->resetpos();
            }
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->update();
        }
    }

    //    for(int pageidx=0; pageidx<mainscene->document->document->numPages(); pageidx++)
    //    {
    //        for(int i=0;i<this->mainscene->annotations.length();i++)
    //        {
    //            switch(this->mainscene->annotations.at(i)->return_annotation()->subType())
    //            {
    //            case 1:  // Text Annotation
    //            {
    //                Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(mainscene->document->document->page(pageidx)->annotations().at(i));
    //                if(annotation->textType()==1)
    //                {
    //                    mainscene->annotations.append(new PaperAnnotation::FlatTextAnnotation(pageidx, annotation, mainscene->width/scalefactor, mainscene->height/mainscene->document->document->numPages()/scalefactor, scalefactor));
    //                    mainscene->addItem(mainscene->annotations.at(mainscene->annotations.length()-1));
    //                }
    //                else {
    //                    mainscene->annotations.append(new PaperAnnotation::PopupTextAnnotation(pageidx, annotation, mainscene->width/scalefactor, mainscene->height/mainscene->document->document->numPages()/scalefactor, mainscene->scale));
    //                    mainscene->addItem(mainscene->annotations.at(mainscene->annotations.length()-1));
    //                    mainscene->annotations.append(new PaperAnnotation::PreviewAnnotation(pageidx, annotation, mainscene->width/scalefactor, mainscene->height/mainscene->document->document->numPages()/scalefactor, mainscene->scale));
    //                    mainscene->addItem(mainscene->annotations.last());
    //                }
    //                break;
    //            }
    //            case 3:  // Geom Annotation
    //            {
    //                Poppler::GeomAnnotation *annotation = (Poppler::GeomAnnotation *)(mainscene->document->document->page(pageidx)->annotations().at(i));

    //                mainscene->annotations.append(new PaperAnnotation::GeomAnnotation(pageidx, annotation, mainscene->width/scalefactor, mainscene->height/mainscene->document->document->numPages()/scalefactor, mainscene->scale));
    //                mainscene->addItem(mainscene->annotations.at(mainscene->annotations.length()-1));
    //                break;
    //            }
    //            case 2:  // Line Annotation
    //            {
    //                Poppler::LineAnnotation *annotation = (Poppler::LineAnnotation *)(mainscene->document->document->page(pageidx)->annotations().at(i));

    //                mainscene->annotations.append(new PaperAnnotation::LineAnnotation(pageidx, annotation, mainscene->width/scalefactor, mainscene->height/mainscene->document->document->numPages()/scalefactor, mainscene->scale));
    //                mainscene->addItem(mainscene->annotations.at(mainscene->annotations.length()-1));

    //                //                QLinkedListIterator<QPointF> rwIterator(annotation->linePoints());
    //                //                mainscene->startPoint = rwIterator.next();
    //                //                rwIterator.toBack();
    //                //                mainscene->endPoint = rwIterator.previous();
    //                break;
    //            }
    //            case 6:  // Ink Annotation
    //            {
    //                Poppler::InkAnnotation *annotation = (Poppler::InkAnnotation *)(mainscene->document->document->page(pageidx)->annotations().at(i));

    //                mainscene->annotations.append(new PaperAnnotation::InkAnnotation(pageidx, annotation, mainscene->width/scalefactor, mainscene->height/mainscene->document->document->numPages()/scalefactor, scalefactor));
    //                mainscene->addItem(mainscene->annotations.at(mainscene->annotations.length()-1));
    //                break;
    //            }
    //            }
    //        }
    //    }

    connect(this, &GraphicsView::timerStarting, mainscene->document, &AutoDocument::updateImages);
    emit timerStarting();
    connect(this->mainscene->document, &AutoDocument::imageCompleted, this->mainscene, &MainScene::updateScene);
}

void GraphicsView::timeStopped()
{
    refreshtimer->stop();
}

QRectF PaperItem::boundingRect() const
{
    QImage **addr = (QImage **)(images + index*size);
    return QRectF(0, 0, (*addr)->width(), (*addr)->height());
}

void PaperItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QImage **addr = (QImage **)(images + index*size);
    painter->drawImage(0, 0, **addr);
}
