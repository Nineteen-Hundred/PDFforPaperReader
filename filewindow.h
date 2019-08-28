#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QGridLayout>
#include <QScrollArea>
#include <QList>
#include "filelist.h"

class FileWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FileWindow(QWidget *parent = nullptr);
    int mresize(QSize size);
    //QMenu *m_contextMenu;
    //QWidget *FileListWidget;
    QScrollArea *m_scorll = nullptr;
    FileList *m_file_list = nullptr;
signals:
    void changeSizeSignal(QSize size);
public slots:
};

#endif // FILEWINDOW_H
