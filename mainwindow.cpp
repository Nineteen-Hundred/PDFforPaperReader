#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    removeToolBar(ui->mainToolBar);

    mainview = new MainScene();

    leftview = new SideScene();
    rightview = new SideScene();
    leftSpliter = new QSplitter();
    rightSpliter = new QSplitter();
    centerSpliter = new QSplitter();
    totalSpliter = new QSplitter(Qt::Horizontal);
    leftSpliter->setFixedWidth(200);
    rightSpliter->setFixedWidth(200);
    MainFrame *mainframe = new MainFrame();
    mainframe->view()->setScene(mainview);
    centerSpliter->addWidget(mainframe);
    totalSpliter->addWidget(leftSpliter);
    totalSpliter->addWidget(centerSpliter);
    totalSpliter->addWidget(rightSpliter);
    this->setCentralWidget(totalSpliter);

    QString filename = "/home/pysong/下载/A Very Brief Introduction to Machine Learning With Applications to Communication Systems.pdf";
    mainview->loadFile(filename);
}

MainWindow::~MainWindow()
{
    delete ui;
}
