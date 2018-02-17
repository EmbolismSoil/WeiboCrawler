#-------------------------------------------------
#
# Project created by QtCreator 2018-02-15T00:47:28
#
#-------------------------------------------------

QT       += core gui webenginewidgets
QT += xmlpatterns
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WeiboCrawler
TEMPLATE = app
QMAKE_CXXFLAGS += -rdynamic

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
        main.cpp \
    userinfoextractor.cpp \
    mysqlclientfactory.cpp \
    maineventloop.cpp \
    scrawlercontroller.cpp \
    loginwindow.cpp \
    userrelationextractor.cpp \
    relationstroage.cpp

HEADERS += \
    htmlloader.h \
    userinfoextractor.h \
    userinfo.h \
    userinfostroage.h \
    mysqlclientfactory.h \
    pageproxy.h \
    maineventloop.h \
    common.h \
    scrawlercontroller.h \
    loginwindow.h \
    userrelation.h \
    userrelationextractor.h \
    relationstroage.h

LIBS += -L/usr/local/lib/ -lgq
LIBS += -ljsoncpp

FORMS +=

DISTFILES += \
    config.json \
    jquery.min.js
