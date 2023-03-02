#include "administratorinterface.h"
#include "ui_administratorinterface.h"

AdministratorInterface::AdministratorInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdministratorInterface)
{
    initialConfigurationDone=false;
    ui->setupUi(this);
    ResetButtonAdmin();
    ComboInit();
    initialConfigurationDone=true;
    UpdateService(1);



    ui->tableWidgetServicesAdmin->setColumnCount(4);
    ui->tableWidgetServicesAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // set background and text color
    ui->tableWidgetServicesAdmin->setShowGrid(false);
    ui->tableWidgetServicesAdmin->setColumnWidth(0, 618);
    ui->tableWidgetServicesAdmin->setColumnWidth(1, 125);


    // Créer un nouvel élément et régler sa hauteur
    QTableWidgetItem *headerItem = new QTableWidgetItem("Nom item");
    headerItem->setSizeHint(QSize(0, 40));

    // Régler l'élément comme horizontal header
    ui->tableWidgetServicesAdmin->setHorizontalHeaderItem(0, headerItem);
}

AdministratorInterface::~AdministratorInterface()
{
    delete ui;
}

void AdministratorInterface::ResetButtonAdmin()
{
    ui->ButtonReset->setStyleSheet("QPushButton {background-color: red;}");
}

int AdministratorInterface::ComboInit()
{
    ui->comboBoxService->clear();
    // Création de la connexion à la base de données
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        QString databasePath = "D://journoudl.SNIRW//ServiceCom//ServiceCOM-2023//BDD_ServiceCom.db";
        db.setDatabaseName(databasePath);
        if (!db.open()) {
            qDebug() << "Erreur lors de l'ouverture de la base de données (databse initial) : " << db.lastError().text() << '\n';
            return 1;
        }

        // Exécution de la requête
        for(int i=1;i<5;i++)
        {
            QSqlQuery queryService("SELECT * FROM tservice WHERE IdService = "+QString::number(i));
            if(!queryService.exec()){
                qDebug() << "Erreur lors de l'execution de la requête : " << queryService.lastError().text() << '\n';
                db.close();
                return 1;
            }

            // Affichage des résultats
            while (queryService.next()) {
                qDebug()<<"Service : "<< queryService.value(0).toString() << ", colonne2 : " << queryService.value(1).toString();
                ui->comboBoxService->addItem(queryService.value(1).toString());

            }
        }
        // Fermeture de la connexion
        db.close();
        return 0;
}
int AdministratorInterface::UpdateService(int indexNombre)
{
    ui->tableWidgetServicesAdmin->setRowCount(0);

    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    QString databasePath = "D://journoudl.SNIRW//ServiceCom//ServiceCOM-2023//BDD_ServiceCom.db";
    db.setDatabaseName(databasePath);

    if (!db.open()) {
        qDebug() << "Erreur lors de l'ouverture de la base de données (update service) : " << db.lastError().text() << '\n';
        return 1;
    }
            QSqlQuery query("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(indexNombre));
            if(!query.exec()){
                qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
                db.close();
                return 1;
            }


            // Affichage des résultats
            while (query.next()) {
                ui->tableWidgetServicesAdmin->setRowCount(ui->tableWidgetServicesAdmin->rowCount()+1);
                ui->tableWidgetServicesAdmin->setItem(query.value(4).toInt()-1,0,new QTableWidgetItem(query.value(1).toString()));
                ui->tableWidgetServicesAdmin->setItem(query.value(4).toInt()-1, 1, new QTableWidgetItem(query.value(2).toString()+"min "));
                ui->tableWidgetServicesAdmin->setItem(query.value(4).toInt()-1, 2, new QTableWidgetItem(query.value(3).toString() + "sec"));
                ui->tableWidgetServicesAdmin->setItem(query.value(4).toInt()-1, 3, new QTableWidgetItem(query.value(4).toString()));
            }

        // Fermeture de la connexion
        db.close();
        return 0;
}

int AdministratorInterface::on_comboBoxService_currentIndexChanged(int index)
{
    if(this->initialConfigurationIsDone())
    {
        UpdateService(index+1);
    }
return 0;
}

bool AdministratorInterface::initialConfigurationIsDone()
{
    return initialConfigurationDone;
}

