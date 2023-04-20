/********************************************************************
    created:	2022-01-06
    file path:	main.cpp
    author:		Lucas Journoud
    copyright:	W.I.P

    purpose: 	call the main window
*********************************************************************/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
