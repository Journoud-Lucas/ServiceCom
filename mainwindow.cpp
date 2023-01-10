#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Code a écrire ici
    ui->pushButton->setVisible(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

