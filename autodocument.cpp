#include "autodocument.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QTime>

AutoDocument::AutoDocument() : QObject()
{

}

void AutoDocument::updateImages()
{
    QDesktopWidget *mydesk = QApplication::desktop();
    double xres = mydesk->physicalDpiX();
    double yres = mydesk->physicalDpiY();
    QTime t;
    t.start();

    while(images.isEmpty())
    {
        int tmpindex = indexes.dequeue();
        QImage *image = new QImage(document->page(tmpindex)->renderToImage(xres, yres, -1, -1, -1, -1));
        images.replace(tmpindex, image);
        emit imageCompleted(tmpindex);
    }
}
