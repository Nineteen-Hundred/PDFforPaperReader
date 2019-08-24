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
#include <paperannotation.h>
#include <QAction>
#include <QCursor>
#include <autodocument.h>
#include <QThread>
#include <unistd.h>
#include <QMessageBox>
#include <QFileDialog>

class MainFrame;

class PaperItem : public QGraphicsItem
{
public:
    PaperItem(int index, long imagess) {this->index=index; this->images = imagess; setZValue(-10);}
    QImage *image;
    long images;
    int index;
    long size = sizeof(QImage *);
    virtual QRectF boundingRect()const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

class MainScene : public QGraphicsScene
{
    Q_OBJECT
    friend class MainFrame;
    friend class GraphicsView;
public:
    MainScene();
    void loadFile(const QString &addr);
    void refreshView(qreal scale);
    void drawAnnotations();
    QVector<PaperAnnotation::Annotation *> annotations;
    void updateSize();
    int xres;
    int yres;
    double width;
    double height;
    double scale = 1;
    bool isDrawing = false;
    bool isPressing = false;
    int shape = 0;
    int currentpage = 0;

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void changeIsDrawing(const QString &text, bool isDrawing);
    void setCurrentShape(int i);
    QGraphicsLineItem *tmplineitem;
    QGraphicsEllipseItem *tmpellipseitem;
    QGraphicsRectItem *tmprectitem;
    QGraphicsTextItem *tmptextitem;
    QPointF startPoint, endPoint;
    void newFlatText(const QString &text, QFont font, QColor color);
    void newPopupText(const QString &text, QFont font, QColor color);
    QVector<int> indexes;
    void updateItem(int index);
    void updateScene();
    AutoDocument *document;
    void removeCertainItem();
    void savePDF();
    void savePDFas();
    QString filename = "";
    void regenerate_annotations();

private:
    //Poppler::Document *document;
    //PaperWidgets *paperwidgets;
    QList<PaperItem *> pages;
    QGraphicsProxyWidget *paperproxywidget;

signals:
    void is_drawing(bool flag);
    void resizeCompleted();
    void save_completed();
    void save_incompleted();
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
    GraphicsView(MainFrame *frame);
    void setScene(MainScene *scene);
    QTimer *refreshtimer;
    void updateSize();
    MainScene *mainscene;
    QPointF currentPoint, currentScenePoint, centerPoint, oldCenterPoint, lengthPoint;
    void timeStopped();

protected:
    virtual void wheelEvent(QWheelEvent *ev) override; 

private:
    MainFrame *frame;
    qreal scalefactor = 1, oldscalefactor = 1;
    QPointF point;


signals:
    void sizeChanged(qreal deltascale, qreal rotate);
    void timerStarting();
};

class MainFrame : public QFrame
{
public:
    MainFrame();
    GraphicsView *view() const;
    void zoom();

private:
    double scale = 1;
    GraphicsView *graphicsview;
};

#endif // PAPERVIEWS_H
