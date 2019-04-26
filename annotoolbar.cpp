#include "annotoolbar.h"

AnnoToolbar::AnnoToolbar()
{
    open_action = new QAction(QIcon(":/image/text"), tr("Open"), this);
    addAction(open_action);
    saving_action = new QAction(QIcon(":/image/text"), tr("Save"), this);
    addAction(saving_action);
    saving_as_action = new QAction(QIcon(":/image/text"), tr("Save As"), this);
    addAction(saving_as_action);
    preview_action = new QAction(QIcon(":/image/text"), tr("Preview"), this);
    addAction(preview_action);


    group.append(new QAction(QIcon(":/image/text"), tr("Line"), this));
    group.append(new QAction(QIcon(":/image/text"), tr("Flat Text"), this));
    group.append(new QAction(QIcon(":/image/text"), tr("Popup Text"), this));
    group.append(new QAction(QIcon(":/image/text"), tr("Circle"), this));
    group.append(new QAction(QIcon(":/image/text"), tr("Rectangle"), this));
    group.append(new QAction(QIcon(":/image/text"), tr("Ink"), this));

    for(int i=0; i<group.count(); i++)
    {
        addAction(group.at(i));
        group.at(i)->setCheckable(true);
        connect(group.at(i), &QAction::triggered, this, &AnnoToolbar::sendDrawing);
    }

    setIconSize(QSize(10,10));
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setMovable(false);
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
