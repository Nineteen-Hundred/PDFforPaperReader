#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pageviewwidget.h>
#include <annotoolbar.h>
#include <QMessageBox>

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
    void setModiefiedTitle(bool status);
    PageViewWidget *pageviewwidget;
    void openNewPDF();

private:
    Ui::MainWindow *ui;

protected:
    void keyPressEvent(QKeyEvent * event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
