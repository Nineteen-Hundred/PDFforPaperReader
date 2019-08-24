#include "aboutwindow.h"

AboutWindow::AboutWindow()
{
    QLabel *logo = new QLabel();
    logo->setPixmap(QPixmap::fromImage(QImage(":/image/logo")));
    QLabel *name = new QLabel(tr("Bamboo多功能PDF阅读器"));
    name->setAlignment(Qt::AlignCenter);
    name->setFont(QFont("Microsoft Yahei", 14, QFont::DemiBold));
    QLabel *version = new QLabel(tr("版本： 1.1.0"));
    version->setFont(QFont("Microsoft Yahei", 10, QFont::Normal));
    QPalette version_pa;
    version->setAlignment(Qt::AlignCenter);
    version_pa.setColor(QPalette::WindowText, QColor(60, 60, 60));
    version->setPalette(version_pa);
    QLabel *address = new QLabel(tr("http://www.xdusoft.com"));
    address->setFont(QFont("Microsoft Yahei", 10, QFont::Normal));
    QPalette addr_pa;
    addr_pa.setColor(QPalette::WindowText, Qt::blue);
    address->setPalette(addr_pa);
    address->setAlignment(Qt::AlignCenter);
    QLabel *contri_label = new QLabel(tr("作者"));
    contri_label->setFont(QFont("Microsoft Yahei", 12, QFont::DemiBold));
    contri_label->setAlignment(Qt::AlignCenter);
    QLabel *contri_1 = new QLabel(tr("    Square3：软件设计、笔记系统"));
    contri_1->setFont(QFont("Microsoft Yahei", 10, QFont::Normal));
    contri_1->setAlignment(Qt::AlignCenter);
    QLabel *contri_2 = new QLabel(tr("    Ink19：文件收藏系统"));
    contri_2->setFont(QFont("Microsoft Yahei", 10, QFont::Normal));
    contri_2->setAlignment(Qt::AlignCenter);
    QPushButton *close_btn = new QPushButton(tr("确定"));
    close_btn->setFixedSize(80, 30);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(logo);
    layout->addSpacing(10);
    layout->addWidget(name);
    layout->addWidget(version);
    layout->addWidget(address);
    layout->addSpacing(14);
    layout->addWidget(contri_label);
    layout->addWidget(contri_1);
    layout->addWidget(contri_2);
    layout->addSpacing(12);
    layout->addWidget(close_btn, 0, Qt::AlignHCenter);
    layout->addSpacing(6);
    layout->setAlignment(Qt::AlignCenter);

    connect(close_btn, &QPushButton::clicked, this, &QDialog::close);

    this->setFixedWidth(410);
    this->setWindowTitle(tr("关于"));
    this->setLayout(layout);
}
