#include "autodocument.h"

AutoDocument::AutoDocument()
{
    timer = new QTimer();
}

void AutoDocument::updateImages()
{
    if(timer->isActive())
        timer->stop();
    QDesktopWidget *mydesk = QApplication::desktop();
    double xres = mydesk->physicalDpiX()*scale;
    double yres = mydesk->physicalDpiY()*scale;

    while(!(indexes.isEmpty()))
    {
        int tmpindex = indexes.dequeue();
        QImage *image = new QImage(document->page(tmpindex)->renderToImage(xres, yres, -1, -1, -1, -1));
        QImage *tmpimage = images[tmpindex];
        delete tmpimage;
        images[tmpindex] = image;
    }

    emit imageCompleted();
}

void AutoDocument::startTimer()
{
    timer->start(500);
    connect(this->timer, &QTimer::timeout, this, &AutoDocument::updateImages);
}
