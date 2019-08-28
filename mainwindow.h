#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pageviewwidget.h>
#include <annotoolbar.h>
#include <QMessageBox>
#include <filewindow.h>

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
    PageViewWidget *pageviewwidget = nullptr;
    void openNewPDF();
    FileWindow *m_FileWindow = nullptr;
    void return_to_front();

private:
    Ui::MainWindow *ui;

protected:
    void keyPressEvent(QKeyEvent * event);
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
