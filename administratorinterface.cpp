#include "administratorinterface.h"
#include "ui_administratorinterface.h"

AdministratorInterface::AdministratorInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdministratorInterface)
{
    ui->setupUi(this);
    ResetButtonAdmin();
}




AdministratorInterface::~AdministratorInterface()
{
    delete ui;
}

void AdministratorInterface::ResetButtonAdmin()
{
    ui->ButtonReset->setStyleSheet("QPushButton {background-color: red;}");
}

