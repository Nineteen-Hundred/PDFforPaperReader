#include <annotationdialog.h>

#define RED 0
#define YELLOW 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4
#define PURPLE 5
#define WHITE 6
#define BLACK 7

AnnotationDialog::FlatTextDialog::FlatTextDialog(const QString &text, const QFont &font, const QColor &color)
{
    this->font = QFont(font);
    edit = new QTextEdit();
    edit->setFixedSize(400,160);
    edit->setText(text);
    QHBoxLayout *attrLayout = new QHBoxLayout();

    QLabel *sizelabel = new QLabel(tr("Size:"));
    sizelabel->setFixedWidth(30);
    attrLayout->addWidget(sizelabel);

    box = new QComboBox();
    box->setFixedWidth(50);

    QStringList str;
    for(int i=0; i<10; i++)
    {
        str << QString::number((i+1)*2);
    }
    box->addItems(str);

    for(int i=0; i<10; i++)
    {
        if(font.pointSize()<=box->itemText(i).toInt())
        {
            box->setCurrentIndex(i);
            break;
        }
    }
    attrLayout->addWidget(box);

    attrLayout->addSpacing(20);

    QLabel *colorlabel = new QLabel(tr("Color:"));
    colorlabel->setFixedWidth(40);
    attrLayout->addWidget(colorlabel);

    blocks = new ColorBlocks(color);
    attrLayout->addWidget(blocks);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    submitBtn = new QPushButton(tr("Ensure"));
    deleteBtn = new QPushButton(tr("Delete"));
    cancelBtn = new QPushButton(tr("Cancle"));
    submitBtn->setFixedWidth(80);
    deleteBtn->setFixedWidth(80);
    cancelBtn->setFixedWidth(80);
    buttonLayout->addWidget(submitBtn);
    buttonLayout->addWidget(deleteBtn);
    buttonLayout->addWidget(cancelBtn);

    connect(cancelBtn, &QPushButton::clicked, this, &FlatTextDialog::close);
    connect(submitBtn, &QPushButton::clicked, this, &FlatTextDialog::emitConfig);
    connect(deleteBtn, &QPushButton::clicked, this, &FlatTextDialog::emitDelete);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(edit);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(attrLayout);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    setWindowTitle(tr("Text Annotation"));
}

void AnnotationDialog::FlatTextDialog::emitConfig()
{
    font.setPointSize(box->currentText().toInt());
    emit configUpdated(this->edit->toPlainText(), font, blocks->getColor());
    close();
}

void AnnotationDialog::FlatTextDialog::emitDelete()
{
    emit itemDeleted();
    close();
}

AnnotationDialog::Dialog::Dialog()
{

}

void AnnotationDialog::ColorBlocks::changeColors(int index)
{
    if(!colorblocks.at(index)->isSelected())
    {
        for(int i=0; i<8; i++)
        {
            colorblocks.at(i)->setIsSelected(false);
        }
        colorblocks.at(index)->changeIsSelected();
    }
}

QColor AnnotationDialog::ColorBlocks::getColor()
{
    for(int i=0; i<8;i++)
    {
        if(colorblocks.at(i)->isSelected())
        {
            return colorblocks.at(i)->getColor();
        }
    }
}

AnnotationDialog::ColorBlock::ColorBlock(int idx)
{
    setFixedSize(20, 20);

    this->index = idx;

    setStyle();
}

bool AnnotationDialog::ColorBlock::isSelected() const
{
    return isselected;
}

void AnnotationDialog::ColorBlock::setIsSelected(bool value)
{
    if(value)
    {
        this->isselected = true;
    }
    else
    {
        this->isselected = false;
    }
    setStyle();
}

void AnnotationDialog::ColorBlock::changeIsSelected()
{
    setIsSelected(!isSelected());
}

void AnnotationDialog::ColorBlock::mouseReleaseEvent(QMouseEvent *ev)
{
    emit clicked(index);
}

QColor AnnotationDialog::ColorBlock::getColor()
{
    switch(index)
    {
    case RED:
        return QColor(Qt::red);
    case YELLOW:
        return QColor(Qt::yellow);
    case GREEN:
        return QColor(Qt::green);
    case BLUE:
        return QColor(Qt::blue);
    case ORANGE:
        return QColor(238, 159, 0);
    case PURPLE:
        return QColor(77, 00, 77);
    case WHITE:
        return QColor(Qt::white);
    case BLACK:
        return QColor(Qt::black);
    default:
        return QColor(Qt::red);
    }
}

