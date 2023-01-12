#include "administratorinterface.h"
#include "ui_administratorinterface.h"

AdministratorInterface::AdministratorInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdministratorInterface)
{
    ui->setupUi(this);
}

AdministratorInterface::~AdministratorInterface()
{
    delete ui;
}
