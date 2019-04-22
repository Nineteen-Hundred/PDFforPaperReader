#include <paperviews.h>

MainScene::MainScene()
{
    QDesktopWidget *mydesk = QApplication::desktop();
    xres = mydesk->physicalDpiX();
    yres = mydesk->physicalDpiY();

    refreshtimer = new QTimer();
}

void MainScene::loadFile(const QString &addr)
{
    document = Poppler::Document::load(addr);
    if (!document || document->isLocked()) {
        delete document;
        return;
    }

    document->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    document->setRenderHint(Poppler::Document::Antialiasing, 1);

    for(int i=0; i<document->numPages(); i++)
    {
        QImage *image = new QImage(document->page(i)->renderToImage(xres, yres, -1, -1, -1, -1));
        if(i==0)
        {
            width = image->width();
            height = image->height()*document->numPages();
        }
        pages.append(new PaperItem(i, image));
        pages.at(i)->setPos(QPoint(0, image->height()*i));
        addItem(pages.at(i));
    }

    for(int pageidx=0; pageidx<document->numPages(); pageidx++)
    {
        for(int i=0;i<document->page(pageidx)->annotations().length();i++)
        {
            switch(document->page(pageidx)->annotations().at(i)->subType())
            {
            case 1:  // Text Annotation
            {
                Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(document->page(pageidx)->annotations().at(i));
                if(annotation->textType()==1)
                {
                    annotations.append(new PaperAnnotation::FlatTextAnnotation(pageidx, annotation, width, height/document->numPages()));
                    this->addItem(annotations.at(annotations.length()-1));
                }
                break;
            }
            case 3:  // Geom Annotation
            {
                Poppler::GeomAnnotation *annotation = (Poppler::GeomAnnotation *)(document->page(pageidx)->annotations().at(i));

                annotations.append(new PaperAnnotation::GeomAnnotation(pageidx, annotation, width, height/document->numPages()));
                this->addItem(annotations.at(annotations.length()-1));
                break;
            }
            case 2:
            {
                Poppler::LineAnnotation *annotation = (Poppler::LineAnnotation *)(document->page(pageidx)->annotations().at(i));

                annotations.append(new PaperAnnotation::LineAnnotation(pageidx, annotation, width, height/document->numPages()));
                this->addItem(annotations.at(annotations.length()-1));
                break;
            }
            }
        }
    }

    setSceneRect(0, 0, width, height);
}

void MainScene::updateSize()
{
    qDebug() << "success";
    clear();
    pages.clear();

    for(int i=0; i<annotations.length(); i++)
    {
        annotations.at(i)->scale = scale;
    }

    for(int i=0; i<document->numPages(); i++)
    {
        QImage *image = new QImage(document->page(i)->renderToImage(xres*scale, yres*scale, -1, -1, -1, -1));
        if(i==0)
        {
            width = image->width();
            height = image->height()*document->numPages();
        }
        pages.append(new PaperItem(i, image));
        pages.at(i)->setPos(QPoint(0, image->height()*i));
        addItem(pages.at(i));
    }

    setSceneRect(0, 0, width, height);

    update();
}

void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(isPressing)
    {
        event->accept();
        switch(shape)
        {
        case 0:
            endPoint = event->scenePos();
            tmplineitem->setLine(QLineF(startPoint-startPoint, endPoint-startPoint));
            tmplineitem->update();
            tmplineitem->show();
            break;
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
    if(isDrawing)
    {
        event->accept();
        QPen pen;
        pen.setColor(QColor(0, 160, 230));
        pen.setWidth(2);
        startPoint = event->scenePos();
        switch(shape)
        {
        case 0: // Line
            tmplineitem = new QGraphicsLineItem;
            startPoint = event->scenePos();
            endPoint = event->scenePos();
            tmplineitem->setPen(pen);
            tmplineitem->setLine(QLineF(startPoint, endPoint));
            tmplineitem->setPos(event->scenePos());
            addItem(tmplineitem);
            break;
        case 1:
            break;
        default:
            tmplineitem = new QGraphicsLineItem;
            startPoint = QPointF(event->scenePos().x()/scale/width, event->scenePos().y()/scale/height);
            endPoint = QPointF(event->scenePos().x()/scale/width, event->scenePos().y()/scale/height);
            tmplineitem->setPen(pen);
            tmplineitem->setLine(QLineF(startPoint, endPoint));
            break;
        }
        isPressing = true;
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void MainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    isPressing = false;
    endPoint = event->scenePos();
    switch(shape)
    {

    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void MainScene::changeIsDrawing()
{
    QAction *action = (QAction *)sender();
    if(action->text()==tr("Line"))
    {
        shape = 0;
    }
    isDrawing = !isDrawing;
}

void MainScene::setCurrentShape(int i)
{
    shape = i;
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
        if(ev->delta()>0)
        {
            refreshtimer->stop();
            point = mapToScene(QPoint(this->x()*1.2+this->width()/2*1.2, this->y()*1.2+this->height()/2*1.2));
            scale(1.2, 1.2);
            scalefactor*=1.2;
            mainscene->scale = scalefactor;
            refreshtimer->start(500);
        }
        else
        {
            refreshtimer->stop();
            point = mapToScene(QPoint(this->x()*0.8+this->width()/2*0.8, this->y()*0.8+this->height()/2*0.8));
            scale(0.8, 0.8);
            scalefactor*=0.8;
            mainscene->scale = scalefactor;
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
    qDebug() << "updateSize";
    refreshtimer->stop();

    QMatrix matrix;
    matrix.scale(1, 1);
    matrix.rotate(0);
    setMatrix(matrix);
}

void GraphicsView::changeCursor(bool isDrawing)
{
    if(isDrawing)
    {
        setCursor(Qt::CrossCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

PaperItem::~PaperItem()
{
    delete image;
}

QRectF PaperItem::boundingRect() const
{
    return QRectF(0, 0, image->width(), image->height());
}

void PaperItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(0, 0, *image);
}
