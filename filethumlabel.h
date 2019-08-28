#ifndef FILETHUMLABEL_H
#define FILETHUMLABEL_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QDir>
#include "fileconfigure.h"
#include "fileimagebuffer.h"
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

class FileThumLabel : public QWidget
{
    Q_OBJECT
public:
    explicit FileThumLabel(QString filepath, int height, int width, QWidget *parent = nullptr);
    QString m_filepath;
    QString m_filename;
    QString m_image_cache_path;
    QLabel *m_image;
    QLabel *m_title;
    int isPDF;
private:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
signals:
    void click_right(FileThumLabel *, QPoint);
    void click_left(FileThumLabel *);
public slots:
};

#endif // FILETHUMLABEL_H
