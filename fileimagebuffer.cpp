#include "fileimagebuffer.h"

FileImageBuffer::FileImageBuffer()
{
    //QFile pfile(this->m_config_path);
    //pfile.open(QIODevice::ReadWrite);
    //auto config_json = QJsonDocument::fromJson(pfile.readAll()).object();
    this->m_cache_path = FileConfigure::getValue("image_cache_dir").toString();
    QFileInfo cachePath(this->m_cache_path);
    if(cachePath.exists() == false) {
        qDebug() << "make dir:" << this->m_cache_path;
        QDir().mkpath(this->m_cache_path);
    }
    
}

QString FileImageBuffer::get_image_path(QString &filepath)
{
    QString cache_md5 = this->get_file_md5(filepath);
    cache_md5 = this->m_cache_path + "/" + cache_md5 + ".jpg";
    QFileInfo cache_info(cache_md5);
    if(cache_info.exists()) {
        return cache_md5;
    } else {
        this->make_image_cache(filepath, cache_md5);
        return cache_md5;
    }
}

QString FileImageBuffer::get_image_path(QString &filepath, int isPDF)
{
    if(!isPDF) {
        if(filepath == "default") {
            return this->m_return_image;
        } else if(filepath == "add_collection") {
            return this->m_add_collection_image;
        } else {
            return this->m_dir_image;
        }
    }
    QString cache_md5 = this->get_file_md5(filepath);
    cache_md5 = this->m_cache_path + "/" + cache_md5 + ".jpg";
    QFileInfo cache_info(cache_md5);
    if(cache_info.exists()) {
        return cache_md5;
    } else {
        this->make_image_cache(filepath, cache_md5);
        return cache_md5;
    }
}

FileImageBuffer::~FileImageBuffer()
{
    
}

QString FileImageBuffer::get_file_md5(QString &filepath)
{
    QString md5;
    QByteArray bb;
    bb = QCryptographicHash::hash ( filepath.toUtf8(), QCryptographicHash::Md5 );
    md5.append(bb.toHex());
    return md5;
}

int FileImageBuffer::make_image_cache(QString &filepath, QString &cache_path)
{
    auto temp_doc = Poppler::Document::load(filepath);
    if(!temp_doc) {
        qDebug() << "No this File";
        return -1;
    }
    if(temp_doc->isLocked()) {
        cache_path = this->m_default_path;
        return 0;
    } 
    temp_doc->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    temp_doc->setRenderHint(Poppler::Document::Antialiasing, 1);
    auto temp_page = temp_doc->page(0);
    temp_page->renderToImage().save(cache_path);
    delete temp_page;
    delete temp_doc;
    return 0;
}
