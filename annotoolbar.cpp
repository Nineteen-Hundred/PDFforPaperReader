#include "annotoolbar.h"

AnnoToolbar::AnnoToolbar()
{
    open_action = new QAction(QIcon(":/image/open"), tr("打开"), this);
    addAction(open_action);
    saving_action = new QAction(QIcon(":/image/save"), tr("保存"), this);
    addAction(saving_action);
    saving_as_action = new QAction(QIcon(":/image/save_as"), tr("另存为"), this);
    addAction(saving_as_action);
    print_action = new QAction(QIcon(":/image/print"), tr("打印"), this);
    addAction(print_action);
    print_action->setCheckable(false);

    addSeparator();

    group.append(new QAction(QIcon(":/image/arrow"), tr("箭头"), this));
    group.append(new QAction(QIcon(":/image/text"), tr("文字"), this));
    group.append(new QAction(QIcon(":/image/comment"), tr("注释"), this));
    group.append(new QAction(QIcon(":/image/circle"), tr("圆形"), this));
    group.append(new QAction(QIcon(":/image/rect"), tr("方形"), this));

    for(int i=0; i<group.count(); i++)
    {
        addAction(group.at(i));
        group.at(i)->setCheckable(true);
        connect(group.at(i), &QAction::triggered, this, &AnnoToolbar::sendDrawing);
    }

    addSeparator();

    preview_action = new QAction(QIcon(":/image/preview"), tr("概览模式"), this);
    preview_action->setCheckable(true);
    addAction(preview_action);

    files_action = new QAction(QIcon(":/image/shoucang"), tr("收藏夹"), this);
    addAction(files_action);

    about_action = new QAction(QIcon(":/image/about"), tr("关于"), this);
    addAction(about_action);

    setIconSize(QSize(24,24));
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setMovable(false);

    this->saving_action->setEnabled(false);

    connect(about_action, &QAction::triggered, this, &AnnoToolbar::showAbout);
}

void AnnoToolbar::sendDrawing()
{
    QAction *action = (QAction *)sender();
    if(action->isChecked())
    {
        for(int i=0; i<group.count(); i++)
        {
            if(group.at(i)!=sender() && group.at(i)->isChecked())
            {
                group.at(i)->setChecked(false);
            }
        }
        action->setChecked(true);
        emit isDrawing(action->text(), true);
    }
    else
    {
        emit isDrawing(action->text(), false);
    }
}

void AnnoToolbar::showAbout()
{
    AboutWindow *about = new AboutWindow();
    about->exec();
}

void AnnoToolbar::resetStatus()
{
    for(int i=0; i<group.count(); i++)
    {
        group.at(i)->setChecked(false);
    }
}

void AnnoToolbar::initialStatus()
{
    for(int i=0; i<group.count(); i++)
    {
        group.at(i)->setEnabled(false);
    }
    saving_action->setEnabled(false);
    saving_as_action->setEnabled(false);
    print_action->setEnabled(false);
    preview_action->setEnabled(false);
    files_action->setEnabled(false);
}

void AnnoToolbar::pdfStatus()
{
    for(int i=0; i<group.count(); i++)
    {
        group.at(i)->setEnabled(true);
    }
    //saving_action->setEnabled(true);
    saving_as_action->setEnabled(true);
    print_action->setEnabled(true);
    preview_action->setEnabled(true);
    files_action->setEnabled(true);
}
