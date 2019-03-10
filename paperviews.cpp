#include <paperviews.h>

MainScene::MainScene()
{

}

void MainScene::loadFile(const QString &addr)
{
    document = Poppler::Document::load(addr);
    if (!document || document->isLocked()) {
        delete document;
        return;
    }

    pages.clear();
    document->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    document->setRenderHint(Poppler::Document::Antialiasing, 1);
    for(int i=0; i<document->numPages();i++)
    {
        qDebug() << "once time";
        pages.append(new PaperPage(document->page(i)));
        pages.at(i)->setPos(QPointF(0.0, 1002.0*i));
        this->addItem(pages.at(i));
    }
}

PaperPage::PaperPage(Poppler::Page *page) : page(page)
{

}

void PaperPage::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    QDesktopWidget *mydesk = QApplication::desktop();
    for(int i=0;i<page->annotations().length();i++)
    {
        page->annotations().at(i)->setFlags(Poppler::Annotation::Hidden);
    }
    auto image = page->renderToImage(mydesk->physicalDpiX(), mydesk->physicalDpiY(), -1, -1, -1, -1, Poppler::Page::Rotate0);
    image.save("/home/pysong/下载/test.png", "PNG", 100);
    painter->drawImage(0, 0, image);
}

QRectF PaperPage::boundingRect() const
{
    return QRectF(0, 0, 782, 1001);
}

MainFrame::MainFrame()
{
    graphicsview = new GraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(graphicsview);
    setLayout(layout);

    this->setMatrix(1, 0);

    connect(graphicsview, &GraphicsView::sizeChanged, this, &MainFrame::setMatrix);
}

QGraphicsView *MainFrame::view() const
{
    return graphicsview;
}

void MainFrame::zoomin(int level)
{
    qDebug() << "zoomin";
}

void MainFrame::zoomout(int level)
{
    qDebug() << "zoomout";
}

void MainFrame::setMatrix(qreal deltascale, qreal deltarotate)
{
    scale*= deltascale;

    QMatrix matrix;
    matrix.scale(scale, scale);
    matrix.rotate(deltarotate);

    graphicsview->setMatrix(matrix);
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
            emit sizeChanged(1.2, 0);
        }
        else
        {
            emit sizeChanged(0.8, 0);
        }
        ev->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(ev);
    }
}
