#include "mainwindow.h"
#include "ui_mainwindow.h"

QByteArray HashingAndSalling(QString textToHash, quint32 saltValue);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initialConfigurationDone=false;
    this->m_update=true;
    ui->setupUi(this);
    this->showFullScreen();
    InitialConfiguration();
    initialConfigurationDone=true;
    DecrementIdActivite();

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

         ui->tableWidgetServicesAdmin->setColumnCount(2);
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

void MainWindow::ResizeRow()
{
    for(int i=0; i<ui->tableWidgetServices->rowCount(); i++){
        ui->tableWidgetServices->resizeRowToContents(i);
    }

    for(int i=0; i<ui->tableWidgetServicesAdmin->rowCount(); i++){
        ui->tableWidgetServicesAdmin->resizeRowToContents(i);
    }
}


int MainWindow::ConfigComboBox()
{
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_databasePath);

        if (!db.open()) {
            qDebug() << "Erreur lors de l'ouverture de la base de données (config combobox) : " << db.lastError().text() << '\n';
            return 1;
        }
                QSqlQuery query("SELECT NameService FROM tservice");
                if(!query.exec()){
                    qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
                    db.close();
                    return 1;
                }


                // Affichage des résultats
                while (query.next()) {
                     ui->comboBoxServices->addItem(query.value(0).toString());
                    ui->comboBoxServicesAdmin->addItem(query.value(0).toString());
                }

            // Fermeture de la connexion

            db.close();
            db.removeDatabase("QSQLITE");

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
            ui->stackedWidget->setCurrentIndex(1);
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


void MainWindow::InitialConfiguration()
{
    m_serviceIsStart=false;
    ResetServices();
    CloseButtonConfiguration();
    SettingsButtonConfiguration();
    TableView();
#if _WIN32
    m_databasePath="D://Users//Warp//Documents//arhm//BDD_ServiceCom.db";
#elif __ANDROID__
    m_databasePath="/storage/emulated/0/Arhm/BDD_ServiceCom.db";
#endif
    UpdateSequence(GetSelectedIndexService());
    m_engine= new QTextToSpeech;
    m_serviceIsStart=false;
    ui->stackedWidget->setCurrentIndex(0);//MainWindow
}

void MainWindow::PushButtonStartConfiguration()
{
    ui->pushButtonStart->setText("Lancer");
    ui->pushButtonStart->setStyleSheet("QPushButton {background-color: #92d04f; color: white;}");
}

void MainWindow::on_comboBoxServices_currentIndexChanged(int index)
{
    ComboboxChange(index);
}

void MainWindow::ComboboxChange(int index)
{
    ui->comboBoxServicesAdmin->setCurrentIndex(index);
    ui->comboBoxServices->setCurrentIndex(index);

    if(m_serviceIsStart)
    {
                     StopSequence();
                     UpdateService(ui->comboBoxServices->currentIndex()+1);
                     ResetServices();

    }
    if(initialConfigurationIsDone()&&m_update)
    {
        UpdateService(index+1);
        ChangeLastSelectedIndexService(index);
    }
}

void MainWindow::on_comboBoxServicesAdmin_currentIndexChanged(int index)
{
    ComboboxChange(index);
}

int MainWindow::UpdateService(int indexNombre)
{
    ui->tableWidgetServices->setRowCount(0);
    ui->tableWidgetServicesAdmin->setRowCount(0);



        QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_databasePath);

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
                    ui->tableWidgetServices->setRowCount((ui->tableWidgetServices->rowCount())+1);
                    ui->tableWidgetServicesAdmin->setRowCount((ui->tableWidgetServicesAdmin->rowCount())+1);
                    ui->tableWidgetServices->setItem(query.value(3).toInt()-1,0,new QTableWidgetItem(query.value(1).toString())); //NomActiviter
                    ui->tableWidgetServicesAdmin->setItem(query.value(3).toInt()-1,0,new QTableWidgetItem(query.value(1).toString())); //NomActiviter
                    QString tempsHeureMinuteSeconde = QString("%1h %2m %3s").arg(query.value(2).toInt()/3600).arg((query.value(2).toInt()%3600)/60).arg(query.value(2).toInt()%60);
                    ui->tableWidgetServices->setItem(query.value(3).toInt()-1,1,new QTableWidgetItem(tempsHeureMinuteSeconde)); //TempHeureMinuteSeconde
                    ui->tableWidgetServicesAdmin->setItem(query.value(3).toInt()-1,1,new QTableWidgetItem(tempsHeureMinuteSeconde)); //TempHeureMinuteSeconde
                }

            // Fermeture de la connexion

            db.close();
            db.removeDatabase("QSQLITE");
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
    ui->tableWidgetServices->item(m_service-1,1)->setText(QString("%1h %2m %3s").arg(m_secondsRemaining/3600).arg((m_secondsRemaining%3600)/60).arg(m_secondsRemaining%60));
}

