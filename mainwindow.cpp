#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    removeToolBar(ui->mainToolBar);

    loadPageView("/home/pysong/下载/test.pdf");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPageView(QString filename)
{
    if(pageviewwidget!=nullptr)
    {
        if(pageviewwidget->modified_status)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("提示"), tr("您的文档尚未保存，是否保存？"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);

            if(reply==QMessageBox::Yes)  // 文件保存，继续打开
            {
                pageviewwidget->mainscene->savePDF();
                this->centralWidget()->close();
            }
            else if(reply==QMessageBox::No)  // 文件未保存，继续打开
            {
                this->centralWidget()->close();
            }
            else  // 操作取消
            {
                return;
            }
        }
        else
        {
            this->centralWidget()->close();
        }
        delete pageviewwidget;
    }
    else
    {
        toolbar = new AnnoToolbar;
        addToolBar(toolbar);

        connect(toolbar->open_action, &QAction::triggered, this, &MainWindow::openNewPDF);
    }

    pageviewwidget = new PageViewWidget(filename);
    this->setCentralWidget(pageviewwidget);

    connect(pageviewwidget->mainscene, &MainScene::draw_completed, this->toolbar, &AnnoToolbar::resetStatus);
    connect(toolbar, &AnnoToolbar::isDrawing, pageviewwidget->mainscene, &MainScene::changeIsDrawing);
    connect(toolbar->saving_action, &QAction::triggered, pageviewwidget->mainscene, &MainScene::savePDF);
    connect(toolbar->saving_as_action, &QAction::triggered, pageviewwidget->mainscene, &MainScene::savePDFas);
    connect(toolbar->print_action, &QAction::triggered, pageviewwidget->mainscene, &MainScene::printPDF);
    connect(pageviewwidget, &PageViewWidget::file_loaded, this, &MainWindow::setWindowTitle);
    connect(pageviewwidget->mainscene, &MainScene::status_changed, this, &MainWindow::setModiefiedTitle);
    connect(toolbar->preview_action, &QAction::triggered, pageviewwidget->mainscene, &MainScene::changePreview);

    pageviewwidget->loadNewPDF();
}

void MainWindow::setModiefiedTitle(bool status)
{
    pageviewwidget->modified_status = status;
    pageviewwidget->mainscene->modified_status = status;
    if(status)
    {
        this->toolbar->saving_action->setEnabled(true);
        this->setWindowTitle(this->pageviewwidget->filename + QString("*"));
    }
    else
    {
        this->toolbar->saving_action->setEnabled(false);
        this->pageviewwidget->filename = this->pageviewwidget->mainscene->filename;
        this->setWindowTitle(this->pageviewwidget->filename);
    }
}

void MainWindow::openNewPDF()
{
    QString open_filename = QFileDialog::getOpenFileName(this, tr("打开文件"), "", "PDF Files(*.pdf)", nullptr);
    if(open_filename!="")
    {
        loadPageView(open_filename);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(Qt::ControlModifier && event->key()==Qt::Key_S)
    {
        if(pageviewwidget!=nullptr)
        {
            if(pageviewwidget->modified_status)
            {
                pageviewwidget->mainscene->savePDF();
            }
        }
    }
    else if(Qt::ControlModifier && event->key()==Qt::Key_P)
    {
        if(pageviewwidget!=nullptr)
        {
            pageviewwidget->mainscene->printPDF();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(pageviewwidget!=nullptr)
    {
        if(pageviewwidget->modified_status)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("提示"), tr("您的文档尚未保存，是否保存？"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);

            if(reply==QMessageBox::Yes)  // 文件保存，继续打开
            {
                pageviewwidget->mainscene->savePDF();
            }
            else if(reply==QMessageBox::No)  // 文件未保存，继续打开
            {
            }
            else  // 操作取消
            {
                event->ignore();
            }
        }
        else
        {
            MainWindow::closeEvent(event);
        }
    }
}
