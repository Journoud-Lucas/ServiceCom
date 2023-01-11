#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QtSql>
#include <iostream>

int database();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Code a écrire ici
    database();



}

MainWindow::~MainWindow()
{
    delete ui;
}

int database()
{

    // Création de la connexion à la base de données
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("D:\\journoudl.SNIRW\\ServiceCom\\ServiceCOM-2023\\BDD_ServiceCom.db");
        if (!db.open()) {
            std::cout << "Erreur lors de l'ouverture de la base de données : " << db.lastError().text().toStdString() << std::endl;
            return 1;
        }

        // Exécution de la requête
        QSqlQuery query("SELECT * FROM titem");
        if(!query.exec()){
            std::cout << "Erreur lors de l'execution de la requête : " << query.lastError().text().toStdString() << std::endl;
            db.close();
            return 1;
        }

        // Affichage des résultats
        while (query.next()) {
            qDebug()<<"colonne1 : "<< query.value(0).toString() << ", colonne2 : " << query.value(1).toString() <<'\n';
            //std::cout << "colonne1 : " << query.value(0).toString().toStdString() << ", colonne2 : " << query.value(1).toString().toStdString() << std::endl;
        }

        // Fermeture de la connexion
        db.close();
        return 0;
}
