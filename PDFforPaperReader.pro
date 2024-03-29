#-------------------------------------------------
#
# Project created by QtCreator 2019-03-09T20:09:02
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PDFforPaperReader
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    aboutwindow.cpp \
    autodocument.cpp \
    fileconfigure.cpp \
    fileimagebuffer.cpp \
    filelist.cpp \
    filethumlabel.cpp \
    filewindow.cpp \
        main.cpp \
        mainwindow.cpp \
    pageviewwidget.cpp \
    paperviews.cpp \
    paperannotation.cpp \
    annotationdialog.cpp \
    annotoolbar.cpp

HEADERS += \
    aboutwindow.h \
    autodocument.h \
    fileconfigure.h \
    fileimagebuffer.h \
    filelist.h \
    filethumlabel.h \
    filewindow.h \
        mainwindow.h \
    pageviewwidget.h \
    paperviews.h \
    paperannotation.h \
    annotationdialog.h \
    annotoolbar.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += poppler-qt5

RESOURCES += \
    filesys.qrc \
    images.qrc

DISTFILES += \
    add_collection.png \
    config.json \
    dir.png \
    pdf.png \
    pdf_filesystem.pro.user \
    return.png