void MainWindow::ElapsedTime() //Si le temps est écouler
{
    if(m_service<=ui->tableWidgetServices->rowCount()-1) //Si la séquence n'est pas finie
    {
        if(m_service!=0)
        {
            ColorRow(m_service-1,QColor(237,28,36)); //Mettre la ligne en rouge
            ui->tableWidgetServices->item(m_service-1,1)->setText("0h 0m 0s"); //Mettre le temps a 0
        }
        ReadServices();
    }
    else //Si la dernière ligne est atteinte
    {
        StopSequence();
        ColorRow(m_service-1,QColor(237,28,36)); //Mettre la ligne en rouge
        ui->tableWidgetServices->item(m_service-1,1)->setText("0h 0m 0s"); //Mettre le temps a 0s
        ResetServices();
    }
}

void MainWindow::ReadServices()
{    
        ColorRow(m_service, QColor(255, 165, 0)); //Color actual row in orange
        m_engine->say(ui->tableWidgetServices->item(m_service,0)->text()); //Say the actual row
        QString time = ui->tableWidgetServices->item(m_service,1)->text(); //Split hours, minutes and seconds into 2 int
        QStringList timeParts = time.split(" ");
        int heures = timeParts[0].remove("h").toInt();
        int minutes = timeParts[1].remove("m").toInt();
        int seconds = timeParts[2].remove("s").toInt();
        m_secondsRemaining=heures*3600+minutes*60+seconds;
        //a convertir les hn les m et s du string en ms
        m_timerNextSequence->setInterval((heures*3600+minutes*60+seconds)*1000); //Set Timer for the next sequence
        m_service++;
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
    m_service=0;
    UpdateService(ui->comboBoxServices->currentIndex()+1);
    PushButtonStartConfiguration();
}

void MainWindow::StopSequence()
{
    m_timerNextSequence->stop(); //Arreter la sequence
    m_timerSeconds->stop();
    m_serviceIsStart=false;
}

