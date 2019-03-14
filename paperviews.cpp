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

    //    paperwidgets->normalwidget->pages.clear();
    //    paperwidgets->scaledwidget->pages.clear();
    //    document->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    //    document->setRenderHint(Poppler::Document::Antialiasing, 1);
    //    QVBoxLayout *layout1 = new QVBoxLayout();
    //    QVBoxLayout *layout2 = new QVBoxLayout();
    //    for(int i=0; i<document->numPages();i++)
    //    {
    //        paperwidgets->normalwidget->pages.append(new QLabel());
    //        paperwidgets->scaledwidget->pages.append(new QLabel());
    //        auto image = document->page(i)->renderToImage(xres, yres, -1, -1, -1, -1, Poppler::Page::Rotate0);
    //        if(i==1)
    //        {
    //            width = image.width();
    //            height = image.height()*document->numPages();
    //        }
    //        paperwidgets->normalwidget->pages.at(i)->setPixmap(QPixmap::fromImage(image));
    //        paperwidgets->normalwidget->pages.at(i)->resize(image.size());
    //        layout1->addWidget(paperwidgets->normalwidget->pages.at(i));

    //        paperwidgets->scaledwidget->pages.at(i)->setPixmap(QPixmap::fromImage(image));
    //        paperwidgets->scaledwidget->pages.at(i)->resize(image.size());
    //        layout2->addWidget(paperwidgets->scaledwidget->pages.at(i));
    //    }
    //    layout1->setAlignment(Qt::AlignTop);
    //    layout2->setAlignment(Qt::AlignTop);
    //    paperwidgets->normalwidget->setLayout(layout1);
    //    paperwidgets->scaledwidget->setLayout(layout2);
    //    paperwidgets->addWidget(paperwidgets->normalwidget);
    //    paperwidgets->addWidget(paperwidgets->scaledwidget);
    //    paperwidgets->setCurrentWidget(paperwidgets->scaledwidget);
    //    paperproxywidget = addWidget(paperwidgets);
    //    paperproxywidget->setPos(0, 0);
    //    paperproxywidget->setZValue(-100);

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
        addItem(pages.at(i));
    }

    for(int i=0;i<document->page(1)->annotations().length();i++)
    {
        if(document->page(1)->annotations().at(i)->subType()==1)
        {
            Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(document->page(1)->annotations().at(i));
            if(annotation->textType()==1)
            {
                annotations.append(new PaperAnnotation::FlatTextAnnotation(annotation, width, height/document->numPages()));
                this->addItem(annotations.at(annotations.length()-1));
            }
        }
    }

    setSceneRect(0, 0, width, height);
}

void MainScene::updateSize()
{

    //    refreshtimer->stop();
    //    paperwidgets->removeWidget(paperwidgets->scaledwidget);
    //    delete paperwidgets->scaledwidget;
    //    paperwidgets->scaledwidget = new PaperWidget();
    //    QVBoxLayout *layout = new QVBoxLayout();
    //    for(int i=0; i<document->numPages();i++)
    //    {
    //        paperwidgets->scaledwidget->pages.append(new QLabel());
    //        auto image = document->page(i)->renderToImage(xres*scale, yres*scale, -1, -1, -1, -1, Poppler::Page::Rotate0);
    //        paperwidgets->scaledwidget->pages.at(i)->setPixmap(QPixmap::fromImage(image));
    //        paperwidgets->scaledwidget->pages.at(i)->resize(image.size());
    //        layout->addWidget(paperwidgets->scaledwidget->pages.at(i));
    //    }
    //    layout->setAlignment(Qt::AlignTop);
    //    paperwidgets->scaledwidget->setLayout(layout);
    //    paperwidgets->addWidget(paperwidgets->scaledwidget);
    //    paperwidgets->setCurrentWidget(paperwidgets->scaledwidget);
    //    //paperwidgets->setFixedSize(width*scale, height*scale*document->numPages());
    //    paperwidgets->setFixedWidth(width*scale);
    //    paperwidgets->setFixedHeight(height*scale);
    //    setSceneRect(0, 0, width, height);

    //    paperproxywidget->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    //    paperproxywidget->setFlag(QGraphicsItem::ItemIsMovable, false);
    //    paperproxywidget->setFlag(QGraphicsItem::ItemIsSelectable, false);
    //    paperproxywidget->setZValue(-100);
    //    for(int i=0; i<document->numPages(); i++)
    //    {
    //        QImage *image = new QImage(document->page(i)->renderToImage(xres*scale, yres*scale, -1, -1, -1, -1));
    //        if(i==0)
    //        {
    //            width = image->width();
    //            height = image->height()*document->numPages();
    //        }
    //        //delete pages.at(i)->image;
    //        //pages.at(i)->image = image;
    //    }
    //    update();
}

