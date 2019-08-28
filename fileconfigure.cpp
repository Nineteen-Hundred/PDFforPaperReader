#include "fileconfigure.h"

FileConfigure::FileConfigure()
{
    
    QFileInfo dirfile(this->m_config_path);
    
    if(dirfile.exists() == false) {
        qDebug() << "make dir:" << this->m_config_path;
        QDir().mkdir(dirfile.dir().absolutePath());
        QFile::copy(":/config.json", this->m_config_path);
        QFile().setPermissions(this->m_config_path, QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ExeOwner);
    }
    
    QFile pfile(this->m_config_path);
    pfile.open(QIODevice::ReadWrite);
    
    this->m_config = QJsonDocument::fromJson(pfile.readAll()).object();
    pfile.close();
}

QJsonValue FileConfigure::getValue(QString path)
{
    FileConfigure self_conf;
    QStringList path_list = path.split(".");
    int loop_times = 0;
    QJsonValue temp_result;
    QJsonObject temp_object = self_conf.m_config;
    for(auto item : path_list) {
        loop_times++;
        temp_result = temp_object[item];
        if(loop_times != path_list.size()) temp_object = temp_result.toObject();
    }
    return temp_result;
}

int FileConfigure::add_collection(QString path)
{
    FileConfigure self_conf;
    QJsonArray temp_object = self_conf.m_config["search_path"].toArray();
    temp_object.append(path);
    self_conf.m_config["search_path"] = temp_object;
    QFile pfile(self_conf.m_config_path);
    pfile.open(QIODevice::ReadWrite);
    pfile.write(QJsonDocument(self_conf.m_config).toJson());
    pfile.close();
    return 0;
}

int FileConfigure::remove_collection(QString path)
{
    FileConfigure self_conf;
    QJsonArray temp_object = self_conf.m_config["search_path"].toArray();
    //temp_object.append(path);
    int loop_i;
    for(loop_i = 0; loop_i < temp_object.size(); ++loop_i) {
        if(temp_object.at(loop_i) == path) {
            break;
        }
    }
    temp_object.removeAt(loop_i);
    self_conf.m_config["search_path"] = temp_object;
    QFile pfile(self_conf.m_config_path);
    pfile.open(QIODevice::ReadWrite|QFile::Truncate);
    pfile.write(QJsonDocument(self_conf.m_config).toJson());
    pfile.close();
    return 0;
}