void MainWindow::on_pushButtonStart_clicked()
{
    if(ui->tableWidgetServices->rowCount()!=0)
    {
        if(!m_serviceIsStart)
        {
            m_serviceIsStart=true;
            StartSequence();
        }
        else {
            if(m_service<=ui->tableWidgetServices->rowCount()-1) //Si la séquence n'est pas finie
            {
                ReadServices();
                ColorRow(m_service-2,QColor(146,208,79)); //Mettre la ligne en verte
            }
            else //Si la dernière ligne est atteinte
            {
                StopSequence();
                ColorRow(m_service-1,QColor(146,208,79)); //Mettre la ligne en verte
                UpdateService(ui->comboBoxServices->currentIndex()+1);
                ResetServices();
            }
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::UpdateSequence(int selectedSequence)
{
    m_update=false;
    ui->comboBoxServices->clear();
    ui->comboBoxServicesAdmin->clear();
    ConfigComboBox();
    m_update=true;
    ui->comboBoxServices->setCurrentIndex(selectedSequence);
    ui->comboBoxServicesAdmin->setCurrentIndex(selectedSequence);
    UpdateService(selectedSequence+1);
}

void MainWindow::on_ButtonAjouter_clicked()
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
        QLabel *labelNameActivity = new QLabel("Nom d'activité :");
        QLineEdit *lineEditNameActivite = new QLineEdit;
        QPushButton *okButton = new QPushButton("OK");
        QLabel *labelTime = new QLabel("Temps:");
        QTimeEdit *timeEdit = new QTimeEdit;
        timeEdit->setDisplayFormat("hh:mm:ss");

        mainLayout->addWidget(labelNameActivity);
        mainLayout->addWidget(lineEditNameActivite);
        mainLayout->addWidget(labelTime);
        mainLayout->addWidget(timeEdit);
        mainLayout->addWidget(okButton);
        dialog->connect(okButton, &QPushButton::clicked, [&](){ //When validation button was pressed
            if (!lineEditNameActivite->text().isEmpty()&&timeEdit->time().toString()!="00:00:00")
            {

                dialog->accept();

                QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Définir le type de la base de donnée
                db.setDatabaseName(m_databasePath); // Assigner le chemin à la variable de la base de donnée
                db.open(); // Ouvrir la base de données


                QString queryStr = QString("INSERT INTO titem (IdItem, NameItem, Time, OrdreItem, fk_titem_tservice) VALUES(%1, '%2', %3, %4, %5)").arg(QString::number(maxIdActivite+1)).arg(lineEditNameActivite->text()).arg(QString::number(timeEdit->time().hour()*3600+timeEdit->time().minute()*60+timeEdit->time().second())).arg(QString::number(ui->tableWidgetServicesAdmin->rowCount()+1)).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
                qDebug()<<queryStr;
                QSqlQuery query; // Créer un objet de type QSqlQuery
                query.prepare(queryStr); // Préparer la requête
                if(query.exec()){ // Executer la requête
                    qDebug() << "Nouvelle ligne ajoutée avec succès"; // Afficher un message de succès
                    UpdateService(ui->comboBoxServicesAdmin->currentIndex()+1);
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
                if (timeEdit->time().toString()=="00:00:00")
                {
                    QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un temps pour l'activité"));
                }

            }

        });
        ResizeRow();
        dialog->exec();
}

void MainWindow::on_ButtonModifier_clicked()
{
    QList<QTableWidgetSelectionRange> selectionRanges = ui->tableWidgetServicesAdmin->selectedRanges();
    int numberOfRow=0;
    int firstRow;
    int lastRow;
    if(!selectionRanges.isEmpty())
    {
        // Récupérer le numéro de ligne de la première case sélectionnée
        firstRow = selectionRanges.first().topRow();
        // Récupérer le numéro de ligne de la dernière case sélectionnée
        lastRow = selectionRanges.last().bottomRow();
        numberOfRow=lastRow-firstRow;
        numberOfRow++;
    }

    if(selectionRanges.isEmpty())
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez séléctionnez une ligne a modifier"));
    }
    else if(numberOfRow==1)
    {
        QRegExpValidator *validator = new QRegExpValidator(QRegExp("[A-Za-z0-9 éè!?.']*"));

        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Modifier une activité");
        QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
        QLabel *labelNameActivity = new QLabel("Nom d'activité :");
        QLineEdit *lineEditNameActivite = new QLineEdit(ui->tableWidgetServicesAdmin->item(ui->tableWidgetServicesAdmin->selectedRanges().first().topRow(),0)->text()); //nameActivite
        QLabel *labelTimeEdit = new QLabel("Temps (hh:mm:ss)");
        QString temps = ui->tableWidgetServicesAdmin->item(ui->tableWidgetServicesAdmin->selectedRanges().first().topRow(), 1)->text();
        QStringList tempsSplit = temps.split(' ');
        int heure = tempsSplit[0].split('h')[0].toInt();
        int minute = tempsSplit[1].split('m')[0].toInt();
        int seconde = tempsSplit[2].split('s')[0].toInt();
        QTimeEdit *timeEdit = new QTimeEdit(QTime(heure, minute, seconde));

        //QTimeEdit *timeEdit = new QTimeEdit;
        timeEdit->setDisplayFormat("hh:mm:ss");


        QPushButton *okButton = new QPushButton("OK");
        lineEditNameActivite->setValidator(validator);

        mainLayout->addWidget(labelNameActivity);
        mainLayout->addWidget(lineEditNameActivite);
        mainLayout->addWidget(labelTimeEdit);
        mainLayout->addWidget(timeEdit);



        mainLayout->addWidget(okButton);
        dialog->setLayout(mainLayout);
        dialog->connect(okButton, &QPushButton::clicked, [&](){ //When validation button was pressed
            if (!lineEditNameActivite->text().isEmpty()&&timeEdit->time().toString()!="00:00:00")
            {
                QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName(m_databasePath);

                if (!db.open()) {
                    qDebug() << "Erreur lors de l'ouverture de la base de données (update service) : " << db.lastError().text() << '\n';
                }
                QString queryStr = QString("UPDATE titem SET NameItem = '%1', Time = %2, OrdreItem = %3, fk_titem_tservice = %4 WHERE fk_titem_tservice = %5 AND OrdreItem = %6").arg(lineEditNameActivite->text()).arg(QString::number(timeEdit->time().hour()*3600+timeEdit->time().minute()*60+timeEdit->time().second())).arg(QString::number(ui->tableWidgetServicesAdmin->rowCount())).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)).arg(QString::number(ui->tableWidgetServicesAdmin->rowCount()));
                qDebug()<<queryStr;
                            QSqlQuery query(queryStr);
                            if(!query.exec()){
                                qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
                            }
                            else
                            {
                                qDebug() << "Ligne mis a jour avec succès" << '\n';
                            }


                    // Fermeture de la connexion

                    db.close();
                    db.removeDatabase("QSQLITE");
                    dialog->accept();
                    UpdateService(ui->comboBoxServicesAdmin->currentIndex()+1);
             }
            else {
                if (lineEditNameActivite->text().isEmpty())
                {
                    QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom d'activité"));
                }
                if (timeEdit->time().toString()=="00:00:00")
                {
                    QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un temps"));
                }
            }
        });

        dialog->exec();
    }
    else {
            QMessageBox::critical(this,tr("Erreur"), tr("Veuillez ne séléctionnez qu'une seule ligne a modifier"));

         }
}


