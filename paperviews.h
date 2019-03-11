#ifndef PAPERVIEWS_H
#define PAPERVIEWS_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <poppler-qt5.h>
#include <QPainter>
#include <QFrame>
#include <QLayout>
#include <QGraphicsView>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QPicture>
#include <QWheelEvent>
#include <QtMath>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QStackedWidget>

class MainFrame;

class PaperWidget : public QWidget
{
    friend class MainScene;
    friend class GraphicsView;
public:
    PaperWidget();

private:
    QList<QLabel *> pages;
};

class PaperWidgets : public QStackedWidget
{
    friend class MainScene;
    friend class MainFrame;
    friend class GraphicsView;
public:
    PaperWidgets();

private:
    PaperWidget *normalwidget, *scaledwidget;
};

class MainScene : public QGraphicsScene
{
    friend class MainFrame;
    friend class GraphicsView;
public:
    MainScene();
    void loadFile(const QString &addr);
    void refreshView(qreal scale);
    void drawAnnotations();
    QList<QGraphicsItem *> annotations;
    QTimer *refreshtimer;
    void updateSize();
    int xres;
    int yres;
    double width;
    double height;
    double scale = 1;

private:
    Poppler::Document *document;
    PaperWidgets *paperwidgets;
    QGraphicsProxyWidget *paperproxywidget;
};

class SideScene : public QGraphicsScene
{
public:
    SideScene();
};

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(MainFrame *frame) : QGraphicsView(), frame(frame) { }

protected:
    virtual void wheelEvent(QWheelEvent *ev) override;

private:
    MainFrame *frame;

signals:
    void sizeChanged(qreal deltascale, qreal rotate);
};

class MainFrame : public QFrame
{
public:
    MainFrame();
    QGraphicsView *view() const;
    void zoom();
    void setMatrix(qreal deltascale, qreal deltarotate);

private:
    double scale = 1;
    GraphicsView *graphicsview;
};

#endif // PAPERVIEWS_H
