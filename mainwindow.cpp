#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "administratorinterface.h"

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
         ui->tableWidgetServices->setColumnWidth(0, 1141);
         ui->tableWidgetServices->setColumnWidth(1, 450);
}

void MainWindow::ResizeRow()
{
    for(int i=0; i<ui->tableWidgetServices->rowCount(); i++){
        ui->tableWidgetServices->resizeRowToContents(i);
    }
}


int MainWindow::Database()
{
    ui->comboBoxChoixService_9->clear();
    // Création de la connexion à la base de données
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_pathDatabase);
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
    m_sequenceIsStart=false;
    ResetServices();
    CloseButtonConfiguration();
    SettingsButtonConfiguration();
    TableView();
#if _WIN32
    m_pathDatabase="D://journoudl.SNIRW//ServiceCom//ServiceCOM-2023//BDD_ServiceCom.db";
#elif __ANDROID__
    m_pathDatabase="/storage/emulated/0/Arhm/BDD_ServiceCom.db";
#endif
    Database();
    m_engine= new QTextToSpeech;
    m_sequenceIsStart=false;
    UpdateService(1);
}

void MainWindow::PushButtonStartConfiguration()
{
    ui->pushButtonStart->setText("Lancer");
    ui->pushButtonStart->setStyleSheet("QPushButton {background-color: #92d04f; color: white;}");
}

int MainWindow::on_comboBoxChoixService_9_currentIndexChanged(int index)
{
    if(m_sequenceIsStart)
    {
        StopSequence();
        UpdateService(ui->comboBoxChoixService_9->currentIndex()+1);
        ResetServices();

    }
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
    db.setDatabaseName(m_pathDatabase);
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
        ResizeRow();
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
    // Timer pour le décompte
    m_timerNextSequence = new QTimer();
    m_timerSeconds = new QTimer();
    m_timerNextSequence->connect(m_timerNextSequence, &QTimer::timeout, this, &MainWindow::ElapsedTime);
    m_timerSeconds->connect(m_timerSeconds, &QTimer::timeout, this, &MainWindow::Timer);
    // Définir le timer pour exécuter la fonction toutes les 60 secondes
    m_timerNextSequence->start(0);
    m_timerSeconds->start(1000);

}

void MainWindow::Timer()
{
    m_secondsRemaining--;
    qDebug()<<m_secondsRemaining;
    int minutes = m_secondsRemaining / 60;
    int secondes = m_secondsRemaining % 60;
    ui->tableWidgetServices->item(m_sequence-1,1)->setText(QString::number(minutes)+"min "+QString::number(secondes)+"sec");
}

void MainWindow::ElapsedTime() //Si le temps est écouler
{
    if(m_sequence<=ui->tableWidgetServices->rowCount()-1) //Si la séquence n'est pas finie
    {
        if(m_sequence!=0)
        {
            ColorRow(m_sequence-1,QColor(237,28,36)); //Mettre la ligne en rouge
            ui->tableWidgetServices->item(m_sequence-1,1)->setText("0min 0sec"); //Mettre le temps a 0
        }
        ReadServices();
    }
    else if(m_sequence==ui->tableWidgetServices->rowCount())
    {
        StopSequence();
        ColorRow(m_sequence-1,QColor(237,28,36));  //Mettre la ligne en rouge
    }
    else //Si la dernière ligne est atteinte
    {
        StopSequence();
        ColorRow(m_sequence-1,QColor(237,28,36)); //Mettre la ligne en rouge
        ui->tableWidgetServices->item(m_sequence-1,1)->setText("0min 0sec"); //Mettre le temps a 0s
        ResetServices();
    }

}

void MainWindow::ReadServices()
{    
        ColorRow(m_sequence, QColor(255, 165, 0)); //Color actual row in orange
        m_engine->say(ui->tableWidgetServices->item(m_sequence,0)->text()); //Say the actual row
        QString time = ui->tableWidgetServices->item(m_sequence,1)->text(); //Split minutes and seconds into 2 int
        QStringList timeParts = time.split(" ");
        int minutes = timeParts[0].remove("min").toInt();
        int seconds = timeParts[1].remove("sec").toInt();
        m_secondsRemaining=minutes*60+seconds;
        //a convertir les min et sec du string en ms
        m_timerNextSequence->setInterval((minutes*60+seconds)*1000); //Set Timer for the next sequence
        m_sequence++;
}

void MainWindow::ColorRow(int row, QColor color)
{
    for(int i=0;i<ui->tableWidgetServices->columnCount();i++)
    {
        QTableWidgetItem *item = ui->tableWidgetServices->item(row, i);
        item->setBackground(QBrush(color));
    }
}

void MainWindow::ResetServices()
{
    m_sequence=0;
    UpdateService(ui->comboBoxChoixService_9->currentIndex()+1);
    PushButtonStartConfiguration();
}

void MainWindow::StopSequence()
{
    m_timerNextSequence->stop(); //Arreter la sequence
    m_timerSeconds->stop();
    m_sequenceIsStart=false;
}

void MainWindow::on_pushButtonStart_clicked()
{
    if(!m_sequenceIsStart)
    {
        m_sequenceIsStart=true;
        StartSequence();
    }
    else {
        if(m_sequence<=ui->tableWidgetServices->rowCount()-1) //Si la séquence n'est pas finie
        {
            ReadServices();
            ColorRow(m_sequence-2,QColor(146,208,79)); //Mettre la ligne en verte
        }
        else //Si la dernière ligne est atteinte
        {
            StopSequence();
            ColorRow(m_sequence-1,QColor(146,208,79)); //Mettre la ligne en verte
            UpdateService(ui->comboBoxChoixService_9->currentIndex()+1);
            ResetServices();
        }
    }
}
