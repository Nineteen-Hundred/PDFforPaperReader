#include <paperviews.h>

MainScene::MainScene()
{
    paperwidgets = new PaperWidgets();
    QDesktopWidget *mydesk = QApplication::desktop();
    xres = mydesk->physicalDpiX();
    yres = mydesk->physicalDpiY();

    refreshtimer = new QTimer();
    connect(refreshtimer, &QTimer::timeout, this, &MainScene::updateSize);
}

void MainScene::loadFile(const QString &addr)
{
    document = Poppler::Document::load(addr);
    if (!document || document->isLocked()) {
        delete document;
        return;
    }

    paperwidgets->normalwidget->pages.clear();
    paperwidgets->scaledwidget->pages.clear();
    document->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    document->setRenderHint(Poppler::Document::Antialiasing, 1);
    QVBoxLayout *layout1 = new QVBoxLayout();
    QVBoxLayout *layout2 = new QVBoxLayout();
    for(int i=0; i<document->numPages();i++)
    {
        paperwidgets->normalwidget->pages.append(new QLabel());
        paperwidgets->scaledwidget->pages.append(new QLabel());
        auto image = document->page(i)->renderToImage(xres, yres, -1, -1, -1, -1, Poppler::Page::Rotate0);
        if(i==0)
        {
            width = image.width();
            height = image.height()*document->numPages();
        }
        paperwidgets->normalwidget->pages.at(i)->setPixmap(QPixmap::fromImage(image));
        paperwidgets->normalwidget->pages.at(i)->resize(image.size());
        layout1->addWidget(paperwidgets->normalwidget->pages.at(i));

        paperwidgets->scaledwidget->pages.at(i)->setPixmap(QPixmap::fromImage(image));
        paperwidgets->scaledwidget->pages.at(i)->resize(image.size());
        layout2->addWidget(paperwidgets->scaledwidget->pages.at(i));
    }
    layout1->setAlignment(Qt::AlignTop);
    layout2->setAlignment(Qt::AlignTop);
    paperwidgets->normalwidget->setLayout(layout1);
    paperwidgets->scaledwidget->setLayout(layout2);
    paperwidgets->addWidget(paperwidgets->normalwidget);
    paperwidgets->addWidget(paperwidgets->scaledwidget);
    paperwidgets->setCurrentWidget(paperwidgets->scaledwidget);
    paperproxywidget = addWidget(paperwidgets);
    paperproxywidget->setPos(0, 0);
    paperproxywidget->setZValue(-100);

    for(int i=0;i<document->page(1)->annotations().length();i++)
    {
        if(document->page(1)->annotations().at(i)->subType()==1)
        {
            Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(document->page(1)->annotations().at(i));
            if(annotation->textType()==1)
            {
                qDebug() << "once";
                PaperAnnotation::FlatTextAnnotation *anno = new PaperAnnotation::FlatTextAnnotation(annotation, width, height/document->numPages());
                this->addItem(anno);
            }
        }
    }
}

void MainScene::updateSize()
{
    qDebug() << "update once";
    refreshtimer->stop();
    paperwidgets->removeWidget(paperwidgets->scaledwidget);
    delete paperwidgets->scaledwidget;
    paperwidgets->scaledwidget = new PaperWidget();
    QVBoxLayout *layout = new QVBoxLayout();

    for(int i=0; i<document->numPages();i++)
    {
        paperwidgets->scaledwidget->pages.append(new QLabel());
        auto image = document->page(i)->renderToImage(xres*scale, yres*scale, -1, -1, -1, -1, Poppler::Page::Rotate0);
        paperwidgets->scaledwidget->pages.at(i)->setPixmap(QPixmap::fromImage(image));
        paperwidgets->scaledwidget->pages.at(i)->resize(image.size());
        layout->addWidget(paperwidgets->scaledwidget->pages.at(i));
    }

    layout->setAlignment(Qt::AlignTop);
    paperwidgets->scaledwidget->setLayout(layout);
    paperwidgets->addWidget(paperwidgets->scaledwidget);
    paperwidgets->setCurrentWidget(paperwidgets->scaledwidget);
    paperwidgets->setFixedWidth(width*scale);
    paperwidgets->setFixedHeight(height*scale);
    setSceneRect(0, 0, width, height);

    paperproxywidget->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    paperproxywidget->setFlag(QGraphicsItem::ItemIsMovable, false);
    paperproxywidget->setFlag(QGraphicsItem::ItemIsSelectable, false);
    paperproxywidget->setPos(0, 0);
    paperproxywidget->setZValue(-100);
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

QGraphicsView *MainFrame::view() const
{
    return graphicsview;
}

void MainFrame::setMatrix(qreal deltascale, qreal deltarotate)
{


}

SideScene::SideScene()
{

}

void GraphicsView::wheelEvent(QWheelEvent *ev)
{
    if(ev->modifiers() & Qt::ControlModifier)
    {
        if(ev->delta()>0)
        {            
            scale(1.2, 1.2);
            scalefactor*= 1.2;
        }
        else
        {
            scale(0.8, 0.8);
            scalefactor/= 1.2;
        }

        MainScene *mainscene = ((MainScene *)scene());
        mainscene->paperwidgets->setFixedWidth(mainscene->width);
        mainscene->paperwidgets->setFixedHeight(mainscene->height);
        mainscene->paperproxywidget->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
        mainscene->paperwidgets->setCurrentWidget(
                mainscene->paperwidgets->normalwidget);
        mainscene->paperproxywidget->setZValue(-100);
        mainscene->scale = scalefactor;
        mainscene->yaxis = mapToScene(ev->pos()).y();

        if(mainscene->refreshtimer->isActive())
        {
            mainscene->refreshtimer->stop();
        }
        mainscene->refreshtimer->start(200);

        ev->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(ev);

    }
}

PaperWidget::PaperWidget()
{

}

PaperWidgets::PaperWidgets()
{
    normalwidget = new PaperWidget();
    scaledwidget = new PaperWidget();
}
