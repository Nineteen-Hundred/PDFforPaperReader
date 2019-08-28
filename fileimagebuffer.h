#ifndef FILEIMAGEBUFFER_H
#define FILEIMAGEBUFFER_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QCryptographicHash>
#include <poppler/qt5/poppler-qt5.h>
#include <QImage>
#include <QFileInfo>
#include "fileconfigure.h"
#include <QDir>

class FileImageBuffer
{
public:
    FileImageBuffer();
    QString get_image_path(QString &filepath);
    QString get_image_path(QString &filepath, int isPDF);
    ~FileImageBuffer();
private:
    QString get_file_md5(QString &filepath);
    int make_image_cache(QString &filepath, QString &cache_path);
    QString m_cache_path;
    QString m_default_path = ":/pdf.png";
    QString m_dir_image = ":/dir.png";
    QString m_return_image = ":/return.png";
    QString m_add_collection_image = ":/add_collection.png";
};

#endif // FILEIMAGEBUFFER_H
