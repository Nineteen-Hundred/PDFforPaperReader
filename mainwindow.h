#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pageviewwidget.h>
#include <annotoolbar.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadPageView(QString filename);
    AnnoToolbar *toolbar;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
