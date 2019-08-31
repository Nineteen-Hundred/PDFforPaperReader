#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    removeToolBar(ui->mainToolBar);

    m_FileWindow = new FileWindow(this);
//    this->setCentralWidget(m_FileWindow);

    this->setWindowTitle(tr("Bamboo多功能PDF阅读器"));

    toolbar = new AnnoToolbar;
    addToolBar(toolbar);
    toolbar->initialStatus();

    connect(this->m_FileWindow->m_file_list, &FileList::sendFileName, this, &MainWindow::loadPageView);
    connect(toolbar->open_action, &QAction::triggered, this, &MainWindow::openNewPDF);
    connect(toolbar->files_action, &QAction::triggered, this, &MainWindow::return_to_front);

    this->showMaximized();

    //    loadPageView("/home/pysong/下载/test.pdf");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPageView(QString filename)
{
    m_FileWindow->hide();
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

    if(!(pageviewwidget->loadNewPDF()))
    {
        QMessageBox::about(this, tr("提示"), tr("打开PDF时出现错误，可能PDF文件已加密或超出大小！"));
        return_to_front();
        return;
    }
    toolbar->pdfStatus();
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

void MainWindow::return_to_front()
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
                return;
            }
        }

        //removeToolBar(toolbar);
        pageviewwidget->close();
        delete pageviewwidget;
        pageviewwidget = nullptr;
    }

    m_FileWindow->show();
    this->setWindowTitle(tr("Bamboo多功能PDF阅读器"));
    toolbar->initialStatus();
    //this->setCentralWidget(m_FileWindow);
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(this->m_FileWindow != nullptr) this->m_FileWindow->mresize(this->size());
}
