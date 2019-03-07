#-------------------------------------------------
#
# Project created by QtCreator 2019-03-07T00:02:26
#
#-------------------------------------------------

QT       += core gui

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

CONFIG += c++11, link_pkgconfig

SOURCES += \
    abstractinfodock.cpp \
    permissions.cpp \
    thumbnails.cpp \
    toc.cpp \
    viewer.cpp \
    metadata.cpp \
    navigationtoolbar.cpp \
    optcontent.cpp \
    pageview.cpp \
    documentobserver.cpp \
    embeddedfiles.cpp \
    fonts.cpp \
    info.cpp \
    main_viewer.cpp

HEADERS += \
    abstractinfodock.h \
    permissions.h \
    thumbnails.h \
    toc.h \
    viewer.h \
    metadata.h \
    navigationtoolbar.h \
    optcontent.h \
    pageview.h \
    documentobserver.h \
    embeddedfiles.h \
    fonts.h \
    info.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

PKGCONFIG += poppler-qt5

DISTFILES += \
    theme.qss