void MainWindow::on_ButtonSupprimer_clicked()
{
    // Récupérer les plages sélectionnées
         QList<QTableWidgetSelectionRange> selectionRanges = ui->tableWidgetServicesAdmin->selectedRanges();
    if(!selectionRanges.isEmpty())
    {
        // Récupérer le numéro de ligne de la première case sélectionnée
        int firstRow = selectionRanges.first().topRow();
        // Récupérer le numéro de ligne de la dernière case sélectionnée
        int lastRow = selectionRanges.last().bottomRow();
        qDebug() << "1st row: " <<firstRow << " last row: "<< lastRow;
        QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_databasePath);

        if (!db.open()) {
            qDebug() << "Erreur lors de l'ouverture de la base de données (update service) : " << db.lastError().text() << '\n';
            return;
        }
        if (!db.isOpen()) {
            qDebug() << "La connexion à la base de données n'est pas ouverte. "<<db.lastError().text();
            db.close();
            db.removeDatabase("QSQLITE");
        }
        qDebug()<<QString::number(ui->comboBoxServicesAdmin->currentIndex()+1);
        qDebug()<<"DELETE FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)+" AND OrdreItem BETWEEN " + QString::number(firstRow+1) + " AND "+QString::number(lastRow+1);
        QSqlQuery query("DELETE FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)+" AND OrdreItem BETWEEN " + QString::number(firstRow+1) + " AND "+QString::number(lastRow+1));
                if(!query.exec()){
                    qDebug() << "Erreur lors de la supression du/des srvices : " << query.lastError().text() << '\n';
                    db.close();
                    db.removeDatabase("QSQLITE");
                    return;
                }

            // Fermeture de la connexion

            db.close();
            db.removeDatabase("QSQLITE");
            DecrementIdActivite();
            UpdateService(ui->comboBoxServicesAdmin->currentIndex()+1);
    }
    else
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez séléctionnez une ou plusieurs lignes a supprimer"));
    }
        return;
}

