#ifndef AUTODOCUMENT_H
#define AUTODOCUMENT_H

#include <poppler-qt5.h>
#include <QQueue>
#include <QTimer>
#include <QDesktopWidget>
#include <QApplication>
#include <QTime>
#include <QDebug>

class AutoDocument : public QObject
{
    Q_OBJECT
public:
    AutoDocument();
    Poppler::Document *document;
    QImage *images[1000];
    double scale = 1;

    QQueue<int> indexes;

    void updateImages();
    QTimer *timer;
    void startTimer();

signals:
    void imageCompleted();
};

#endif // AUTODOCUMENT_H