void AnnotationDialog::ColorBlock::setStyle()
{
    if(!isselected)
    {
        setFixedSize(20, 20);
        switch(index)
        {
        case RED:
            setStyleSheet("background-color:red;border:1px solid gray;");
            break;
        case YELLOW:
            setStyleSheet("background-color:yellow;border:1px solid gray;");
            break;
        case GREEN:
            setStyleSheet("background-color:green;border:1px solid gray;");
            break;
        case BLUE:
            setStyleSheet("background-color:blue;border:1px solid gray;");
            break;
        case ORANGE:
            setStyleSheet("background-color:orange;border:1px solid gray;");
            break;
        case PURPLE:
            setStyleSheet("background-color:purple;border:1px solid gray;");
            break;
        case WHITE:
            setStyleSheet("background-color:white;border:1px solid gray;");
            break;
        case BLACK:
            setStyleSheet("background-color:black;border:1px solid gray;");
            break;
        }
    }
    else
    {
        setFixedSize(24, 24);
        switch(index)
        {
        case RED:
            setStyleSheet("background-color:red;border:3px solid rgb(120,123,144);");
            break;
        case YELLOW:
            setStyleSheet("background-color:yellow;border:3px solid rgb(120,123,144);");
            break;
        case GREEN:
            setStyleSheet("background-color:green;border:3px solid rgb(120,123,144);");
            break;
        case BLUE:
            setStyleSheet("background-color:blue;border:3px solid rgb(120,123,144);");
            break;
        case ORANGE:
            setStyleSheet("background-color:orange;border:3px solid rgb(120,123,144);");
            break;
        case PURPLE:
            setStyleSheet("background-color:purple;border:3px solid rgb(120,123,144);");
            break;
        case WHITE:
            setStyleSheet("background-color:white;border:3px solid rgb(120,123,144);");
            break;
        case BLACK:
            setStyleSheet("background-color:black;border:3px solid rgb(120,123,144);");
            break;
        }
    }
}

AnnotationDialog::GeomDialog::GeomDialog(const QColor &color, int linewidth)
{
    QHBoxLayout *attrLayout = new QHBoxLayout;
//    for(int i=0; i<8; i++)
//    {
//        colorblocks.append(new ColorBlock(i));
//        attrLayout->addWidget(colorblocks.at(i));
//        colorblocks.at(i)->setIsSelected(false);
//        connect(colorblocks.at(i), &ColorBlock::clicked, this, &FlatTextDialog::changeColors);
//    }
//    colorblocks.at(0)->setIsSelected(true);

    QLabel *widthLabel = new QLabel(tr("Width:"));
    box = new QComboBox();
    QStringList str;
    for(int i=0; i<8; i++)
    {
        str << QString::number(i+1);
    }
    box->addItems(str);
    box->setCurrentText(QString::number(linewidth>7?7:linewidth));
    QHBoxLayout *widthLayout = new QHBoxLayout;
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(box);

    QLabel *colorlabel = new QLabel(tr("Color:"));
    colorlabel->setFixedWidth(40);
    attrLayout->addWidget(colorlabel);

    blocks = new ColorBlocks(color);
    attrLayout->addWidget(blocks);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    submitBtn = new QPushButton(tr("Ensure"));
    deleteBtn = new QPushButton(tr("Delete"));
    cancelBtn = new QPushButton(tr("Cancle"));
    submitBtn->setFixedWidth(80);
    deleteBtn->setFixedWidth(80);
    cancelBtn->setFixedWidth(80);
    buttonLayout->addWidget(submitBtn);
    buttonLayout->addWidget(deleteBtn);
    buttonLayout->addWidget(cancelBtn);

    connect(cancelBtn, &QPushButton::clicked, this, &GeomDialog::close);
    connect(submitBtn, &QPushButton::clicked, this, &GeomDialog::emitConfig);
    connect(deleteBtn, &QPushButton::clicked, this, &GeomDialog::emitDelete);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(widthLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(attrLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void AnnotationDialog::GeomDialog::emitConfig()
{
    emit configUpdated(blocks->getColor(), box->currentText().toInt());
    close();
}

void AnnotationDialog::GeomDialog::emitDelete()
{
    emit itemDeleted();
}

AnnotationDialog::ColorBlocks::ColorBlocks(QColor color)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    for(int i=0; i<8; i++)
    {
        colorblocks.append(new ColorBlock(i));
        mainLayout->addWidget(colorblocks.at(i));
        colorblocks.at(i)->setIsSelected(false);
        connect(colorblocks.at(i), &ColorBlock::clicked, this, &ColorBlocks::changeColors);
    }

    setLayout(mainLayout);

    for(int i=0; i<8; i++)
    {
        if(color==Qt::red)
        {
            colorblocks.at(0)->setIsSelected(true);
        }
        else if(color==Qt::yellow)
        {
            colorblocks.at(1)->setIsSelected(true);
        }
        else if(color==Qt::green)
        {
            colorblocks.at(2)->setIsSelected(true);
        }
        else if(color==Qt::blue)
        {
            colorblocks.at(3)->setIsSelected(true);
        }
        else if(color==QColor(0xFF,0x7F, 0x00))
        {
            colorblocks.at(4)->setIsSelected(true);
        }
        else if(color==Qt::magenta)
        {
            colorblocks.at(5)->setIsSelected(true);
        }
        else if(color==Qt::white)
        {
            colorblocks.at(6)->setIsSelected(true);
        }
        else if(color==Qt::black)
        {
            colorblocks.at(7)->setIsSelected(true);
        }
        else
        {
            colorblocks.at(0)->setIsSelected(true);
        }
    }
}

void AnnotationDialog::LineDialog::emitConfig()
{
    emit configUpdated(blocks->getColor(), box->currentText().toInt());
}