void MainWindow::DecrementIdActivite()
{
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_databasePath);
        if (!db.open())
        {
        qDebug() << "Impossible de se connecter a la base de donnees ";
        return;
        }

        QSqlQuery query;
        query.exec("SELECT * FROM titem");
        int idActivite = 0;
        while (query.next())
        {
        idActivite++;
        int currentIdActivite = query.value(0).toInt();
        if (idActivite != currentIdActivite)
        {
                    //L'id n'est pas le bon, on le corrige
                    QString updateQuery = QString("UPDATE titem SET IdItem = %1 WHERE IdItem = %2").arg(idActivite).arg(currentIdActivite);
                    query.exec(updateQuery);
        }
        }

        query.exec("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
        int ordreActivite=0;
        while (query.next())
        {
        ordreActivite++;
        int currentIdOrdre = query.value(3).toInt();
        if (ordreActivite != currentIdOrdre)
        {
                    //L'id n'est pas le bon, on le corrige
                    QString updateQuery = QString("UPDATE titem SET OrdreItem = %1 WHERE OrdreItem = %2 AND fk_titem_tservice = %3").arg(ordreActivite).arg(currentIdOrdre).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
                    qDebug()<<updateQuery;
                    if(!query.exec(updateQuery)){
                        qDebug() << "Erreur lors de la requête permettant de remettre en ordre l'ordre des items : " << query.lastError().text() << '\n';
                    }
                    else
                    {
                        qDebug() << "Ordres des items mis a jour avec succès" << '\n';
                    }

        }
        }
        db.close();
        db.removeDatabase("QSQLITE");
}

void MainWindow::on_ButtonAjouterCombo_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_databasePath);
        db.open();

        int maxIdService = 0;
        QSqlQuery query;
        query.prepare("SELECT MAX(IdService) FROM tservice");
        if (!query.exec())
        {
        qDebug() << "Erreur lors de la requête permettant de récupérer l'id maximum des services pour la création d'un nouveau service : " << query.lastError().text() << '\n';
        }
        else
        {
            if (query.next())
            {
                maxIdService = query.value(0).toInt();
            }
        }

        db.close();
        db.removeDatabase("QSQLITE");

        QRegExpValidator *validator = new QRegExpValidator(QRegExp("[A-Za-z0-9 éè!?.']*"));

        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Ajouter un service");
        QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
        QLabel *label = new QLabel("Nom du service :");
        QLineEdit *NameActivitelineEdit = new QLineEdit;



        QPushButton *okButton = new QPushButton("OK");



        NameActivitelineEdit->setValidator(validator);


        mainLayout->addWidget(label);
        mainLayout->addWidget(NameActivitelineEdit);
        mainLayout->addWidget(okButton);

        dialog->setLayout(mainLayout);
        dialog->connect(okButton, &QPushButton::clicked, [&](){ //When validation button was pressed
            if (!NameActivitelineEdit->text().isEmpty())
            {

                dialog->accept();

                QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Définir le type de la base de donnée
                db.setDatabaseName(m_databasePath); // Assigner le chemin à la variable de la base de donnée
                db.open(); // Ouvrir la base de données

                QSqlQuery query; // Créer un objet de type QSqlQuery
                query.prepare("INSERT INTO tservice (IdService, NameService) VALUES(" + QString::number(maxIdService+1) + ",'" + NameActivitelineEdit->text() + "')"); // Préparer la requête
                if(query.exec()){ // Executer la requête
                    qDebug() << "Nouveau service ajouté avec succès"; // Afficher un message de succès
                    UpdateSequence(ui->comboBoxServicesAdmin->currentIndex());
                    ui->comboBoxServicesAdmin->setCurrentIndex(ui->comboBoxServicesAdmin->count()-1);

                }
                else{
                    qDebug() << "Erreur lors de la création de la séquence" << query.lastError(); // Afficher un message d'erreur
                }
                db.close(); // Fermer la base de données
                db.removeDatabase("QSQLITE");
             }
            else {
                if (NameActivitelineEdit->text().isEmpty())
                {
                    QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom de séquence"));
                }

            }

        });


        dialog->exec();
}

