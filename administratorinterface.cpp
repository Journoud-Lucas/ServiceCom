#include "administratorinterface.h"
#include "ui_administratorinterface.h"


AdministratorInterface::AdministratorInterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdministratorInterface)
{
#if _WIN32
    m_databasePath="D://journoudl.SNIRW//ServiceCom//ServiceCOM-2023//BDD_ServiceCom.db";
#elif __ANDROID__
    m_databasePath="/storage/emulated/0/Arhm/BDD_ServiceCom.db";
#endif
    initialConfigurationDone=false;
    ui->setupUi(this);
    ComboInit();
    initialConfigurationDone=true;
    UpdateService(1);



    ui->tableWidgetServicesAdmin->setColumnCount(4);
    ui->tableWidgetServicesAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // set background and text color
    ui->tableWidgetServicesAdmin->setShowGrid(false);
    ui->tableWidgetServicesAdmin->setColumnWidth(0, 1200);
    ui->tableWidgetServicesAdmin->setColumnWidth(1, 175);
    ui->tableWidgetServicesAdmin->setColumnWidth(2, 175);
    ui->tableWidgetServicesAdmin->setColumnWidth(3, 103);



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

int AdministratorInterface::ComboInit()
{
    ui->comboBoxService->clear();
    // Création de la connexion à la base de données
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        QString databasePath =  m_databasePath;
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
    QString databasePath =  m_databasePath;
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
            while (query.next()) {//
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


void AdministratorInterface::on_ButtonAjouter_clicked()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(m_databasePath);
            db.open();

            int maxIdActivite = 0;
            QSqlQuery query;
            query.prepare("SELECT MAX(IdItem) FROM titem");
            if (query.exec()) {
                if (query.next()) {
                    maxIdActivite = query.value(0).toInt();
                }
            }

            db.close();


            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("Ajouter une activité");
            QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
            QLabel *labelNameActivity = new QLabel("Nom activité :");
            QLineEdit *lineEditNameActivite = new QLineEdit;
            QPushButton *okButton = new QPushButton("OK");
            QLabel *labelMinutes = new QLabel("Minutes:");
            QLineEdit *lineEditMinutes = new QLineEdit;
            QLabel *labelNameSecondes = new QLabel("Secondes:");
            QLineEdit *lineEditSecondes = new QLineEdit;
            QLabel *labelOrdre = new QLabel("Ordre:");
            QLineEdit *LineEditOrdre = new QLineEdit;

            mainLayout->addWidget(labelNameActivity);
            mainLayout->addWidget(lineEditNameActivite);
            mainLayout->addWidget(labelMinutes);
            mainLayout->addWidget(lineEditMinutes);
            mainLayout->addWidget(labelNameSecondes);
            mainLayout->addWidget(lineEditSecondes);
            mainLayout->addWidget(labelOrdre);
            mainLayout->addWidget(LineEditOrdre);
            mainLayout->addWidget(okButton);

            dialog->setLayout(mainLayout);
            dialog->connect(okButton, &QPushButton::clicked, [&](){ //When validation button was pressed
                if (!lineEditNameActivite->text().isEmpty()&&!lineEditMinutes->text().isEmpty()&&!lineEditSecondes->text().isEmpty()&&!LineEditOrdre->text().isEmpty())
                {

                    dialog->accept();

                    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Définir le type de la base de donnée
                    db.setDatabaseName(m_databasePath); // Assigner le chemin à la variable de la base de donnée
                    db.open(); // Ouvrir la base de données

                    QString queryStr = QString("INSERT INTO titem (IdItem, NomItem, TempsMinute, TempsSeconde, OrdreItem, fk_titem_tservice) VALUES(%1, %2, %3, %4, %5, %6)").arg(QString::number(maxIdActivite+1)).arg(lineEditNameActivite->text()).arg(lineEditMinutes->text()).arg(lineEditSecondes->text()).arg(LineEditOrdre->text()).arg(QString::number(ui->comboBoxService->currentIndex()+1));
                    qDebug()<<QString("INSERT INTO titem (IdItem, NomItem, TempsMinute, TempsSeconde, OrdreItem, fk_titem_tservice) VALUES(%1, '%2', %3, %4, %5, %6)").arg(QString::number(maxIdActivite+1)).arg(lineEditNameActivite->text()).arg(lineEditMinutes->text()).arg(lineEditSecondes->text()).arg(LineEditOrdre->text()).arg(QString::number(ui->comboBoxService->currentIndex()+1));
                    QSqlQuery query; // Créer un objet de type QSqlQuery
                    query.prepare(queryStr); // Préparer la requête
                    if(query.exec()){ // Executer la requête
                        qDebug() << "Nouvelle ligne ajoutée avec succès"; // Afficher un message de succès
                        UpdateService(ui->comboBoxService->currentIndex()+1);
                    }
                    else{
                        qDebug() << "Erreur lors de l'ajout d'une nouvelle ligne"; // Afficher un message d'erreur
                    }
                    db.close(); // Fermer la base de données
                    db.removeDatabase("QSQLITE");
                 }
                else {
                    if (lineEditNameActivite->text().isEmpty())
                    {
                        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom d'activité"));
                    }
                    if (lineEditMinutes->text().isEmpty())
                    {
                        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom de son"));
                    }
                    if (lineEditSecondes->text().isEmpty())
                    {
                        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom pour l'image"));
                    }

                }

            });


            ResizeRow();
            dialog->exec();

}

void AdministratorInterface::ResizeRow()
{
    for(int i=0; i<ui->tableWidgetServicesAdmin->rowCount(); i++){
        ui->tableWidgetServicesAdmin->resizeRowToContents(i);
    }
}
