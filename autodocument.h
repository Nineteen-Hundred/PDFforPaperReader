#ifndef AUTODOCUMENT_H
#define AUTODOCUMENT_H

#include <poppler-qt5.h>
#include <QQueue>
#include <QObject>

class AutoDocument : public QObject
{
public:
    AutoDocument();
    Poppler::Document *document;
    QVector <QImage *> images;
    double scale = 1;

    QQueue<int> indexes;

    void updateImages();

signals:
    void imageCompleted(int index);
};

#endif // AUTODOCUMENT_H