void MainWindow::DecrementIdSequence()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(m_databasePath);
            if (!db.open())
            {
                qDebug() << "Impossible de se connecter a la base de donnees";
                return;
            }

            QSqlQuery query;
            query.exec("SELECT * FROM tservice");
            int idSequence = 0;
            while (query.next())
            {
                idSequence++;
                int currentIdSequence = query.value(0).toInt();
                if (idSequence != currentIdSequence)
                {
                    //L'id n'est pas le bon, on le corrige
                    QString updateQuery = QString("UPDATE tservice SET IdService = %1 WHERE IdService = %2").arg(idSequence).arg(currentIdSequence);
                    query.exec(updateQuery);
                }
            }
            db.close();
            db.removeDatabase("QSQLITE");
}

void MainWindow::on_pushButtonDeleteService_clicked()
{
    QMessageBox::StandardButton reply;
            reply = QMessageBox::question(nullptr, "Suppresion séquence", "Voulez-vous vraiment supprimer le service <b>"+ui->comboBoxServicesAdmin->currentText() +"</b> qui contient <b>" + QString::number(ui->tableWidgetServicesAdmin->rowCount())+ "</b> activités ?", QMessageBox::Yes|QMessageBox::No);
       if (reply == QMessageBox::Yes) {
           // Le bouton "Oui" a été cliqué
           // Récupérer les plages sélectionnées
               QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
               db.setDatabaseName(m_databasePath);

               if (!db.open()) {
                   qDebug() << "Erreur lors de l'ouverture de la base de données (supprimer sequence) : " << db.lastError().text() << '\n';
                   return;
               }
               qDebug()<<QString("DELETE FROM tservice WHERE IdService = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1);

               QSqlQuery query(QString("DELETE FROM tservice WHERE IdService = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1));
                       if(!query.exec()){
                           qDebug() << "Erreur lors de la supression du service: " << query.lastError().text() << '\n';
                           db.close();
                           db.removeDatabase("QSQLITE");
                           return;
                       }
                       QSqlQuery secondQuery(QString("DELETE FROM titem WHERE fk_titem_tservice = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1));
                       if(!secondQuery.exec()){
                           qDebug() << "Erreur lors de la suprression des activités liés au service supprimé: " << secondQuery.lastError().text() << '\n';
                           db.close();
                           db.removeDatabase("QSQLITE");
                           return;
                       }

                       QSqlQuery thirdQuery;
                            thirdQuery.prepare("UPDATE titem SET fk_titem_tservice = fk_titem_tservice - 1 WHERE fk_titem_tservice > ?");
                       thirdQuery.addBindValue(ui->comboBoxServicesAdmin->currentIndex()+1);
                            thirdQuery.exec();


                   // Fermeture de la connexion

                   db.close();
                   db.removeDatabase("QSQLITE");
                   DecrementIdSequence();
                   UpdateSequence(ui->comboBoxServicesAdmin->currentIndex()-1);
       }
       else {
           // Le bouton "Non" a été cliqué ou la boîte de dialogue a été fermée
       }
}

void MainWindow::ChangeLastSelectedIndexService(int index)
{
       QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
       db.setDatabaseName(m_databasePath);

       db.open();
       QString queryStr = QString("UPDATE tsettings SET ValueOfTheSettings = %1 WHERE IdSettings = 1").arg(index);
       qDebug()<<queryStr;
       QSqlQuery query(queryStr);
       if(!query.exec())
       {
            qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
       }
       else
       {
            qDebug() << "Changement de l'index du dernier service sélécitonner, changer avec succès." << '\n';
       }


       // Fermeture de la connexion

       db.close();
       db.removeDatabase("QSQLITE");
}

int MainWindow::GetSelectedIndexService()
{
       int index = 0;
       QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
       db.setDatabaseName(m_databasePath);

       db.open();
       QSqlQuery query("SELECT ValueOfTheSettings FROM tsettings WHERE IdSettings = 1");

       if(query.exec())
       {
            while(query.next())
            {
                           index = query.value(0).toInt();
            }
       }
       else
       {
            qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
       }

       // Fermeture de la connexion
       db.close();
       db.removeDatabase("QSQLITE");

       return index;
}