MainFrame::MainFrame()
{
    graphicsview = new GraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(graphicsview);
    setLayout(layout);

    connect(graphicsview, &GraphicsView::sizeChanged, this, &MainFrame::setMatrix);

    graphicsview->setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsview->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsview->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

GraphicsView *MainFrame::view() const
{
    return graphicsview;
}

void MainFrame::setMatrix(qreal deltascale, qreal deltarotate)
{
    //    scale*= deltascale;
    //    ((MainScene *)graphicsview->scene())->paperwidgets->setFixedWidth(((MainScene *)graphicsview->scene())->width);
    //    ((MainScene *)graphicsview->scene())->paperwidgets->setFixedHeight(((MainScene *)graphicsview->scene())->height);
    //    ((MainScene *)graphicsview->scene())->paperproxywidget->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
    //    ((MainScene *)graphicsview->scene())->paperwidgets->setCurrentWidget(
    //            ((MainScene *)graphicsview->scene())->paperwidgets->normalwidget);
    //    ((MainScene *)graphicsview->scene())->paperproxywidget->setZValue(-100);
    //    ((MainScene *)graphicsview->scene())->scale = scale;

    //    ((MainScene *)graphicsview->scene())->refreshtimer->stop();
    //    ((MainScene *)graphicsview->scene())->refreshtimer->start(500);
}

SideScene::SideScene()
{

}

GraphicsView::GraphicsView(MainFrame *frame) : QGraphicsView(), frame(frame)
{
    refreshtimer = new QTimer();
    connect(this->refreshtimer, &QTimer::timeout, this, &GraphicsView::updateSize);
}

void GraphicsView::wheelEvent(QWheelEvent *ev)
{
    if(ev->modifiers() & Qt::ControlModifier)
    {
        if(ev->delta()>0)
        {
            point = mapToScene(QPoint(this->x()*1.2+this->width()/2*1.2, this->y()*1.2+this->height()/2*1.2));
            scale(1.2, 1.2);
            scalefactor*=1.2;
            refreshtimer->stop();
            refreshtimer->start(500);
        }
        else
        {
            point = mapToScene(QPoint(this->x()*0.8+this->width()/2*0.8, this->y()*0.8+this->height()/2*0.8));
            scale(0.8, 0.8);
            scalefactor*=0.8;
            refreshtimer->stop();
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
    MainScene *scene = (MainScene *)(this->scene());
    for(int i=0; i<scene->annotations.length(); i++)
    {
        scene->annotations.at(i)->scale = scalefactor;
    }

//    for(int i=0; i<scene->document->numPages(); i++)
//    {
//        QImage *image = new QImage(scene->document->page(i)->renderToImage(scene->xres*scalefactor, scene->yres*scalefactor, -1, -1, -1, -1));
//        if(i==0)
//        {
//            scene->width = image->width();
//            scene->height = image->height()*scene->document->numPages();
//        }
//        delete scene->pages.at(i)->image;
//        scene->pages.at(i)->image = image;
//        scene->pages.at(i)->update();
//    }

    QMatrix matrix;
    matrix.scale(1, 1);
    matrix.rotate(0);
    setMatrix(matrix);

    scene->clear();
    scene->pages.clear();
    for(int i=0; i<scene->document->numPages(); i++)
    {
        QImage *image = new QImage(scene->document->page(i)->renderToImage(scene->xres*scalefactor, scene->yres*scalefactor, -1, -1, -1, -1));
        if(i==0)
        {
            scene->width = image->width();
            scene->height = image->height()*scene->document->numPages();
        }
        scene->pages.append(new PaperItem(i, image));
        scene->addItem(scene->pages.at(i));
    }

    scene->setSceneRect(0, 0, scene->width, scene->height);
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
    setPos(QPoint(0, image->height()*index));
}
