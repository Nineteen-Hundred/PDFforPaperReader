#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    removeToolBar(ui->mainToolBar);
    toolbar = new AnnoToolbar;
    addToolBar(toolbar);

    mainscene = new MainScene();

    leftscene = new SideScene();
    rightscene = new SideScene();
    leftSpliter = new QSplitter();
    rightSpliter = new QSplitter();
    centerSpliter = new QSplitter();
    totalSpliter = new QSplitter(Qt::Horizontal);
    leftSpliter->setFixedWidth(200);
    rightSpliter->setFixedWidth(200);
    MainFrame *mainframe = new MainFrame();
    mainframe->view()->setScene(mainscene);
    centerSpliter->addWidget(mainframe);
    totalSpliter->addWidget(leftSpliter);
    totalSpliter->addWidget(centerSpliter);
    totalSpliter->addWidget(rightSpliter);
    this->setCentralWidget(totalSpliter);

    QString filename = "/home/pysong/下载/新形势下党校教科研与党建工作协调机制研究_钱昕黎.pdf";
    //QString filename = "/home/pysong/下载/finalpaperfile.pdf";
    mainscene->loadFile(filename);


    connect(toolbar, &AnnoToolbar::isDrawing, mainscene, &MainScene::changeIsDrawing);

    connect(mainframe->view()->refreshtimer, &QTimer::timeout, mainframe->view(), &GraphicsView::updateSize);
}

MainWindow::~MainWindow()
{
    delete ui;
}
