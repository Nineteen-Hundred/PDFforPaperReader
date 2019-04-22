#include "annotoolbar.h"

AnnoToolbar::AnnoToolbar()
{
    actions[0] = new QAction(QIcon(":/image/text"), tr("Open"), this);
    actions[1] = new QAction(QIcon(":/image/text"), tr("Save"), this);
    actions[2] = new QAction(QIcon(":/image/text"), tr("Save As"), this);
    actions[3] = new QAction(QIcon(":/image/text"), tr("Line"), this);
    actions[4] = new QAction(QIcon(":/image/text"), tr("Flat Text"), this);
    actions[5] = new QAction(QIcon(":/image/text"), tr("Popup Text"), this);
    actions[6] = new QAction(QIcon(":/image/text"), tr("Geom"), this);
    actions[7] = new QAction(QIcon(":/image/text"), tr("Preview"), this);

    for(int i=0; i<8; i++)
    {
        addAction(actions[i]);
        if(i>=3)
        {
            actions[i]->setCheckable(true);
        }
    }

    setIconSize(QSize(10,10));
   setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
   setMovable(false);
}
