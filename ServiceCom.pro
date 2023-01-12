#-------------------------------------------------
#
# Project created by QtCreator 2023-01-05T14:25:11
#
#-------------------------------------------------

QT       += core gui
QT += uitools
QT += sql
RESOURCES += ressource.qrc \
    ressource.qrc



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServiceCom
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
        administratorinterface.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        administratorinterface.h \
        mainwindow.h

FORMS += \
    administratorinterface.ui \
    mainwindow.ui

CONFIG += mobility
MOBILITY = 


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
