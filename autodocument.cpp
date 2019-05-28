#include "autodocument.h"

AutoDocument::AutoDocument()
{
    timer = new QTimer();
}

void AutoDocument::updateImages()
{
    qDebug() << "success";

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
        //delete tmpimage;
        images[tmpindex] = image;
        qDebug() << "success" << images[tmpindex]->height() << indexes.isEmpty();
    }

    timer->start(500);
}

void AutoDocument::startTimer()
{
    timer->start(500);
    connect(timer, &QTimer::timeout, this, &AutoDocument::updateImages);
}
