#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    removeToolBar(ui->mainToolBar);

    loadPageView("/home/pysong/下载/An efficient implementation of lattice staggered multicarier faster than nyquist signaling.pdf");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPageView(QString filename)
{
    toolbar = new AnnoToolbar;
    addToolBar(toolbar);

    PageViewWidget *pageviewwidget = new PageViewWidget(filename);
    connect(toolbar, &AnnoToolbar::isDrawing, pageviewwidget->mainscene, &MainScene::changeIsDrawing);
    connect(toolbar->saving_action, &QAction::triggered, pageviewwidget->mainscene, &MainScene::savePDF);
    this->setCentralWidget(pageviewwidget);
}
