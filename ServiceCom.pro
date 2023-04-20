#-------------------------------------------------
#
# Project created by QtCreator 2023-01-05T14:25:11
#
#-------------------------------------------------

QT += widgets
QT += core gui
QT += sql
QT += texttospeech
RESOURCES += ressource.qrc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServiceCom
TEMPLATE = app
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
    mainwindow.ui

CONFIG += mobility
MOBILITY = 

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
