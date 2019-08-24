#ifndef PAGEVIEWWIDGET_H
#define PAGEVIEWWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <poppler-qt5.h>
#include <paperviews.h>
#include <QtMath>
#include <QMatrix>
#include <QThread>

class PageViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PageViewWidget(QString filename, QWidget *parent = nullptr);
    QSplitter *leftSpliter, *centerSpliter, *rightSpliter, *totalSpliter;
    MainScene *mainscene;
    SideScene *leftscene, *rightscene;
    QThread *scenethread;
    void setPaperTitle();
    void loadNewPDF();
    MainFrame *mainframe;
    QString filename;
    bool modified_status = false;

signals:
    void file_loaded(QString filename);

public slots:
};

#endif // PAGEVIEWWIDGET_H
