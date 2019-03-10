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

class MainFrame;

class PaperPage : public QGraphicsItem
{
public:
    PaperPage(Poppler::Page *page);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    QRectF boundingRect() const override;

private:
    Poppler::Page *page;
};

class MainScene : public QGraphicsScene
{
public:
    MainScene();
    void loadFile(const QString &addr);
    void refreshView(qreal scale);
    void drawAnnotations();
    QList<PaperPage *> pages;
    QList<QGraphicsItem *> annotations;
    QTimer *refreshtimer;

private:
    Poppler::Document *document;
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
    void zoomin(int level=1);
    void zoomout(int level=1);
    void setMatrix(qreal deltascale, qreal deltarotate);

private:
    double scale = 1;
    GraphicsView *graphicsview;
};

#endif // PAPERVIEWS_H
