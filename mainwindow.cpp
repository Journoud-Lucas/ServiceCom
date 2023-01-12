#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QtSql>
#include <iostream>
#include <QUiLoader>

void settings();
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    /*
     Code a écrire ici
    */
    database();
    closeButtonConfiguration();
    settingsButtonConfiguration();


}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::database()
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

void MainWindow::closeButtonConfiguration()
{
    //bouton fermer
    ui->pushButtonClose_9->setIcon(QIcon(QPixmap(":/images/close.png")));
    ui->pushButtonClose_9->setFlat(true);
    QObject::connect(ui->pushButtonClose_9, &QPushButton::clicked, &QApplication::quit);
}

void MainWindow::settingsButtonConfiguration()
{
    ui->pushButtonSettings_9->setIcon(QIcon(QPixmap(":/images/settings.png")));
    ui->pushButtonSettings_9->setFlat(true);
    QObject::connect(ui->pushButtonSettings_9, &QPushButton::clicked, this, &MainWindow::settings);
}

void MainWindow::settings()
{

    administratorInterface();
}

void MainWindow::administratorInterface()
{
    QUiLoader loader;
    QFile file("administratorinterface.ui");
    file.open(QFile::ReadOnly);
    QWidget* new_ui = loader.load(&file);
    file.close();
    //Utilisez la méthode setCentralWidget() de la fenêtre principale pour remplacer l'interface utilisateur actuelle par la nouvelle :
    QMainWindow* main_window = this; //this est l'objet de la fenêtre principale
    main_window->setCentralWidget(new_ui);
    //Utilisez la méthode hide() pour cacher l'interface utilisateur actuelle :
    QWidget* current_ui = main_window->centralWidget();
    current_ui->hide();
    //Utilisez la méthode show() pour afficher la nouvelle interface utilisateur:
    new_ui->show();
}
