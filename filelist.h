#ifndef FILELIST_H
#define FILELIST_H

#include <QWidget>
#include "filethumlabel.h"
#include <QList>
#include <QGridLayout>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include "fileconfigure.h"
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QStack>

class FileList : public QWidget
{
    Q_OBJECT
public:
    explicit FileList(QWidget *parent = nullptr);
    QList<FileThumLabel *> m_pdf_list;
    QGridLayout *m_layout;
    int into_dir(QString path);
    int draw();
    int redraw();
    QMenu *m_context_menu = nullptr;
    QString m_now_path = "default";
    FileThumLabel *OperaItem;
private:
    QList<QString> get_file_list(QString path);
    int add_collection();
    QStack<QString> m_content_stack;
signals:
    void sendFileName(QString filename);
    
public slots:
    void sizeChange(QSize size);
    void clickThum(FileThumLabel *item);
    void contextClickThum(FileThumLabel *item, QPoint pos);
    void contextOpen();
    void contextDel();
};

#endif // FILELIST_H
