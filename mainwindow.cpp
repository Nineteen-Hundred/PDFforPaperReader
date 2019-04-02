#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    removeToolBar(ui->mainToolBar);

    mainscene = new MainScene();
//    scenethread = new QThread;
//    mainscene->moveToThread(scenethread);
//    scenethread->start();

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

    QString filename = "/home/pysong/下载/A Very Brief Introduction to Machine Learning With Applications to Communication Systems.pdf";
    //QString filename = "/home/pysong/下载/finalpaperfile.pdf";
    mainscene->loadFile(filename);

    connect(mainframe->view()->refreshtimer, &QTimer::timeout, mainframe->view(), &GraphicsView::updateSize);
    connect(mainframe->view()->refreshtimer, &QTimer::timeout, mainframe->view()->mainscene, &MainScene::updateSize);
}

MainWindow::~MainWindow()
{
    delete ui;
}
