#ifndef ANNOPROCESSOR_H
#define ANNOPROCESSOR_H
#include <QObject>

class AnnoProcessor : public QObject
{
    Q_OBJECT
public:
    AnnoProcessor();
    virtual ~AnnoProcessor();
};

#endif // ANNOPROCESSOR_H
