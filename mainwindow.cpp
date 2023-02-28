#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "administratorinterface.h"



QByteArray HashingAndSalling(QString textToHash, quint32 saltValue);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    administratorInterface=new AdministratorInterface;
    ui->setupUi(this);
    this->showFullScreen();
    InitialConfiguration();
    initialConfigurationDone=true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TableView()
{
    model = new QStandardItemModel();
    // Assigner le modèle à la vue
    ui->tableView->setModel(model);
    // Définir le nombre de colonnes et de lignes
    model->setColumnCount(3);
    ui->tableView->setShowGrid(false);
    ui->tableView->setColumnWidth(0, 300);
    model->setHeaderData(0, Qt::Horizontal, "Service en cours");
    model->setHeaderData(1, Qt::Horizontal, "");
    model->setHeaderData(2, Qt::Horizontal, "Temps restant");
}

int MainWindow::Database()
{
    ui->comboBoxChoixService_9->clear();
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
                ui->comboBoxChoixService_9->addItem(queryService.value(1).toString());

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
    ui->pushButtonClose_9->setIconSize(QSize(75,75));
    ui->pushButtonClose_9->setFlat(true);
    QObject::connect(ui->pushButtonClose_9, &QPushButton::clicked, &QApplication::quit);
}

void MainWindow::SettingsButtonConfiguration()
{
    ui->pushButtonSettings_9->setIcon(QIcon(QPixmap(":/images/settings.png")));
    ui->pushButtonSettings_9->setIconSize(QSize(75,75));
    ui->pushButtonSettings_9->setFlat(true);
    QObject::connect(ui->pushButtonSettings_9, &QPushButton::clicked, this, &MainWindow::Settings);
}

void MainWindow::Settings()
{
//Hashache et salage (salage générer aléatoirement) du mot de passe
    QRandomGenerator generator;
    quint32 saltValue = generator.generate();
    QByteArray hashedPassword = HashingAndSalling("arhm", saltValue);


    QDialog *dialog = new QDialog(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QLabel *label = new QLabel("Entrez le mot de passe :");
    QLineEdit *lineEdit = new QLineEdit;
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Annuler");
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addWidget(okButton);
    mainLayout->addWidget(cancelButton);
    dialog->setLayout(mainLayout);
    dialog->connect(cancelButton, &QPushButton::clicked, [&](){
            dialog->reject();
    });
    dialog->connect(okButton, &QPushButton::clicked, [&](){ //When validation button was pressed
        //Hashage du text
        QString enterText = lineEdit->text().toLower();
        QByteArray hashedEnterText = HashingAndSalling(enterText, saltValue);


        if(hashedEnterText==hashedPassword) { //Corect password
            dialog->accept();
            ShowAdministratorInterface();
        } else { //Incorrect Password
            QMessageBox::critical(this, tr("Erreur"), tr("Le mot de passe entré est incorect.\nVeuillez réessayez"));
        }
});
    dialog->exec();
}

QByteArray HashingAndSalling(QString textToHash, quint32 saltValue)
{
    unsigned int saltUInt = static_cast<unsigned int>(saltValue);
    char salt = static_cast<char>(saltUInt);
    QByteArray passwordBytes = (textToHash+salt).toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return hashedPassword;
}

void MainWindow::ShowAdministratorInterface()
{
    administratorInterface->showFullScreen();
}

void MainWindow::InitialConfiguration()
{
    ui->pushButtonStart->setText("Lancer");
    ui->pushButtonStart->setStyleSheet("QPushButton {background-color: #92d04f; color: white;}");
    CloseButtonConfiguration();
    SettingsButtonConfiguration();
    TableView();
    Database();
    m_engine= new QTextToSpeech;
    m_engine->say(model->data(model->index(0, 0)).toString());

    UpdateService(1);
}



int MainWindow::on_comboBoxChoixService_9_currentIndexChanged(int index)
{
    if(initialConfigurationIsDone())
    {
        UpdateService(index+1);
    }
return 0;
}


int MainWindow::UpdateService(int indexNombre)
{
    model->setRowCount(0);


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
                model->setRowCount(model->rowCount()+1);
                model->setData(model->index(query.value(4).toInt()-1, 0), query.value(1).toString(), Qt::EditRole);
                model->setData(model->index(query.value(4).toInt()-1, 2), query.value(2).toString()+"min "+ query.value(3).toString() + "sec", Qt::EditRole);
            }

        // Fermeture de la connexion

        db.close();
        return 0;
}

bool MainWindow::initialConfigurationIsDone()
{
    return initialConfigurationDone;
}

void MainWindow::StartSequence()
{
    ui->pushButtonStart->setStyleSheet("QPushButton {background-color: orange; color: white;}");
    ui->pushButtonStart->setText("Continuer");

}

void MainWindow::on_pushButtonStart_clicked()
{
        StartSequence();
}
