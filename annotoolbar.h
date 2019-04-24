#ifndef ANNOTOOLBAR_H
#define ANNOTOOLBAR_H

#include <QToolBar>
#include <QAction>
#include <QDebug>

class AnnoAction : public QAction
{
    Q_OBJECT
public:
    AnnoAction(const QIcon &icon, const QString &text, int index, QObject *parent = nullptr): QAction(icon, text, parent), index(index){
    }
    int index = 0;
};

class AnnoToolbar : public QToolBar
{
    Q_OBJECT
public:
    AnnoToolbar();
    //QAction *actions[8];
    QAction *saving_action, *saving_as_action, *open_action;
    QList<QAction *> group;
    void sendDrawing();
    void sendSaving();
    void sendSavingAs();
    void sendOpening();

signals:
    void isDrawing(const QString &text, bool is_drawing);
    void isSaving();
    void isSavingAs();
    void isOpening();
};

#endif // ANNOTOOLBAR_H
