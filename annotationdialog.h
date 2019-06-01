#ifndef ANNOTATIONDIALOG_H
#define ANNOTATIONDIALOG_H

#endif // ANNOTATIONDIALOG_H

#include <QDialog>
#include <QLayout>
#include <qtextedit.h>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>

namespace AnnotationDialog {
class ColorBlock;

class Dialog : public QDialog
{
public:
    Dialog();
    void cancelDialog();

};

class ColorBlock : public QLabel
{
    Q_OBJECT
public:
    ColorBlock(int idx);

    bool isSelected() const;
    void setIsSelected(bool value);
    void changeIsSelected();
    virtual void mouseReleaseEvent(QMouseEvent * ev);
    int index;
    void setStyle();
    QColor getColor();

signals:
    void clicked(int index);

private:
    bool isselected = false;
};

class ColorBlocks : public QWidget
{
public:
    ColorBlocks(QColor color);
    QList<ColorBlock *> colorblocks;
    void changeColors(int index);
    QColor getColor();
};


class FlatTextDialog : public Dialog
{
    Q_OBJECT
public:
    FlatTextDialog(const QString &text, const QFont &font, const QColor &color);
    QPushButton *submitBtn = new QPushButton(tr("Ensure"));
    QPushButton *deleteBtn = new QPushButton(tr("Delete"));
    QPushButton *cancelBtn = new QPushButton(tr("Cancle"));
    void emitConfig();
    void emitDelete();

    QTextEdit *edit;
    QComboBox *box;
    ColorBlocks *blocks;
    QFont font;

signals:
    void configUpdated(const QString &text, const QFont &font, const QColor &color);
    void itemDeleted();
};


class GeomDialog : public Dialog
{
    Q_OBJECT
public:
    GeomDialog(const QColor &color, int linewidth);
    QComboBox *box;
    QPushButton *submitBtn;
    QPushButton *deleteBtn;
    QPushButton *cancelBtn;

    void emitConfig();
    void emitDelete();

    ColorBlocks *blocks;

signals:
    void configUpdated(const QColor &color, int linewidth);
    void itemDeleted();
};

class LineDialog : public GeomDialog
{
    Q_OBJECT
public:
    LineDialog(const QColor &color, int linewidth, int linetype);
    void emitConfig();
    void emitDelete();

signals:
    void configUpdated(const QColor &color, int linetype);
};
}
