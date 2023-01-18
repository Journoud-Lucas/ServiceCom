#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QtSql>
#include <iostream>
#include <QUiLoader>
#include <QApplication>
#include <QPushButton>
#include <QUiLoader>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>

QByteArray HashingAndSalling(QString textToHash, quint32 saltValue);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showFullScreen();
    /*
     Code a écrire ici
    */
    InitialConfiguration();
    Database();
    CloseButtonConfiguration();
    SettingsButtonConfiguration();


}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::Database()
{

    // Création de la connexion à la base de données
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        QString databasePath = "D://journoudl.SNIRW//ServiceCom//ServiceCOM-2023//BDD_ServiceCom.db";
        db.setDatabaseName(databasePath);
        if (!db.open()) {
            qDebug() << "Erreur lors de l'ouverture de la base de données : " << db.lastError().text() << '\n';
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
                qDebug()<<"Sevice : "<< queryService.value(0).toString() << ", colonne2 : " << queryService.value(1).toString();
            }

            QSqlQuery query("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(i));
            if(!query.exec()){
                qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
                db.close();
                return 1;
            }

            // Affichage des résultats
            while (query.next()) {
                qDebug()<<"colonne1 : "<< query.value(0).toString() << ", colonne2 : " << query.value(1).toString()
                       << ", colonne3 : " << query.value(2).toString() << ", colonne4 : " << query.value(3).toString()
                       << ", colonne5 : " << query.value(4).toString() << ", colonne5 : " << query.value(5).toString();
            }

        }
        // Fermeture de la connexion
        db.close();
        return 0;
}

void MainWindow::CloseButtonConfiguration()
{
    //bouton fermer
    ui->pushButtonClose_9->setIcon(QIcon(QPixmap(":/images/close.png")));
    ui->pushButtonClose_9->setFlat(true);
    QObject::connect(ui->pushButtonClose_9, &QPushButton::clicked, &QApplication::quit);
}

void MainWindow::SettingsButtonConfiguration()
{
    ui->pushButtonSettings_9->setIcon(QIcon(QPixmap(":/images/settings.png")));
    ui->pushButtonSettings_9->setFlat(true);
    QObject::connect(ui->pushButtonSettings_9, &QPushButton::clicked, this, &MainWindow::Settings);
}

void MainWindow::Settings()
{
    //Hashache et salage (salage générer aléatoirement) du mot de passe
    QRandomGenerator generator;
    quint32 saltValue = generator.generate();
    QByteArray hashedPassword = HashingAndSalling("arhm", saltValue);


    QUiLoader loader;
    QFile file(":/ui/clogindlg.ui");
    file.open(QFile::ReadOnly);
    QDialog *dialog = qobject_cast<QDialog*>(loader.load(&file, this));
    file.close();

    QPushButton *okButton = dialog->findChild<QPushButton*>("pushButtonValiderMdp");
    QLineEdit *lineEdit=dialog->findChild<QLineEdit*>("lineEditMdp");

    dialog->connect(okButton, &QPushButton::clicked, [&](){ //When validation button was pressed
        //Hashage du text
        QString enterText = lineEdit->text();
        QByteArray hashedEnterText = HashingAndSalling(enterText, saltValue);


        if(hashedEnterText==hashedPassword) { //Corect password
            dialog->accept();
            AdministratorInterface();
        } else { //Incorrect Password
            QMessageBox::critical(this, tr("Erreur"), tr("Le mot de passe entré est incorect.\nVeuillez réessayez"));
        }
});
    dialog->exec();


    //code pour le mot de passe
}

QByteArray HashingAndSalling(QString textToHash, quint32 saltValue)
{
    unsigned int saltUInt = static_cast<unsigned int>(saltValue);
    char salt = static_cast<char>(saltUInt);
    QByteArray passwordBytes = (textToHash+salt).toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return hashedPassword;
}

void MainWindow::AdministratorInterface()
{
    QUiLoader loader;
    QFile file(":/ui/administratorinterface.ui");
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

void MainWindow::InitialConfiguration()
{
    ui->pushButtonStart->setText("Lancer");
    ui->pushButtonStart->setStyleSheet("QPushButton {background-color: #92d04f; color: white;}");
}
