#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "administratorinterface.h"

using namespace std;


QByteArray HashingAndSalling(QString textToHash, quint32 saltValue);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initialConfigurationDone=false;
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

         ui->tableWidgetServices->setColumnCount(2);
         ui->tableWidgetServices->setEditTriggers(QAbstractItemView::NoEditTriggers);

         // set background and text color
         ui->tableWidgetServices->setShowGrid(false);
         ui->tableWidgetServices->setColumnWidth(0, 618);
         ui->tableWidgetServices->setColumnWidth(1, 125);
         ui->tableWidgetServices->setColumnWidth(2, 375);

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
    ui->tableWidgetServices->setRowCount(0);


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
                ui->tableWidgetServices->setRowCount(ui->tableWidgetServices->rowCount()+1);
                ui->tableWidgetServices->setItem(query.value(4).toInt()-1,0,new QTableWidgetItem(query.value(1).toString()));


                ui->tableWidgetServices->setItem(query.value(4).toInt()-1,1,new QTableWidgetItem(query.value(2).toString()+"min "+ query.value(3).toString() + "sec"));
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
    m_sequence=0;
    ui->pushButtonStart->setStyleSheet("QPushButton {background-color: orange; color: white;}");
    ui->pushButtonStart->setText("Continuer");
    // Timer pour le décompte
    m_timerNextSequence = new QTimer();

    m_timerNextSequence->connect(m_timerNextSequence, &QTimer::timeout, this, &MainWindow::ReadServices);


    // Définir le timer pour exécuter la fonction toutes les 60 secondes
    m_timerNextSequence->start(0);

}

void MainWindow::ReadServices()
{
    if(m_sequence<=ui->tableWidgetServices->rowCount()-1)
    {
        ColorRow(m_sequence);
        m_engine->say(ui->tableWidgetServices->item(m_sequence,0)->text());

/*
        int seconds;



  std::regex min_pattern("\\d+(?=min)");
  std::smatch min_results;
  std::regex_search(str1, min_results, min_pattern);
  int min1 = std::stoi(min_results[0]);
  std::regex_search(str2, min_results, min_pattern);
  int min2 = std::stoi(min_results[0]);

  std::regex sec_pattern("\\d+(?=sec)");
  std::smatch sec_results;
  std::regex_search(str1, sec_results, sec_pattern);
  int sec1 = std::stoi(sec_results[0]);
  std::regex_search(str2, sec_results, sec_pattern);
  int sec2 = std::stoi(sec_results[0]);

        */


     //a convertir les min et sec du string en ms   //m_timerNextSequence->setInterval(ui->tableWidgetServices->item(m_sequence,1)->text().toInt());
        m_timerNextSequence->setInterval(5000);

        m_sequence++;
    }
    else
    {
        m_timerNextSequence->stop();
    }
}

void MainWindow::ColorRow(int row)
{
    for(int i=0;i<ui->tableWidgetServices->columnCount();i++)
    {
        QTableWidgetItem *item = ui->tableWidgetServices->item(row, i);
        item->setBackground(QBrush(QColor(255, 165, 0)));
    }
}

void MainWindow::on_pushButtonStart_clicked()
{
        StartSequence();
}
