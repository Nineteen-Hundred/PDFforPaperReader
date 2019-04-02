#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <poppler-qt5.h>
#include <paperviews.h>
#include <QtMath>
#include <QMatrix>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSplitter *leftSpliter, *centerSpliter, *rightSpliter, *totalSpliter;
    MainScene *mainscene;
    SideScene *leftscene, *rightscene;
    QThread *scenethread;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
