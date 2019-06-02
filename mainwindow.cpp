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
    setWindowTitle(tr("Bamboo PDF阅读器"));

    mainscene = new MainScene();

    leftscene = new SideScene();
    rightscene = new SideScene();
    leftSpliter = new QSplitter();
    rightSpliter = new QSplitter();
    centerSpliter = new QSplitter();
    totalSpliter = new QSplitter(Qt::Horizontal);
    leftSpliter->setFixedWidth(20);
    rightSpliter->setFixedWidth(20);
    MainFrame *mainframe = new MainFrame();
    mainframe->view()->setScene(mainscene);
    centerSpliter->addWidget(mainframe);
    totalSpliter->addWidget(leftSpliter);
    totalSpliter->addWidget(centerSpliter);
    totalSpliter->addWidget(rightSpliter);
    this->setCentralWidget(totalSpliter);

    QString filename = "/home/pysong/下载/An efficient implementation of lattice staggered multicarier faster than nyquist signaling.pdf";
    mainscene->loadFile(filename);


    connect(toolbar, &AnnoToolbar::isDrawing, mainscene, &MainScene::changeIsDrawing);

    connect(mainframe->view()->refreshtimer, &QTimer::timeout, mainframe->view(), &GraphicsView::updateSize);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPaperTitle()
{

}
