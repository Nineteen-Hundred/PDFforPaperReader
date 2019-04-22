#ifndef ANNOTOOLBAR_H
#define ANNOTOOLBAR_H

#include <QToolBar>


class AnnoToolbar : public QToolBar
{
    Q_OBJECT
public:
    AnnoToolbar();
    QAction *actions[8];
};

#endif // ANNOTOOLBAR_H
