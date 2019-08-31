#include "filewindow.h"

FileWindow::FileWindow(QWidget *parent) : QWidget(parent)
{
    if(parent != nullptr) this->mresize(parent->size());
    this->m_scorll = new QScrollArea(this); 
    this->m_file_list = new FileList(this->m_scorll);
    this->m_scorll->setWidget(this->m_file_list);

    //this->m_scorll->setBackgroundRole(QPalette::Dark);
    connect(this, SIGNAL(changeSizeSignal(QSize)), this->m_file_list, SLOT(sizeChange(QSize)));
}



int FileWindow::mresize(QSize size)
{
    this->resize(size);
    if(this->m_scorll != nullptr) this->m_scorll->resize(size);
    emit changeSizeSignal(size);
    return 0;
}
