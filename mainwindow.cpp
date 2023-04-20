/********************************************************************
    created:	2022-01-06
    file path:	mainwindow.cpp
    author:		Lucas Journoud
    copyright:	W.I.P

    purpose: 	file containing all the features:
    - Possibility to display the different services and their activities.
    - Possibility to launch a service.
    - Administrator interface allowing to modify the database associated to the service and activity
*********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

/*!
 * \brief MainWindow::MainWindow Main window constructor
 * \param parent Parent of the window
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitialConfiguration();
}

/*!
 * \brief MainWindow::~MainWindow Main window destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * \brief MainWindow::TableView Initialize the 2 table views
 */
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
    ui->tableWidgetServicesAdmin->setColumnWidth(1, 250);
    // Créer un nouvel élément et régler sa hauteur
    QTableWidgetItem *pHeaderItem = new QTableWidgetItem("Nom item");
    pHeaderItem->setSizeHint(QSize(0, 40));
    // Régler l'élément comme horizontal header
    ui->tableWidgetServicesAdmin->setHorizontalHeaderItem(0, pHeaderItem);
}

/*!
 * \brief MainWindow::ResizeRow Allows you to change the size of the lines in relation to the size of the content inside
 */
void MainWindow::ResizeRow()
{
    for(int nNumberOfRow=0; nNumberOfRow<ui->tableWidgetServices->rowCount(); nNumberOfRow++)
    {
        ui->tableWidgetServices->resizeRowToContents(nNumberOfRow);
    }
    for(int nNumberOfRow=0; nNumberOfRow<ui->tableWidgetServicesAdmin->rowCount(); nNumberOfRow++)
    {
        ui->tableWidgetServicesAdmin->resizeRowToContents(nNumberOfRow);
    }
}

/*!
 * \brief MainWindow::ConfigComboBox Allows you to configure the combobox
 */
void MainWindow::ConfigComboBox()
{
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open())
    {
        qDebug() << "Erreur lors de l'ouverture de la base de données (config combobox) : " << db.lastError().text() << '\n';
        return;
    }
    QSqlQuery query("SELECT NameService FROM tservice");
    if(!query.exec())
    {
        qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
        db.close();
        return;
    }
    // Affichage des résultats
    while (query.next())
    {
        ui->comboBoxServices->addItem(query.value(0).toString());
        ui->comboBoxServicesAdmin->addItem(query.value(0).toString());
    }
    // Fermeture de la connexion
    db.close();
    db.removeDatabase("QSQLITE");
    return;
}

/*!
 * \brief MainWindow::CloseButtonConfiguration Allows you to configure the close button
 */
void MainWindow::CloseButtonConfiguration()
{
    //bouton fermer
    ui->pushButtonClose->setIcon(QIcon(QPixmap(":/images/close.png")));
    ui->pushButtonClose->setIconSize(QSize(75,75));
    ui->pushButtonClose->setFlat(true);
    QObject::connect(ui->pushButtonClose, &QPushButton::clicked, &QApplication::quit);
}

/*!
 * \brief MainWindow::SettingsButtonConfiguration Allows you to configure the settings button
 */
void MainWindow::SettingsButtonConfiguration()
{
    ui->pushButtonSettings->setIcon(QIcon(QPixmap(":/images/settings.png")));
    ui->pushButtonSettings->setIconSize(QSize(75,75));
    ui->pushButtonSettings->setFlat(true);
    QObject::connect(ui->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::Settings);
}

/*!
 * \brief MainWindow::Settings Call function when you press the settings button to go to the admin interface if the right password is entered
 */
void MainWindow::Settings()
{
    //Hashache et salage (salage générer aléatoirement) du mot de passe
    QRandomGenerator generator;
    quint32 saltValue = generator.generate();
    QByteArray hashedPassword = HashingAndSalling(GetPasswordOfArhm(), saltValue);
    QDialog *pDialogPassword = new QDialog(this);
    QVBoxLayout *pMainLayoutDialogPassword = new QVBoxLayout(pDialogPassword);
    QLabel *pLabelPassword = new QLabel("Entrez le mot de passe :");
    QLineEdit *pLineEditPassword = new QLineEdit;
    QPushButton *pOkButtonPassword = new QPushButton("OK");
    QPushButton *pCancelButtonPassword = new QPushButton("Annuler");
    pMainLayoutDialogPassword->addWidget(pLabelPassword);
    pMainLayoutDialogPassword->addWidget(pLineEditPassword);
    pMainLayoutDialogPassword->addWidget(pOkButtonPassword);
    pMainLayoutDialogPassword->addWidget(pCancelButtonPassword);
    pDialogPassword->setLayout(pMainLayoutDialogPassword);
    pDialogPassword->connect(pCancelButtonPassword, &QPushButton::clicked, [&]()
    {
        pDialogPassword->reject();
    });
    pDialogPassword->connect(pOkButtonPassword, &QPushButton::clicked, [&]() //When validation button was pressed
    {
        //Hashage du text
        QString sEntryText = pLineEditPassword->text().toLower();
        QByteArray hashedEnterText = HashingAndSalling(sEntryText, saltValue);

        if(hashedEnterText==hashedPassword) { //Corect password
            pDialogPassword->accept();
            ui->stackedWidget->setCurrentIndex(1);
        }
        else //Incorrect Password
        {
            QMessageBox::critical(this, tr("Erreur"), tr("Le mot de passe entré est incorect.\nVeuillez réessayez"));
        }
    });
    pDialogPassword->exec();
}

/*!
 * \brief HashingAndSalling Hasher and salt the password passed to it in parameter
 * \param sTextToHash Text to hasher
 * \param saltValue Salt to use
 * \return hashedPassword the asher password
 */
QByteArray MainWindow::HashingAndSalling(QString sTextToHash, quint32 saltValue)
{
    unsigned int saltUInt = static_cast<unsigned int>(saltValue);
    char salt = static_cast<char>(saltUInt);
    QByteArray passwordBytes = (sTextToHash+salt).toUtf8();
    QByteArray hashedPassword = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return hashedPassword;
}

/*!
 * \brief MainWindow::InitialConfiguration Function to set all elements to their initial configuration
 */
void MainWindow::InitialConfiguration()
{
    showFullScreen();
    m_bInitialConfigurationIsDone=false;
    m_bUpdate=true;
    m_bServiceIsStart=false;
    m_pValidatorName = new QRegExpValidator(QRegExp("[A-Za-z0-9 éèêàâæçôöùûüïî!?.,]*"));
    ResetServices();
    CloseButtonConfiguration();
    SettingsButtonConfiguration();
    TableView();
    #if _WIN32 //Window 32 and 64bit
        m_sDatabasePath="D://Users//Warp//Documents//arhm//BDD_ServiceCom.db";
    #elif __ANDROID__ // for Android device
        m_databasePath="/storage/emulated/0/Arhm/BDD_ServiceCom.db";
    #elif __APPLE__ //All apple device (use for ios)
        m_databasePath="W.I.P";
    #endif
    UpdateServices(GetLastSelectedIndexService());
    m_pEngine= new QTextToSpeech;
    m_bServiceIsStart=false;
    ui->stackedWidget->setCurrentIndex(0); //show the MainWindow
    m_bInitialConfigurationIsDone=true;
}

/*!
 * \brief MainWindow::StartButtonConfiguration Function to configure the start button
 */
void MainWindow::StartButtonConfiguration()
{
    ui->pushButtonStartService->setText("Lancer");
    ui->pushButtonStartService->setStyleSheet("QPushButton {background-color: #92d04f; color: white;}");
}

/*!
 * \brief MainWindow::on_comboBoxServices_currentIndexChanged Function that calls the ComboBoxIndexChange function when a service is changed in the combobox
 * \param nIndex_in Index of the new service select
 */
void MainWindow::on_comboBoxServices_currentIndexChanged(int nIndex_in)
{
    ComboboxIndexChange(nIndex_in);
}

/*!
 * \brief MainWindow::on_comboBoxServicesAdmin_currentIndexChanged Function to call when changing service in the combobox and which allows to call the necessary functions to update the tableWidget
 * \param nIndex_in Index of the service that will be updated
 */
void MainWindow::on_comboBoxServicesAdmin_currentIndexChanged(int nIndex_in)
{
    ComboboxIndexChange(nIndex_in);
}

/*!
 * \brief MainWindow::ComboboxIndexChange Function to call when changing service in the combobox and which allows to call the necessary functions to update the tableWidget
 * \param nIndex_in Index of the service that will be updated
 */
void MainWindow::ComboboxIndexChange(int nIndex_in)
{
    ui->comboBoxServicesAdmin->setCurrentIndex(nIndex_in);
    ui->comboBoxServices->setCurrentIndex(nIndex_in);
    if(m_bServiceIsStart)
    {
        StopServices();
        UpdateActivity(ui->comboBoxServices->currentIndex()+1);
        ResetServices();
    }
    if(m_bInitialConfigurationIsDone&&m_bUpdate)
    {
        UpdateActivity(nIndex_in+1);
        ChangeLastSelectedIndexService(nIndex_in);
    }
}

/*!
 * \brief MainWindow::UpdateActivity Allows to update (especially tableWidget) a service passed in parameter
 * \param IndexNombre_in The index of the service to be updated
 */
void MainWindow::UpdateActivity(int nIndexOfServices_in)
{
    ui->tableWidgetServices->setRowCount(0);
    ui->tableWidgetServicesAdmin->setRowCount(0);
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open())
    {
        qDebug() << "Erreur lors de l'ouverture de la base de données (update service) : " << db.lastError().text() << '\n';
        return;
    }
    QSqlQuery query("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(nIndexOfServices_in));
    if(!query.exec())
    {
        qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
        db.close();
        return;
    }
    // Affichage des résultats
    while (query.next())
    {
        ui->tableWidgetServices->setRowCount((ui->tableWidgetServices->rowCount())+1);
        ui->tableWidgetServicesAdmin->setRowCount((ui->tableWidgetServicesAdmin->rowCount())+1);
        ui->tableWidgetServices->setItem(query.value(3).toInt()-1,0,new QTableWidgetItem(query.value(1).toString())); //NomActiviter
        ui->tableWidgetServicesAdmin->setItem(query.value(3).toInt()-1,0,new QTableWidgetItem(query.value(1).toString())); //NomActiviter
        QString sTimeHoursMinutesSeconds = QString("%1h %2m %3s").arg(query.value(2).toInt()/3600).arg((query.value(2).toInt()%3600)/60).arg(query.value(2).toInt()%60);
        ui->tableWidgetServices->setItem(query.value(3).toInt()-1,1,new QTableWidgetItem(sTimeHoursMinutesSeconds)); //TempHeureMinuteSeconde
        ui->tableWidgetServicesAdmin->setItem(query.value(3).toInt()-1,1,new QTableWidgetItem(sTimeHoursMinutesSeconds)); //TempHeureMinuteSeconde
    }
    db.close();
    db.removeDatabase("QSQLITE");
    ResizeRow();
    return;
}

/*!
 * \brief MainWindow::StartServices Allows you to launch the service
 */
void MainWindow::StartServices()
{
    ui->pushButtonStartService->setStyleSheet("QPushButton {background-color: orange; color: white;}");
    ui->pushButtonStartService->setText("Continuer");
    // Timer pour le décompte
    m_pTimerNextSequence = new QTimer();
    m_pTimerSeconds = new QTimer();
    m_pTimerNextSequence->connect(m_pTimerNextSequence, &QTimer::timeout, this, &MainWindow::ElapsedTime);
    m_pTimerSeconds->connect(m_pTimerSeconds, &QTimer::timeout, this, &MainWindow::Timer);
    // Définir le timer pour exécuter la fonction toutes les 60 secondes
    m_pTimerNextSequence->start(0);
    m_pTimerSeconds->start(1000);
}

/*!
 * \brief MainWindow::Timer Function call every second when the service is launched. Allows to decrement the timer
 */
void MainWindow::Timer()
{
    m_nSecondsRemaining--;
    ui->tableWidgetServices->item(m_nService-1,1)->setText(QString("%1h %2m %3s").arg(m_nSecondsRemaining/3600).arg((m_nSecondsRemaining%3600)/60).arg(m_nSecondsRemaining%60));
}

/*!
 * \brief MainWindow::ElapsedTime Call function when the service time is over. Allows you to move to the next activity and turn your current line red
 */
void MainWindow::ElapsedTime() //Si le temps est écouler
{
    if(m_nService<=ui->tableWidgetServices->rowCount()-1) //Si la séquence n'est pas finie
    {
        if(m_nService!=0)
        {
            ColorRow(m_nService-1,COLOR_RED); //Mettre la ligne en rouge
            ui->tableWidgetServices->item(m_nService-1,1)->setText("0h 0m 0s"); //Mettre le temps a 0
        }
        ReadServices();
    }
    else //Si la dernière ligne de la séquence est atteinte
    {
        StopServices();
        ColorRow(m_nService-1,COLOR_RED); //Mettre la ligne en rouge
        ui->tableWidgetServices->item(m_nService-1,1)->setText("0h 0m 0s"); //Mettre le temps a 0s
        ResetServices();
    }
}

/*!
 * \brief MainWindow::ReadServices Allows to read orally the current service and to pass its orange line
 */
void MainWindow::ReadServices()
{    
    ColorRow(m_nService, COLOR_ORANGE); //Color actual row in orange
    m_pEngine->say(ui->tableWidgetServices->item(m_nService,0)->text()); //Say the actual row
    QString sTimeToSplit = ui->tableWidgetServices->item(m_nService,1)->text(); //Split hours, minutes and seconds into 2 int
    QStringList timeParts = sTimeToSplit.split(" ");
    int nHours = timeParts[0].remove("h").toInt();
    int nMinutes = timeParts[1].remove("m").toInt();
    int nSeconds = timeParts[2].remove("s").toInt();
    m_nSecondsRemaining=nHours*3600+nMinutes*60+nSeconds;
    //a convertir les hn les m et s du string en ms
    m_pTimerNextSequence->setInterval((nHours*3600+nMinutes*60+nSeconds)*1000); //Set Timer for the next sequence
    m_nService++;
}

/*!
 * \brief MainWindow::ColorRow Allows to change the color (pass as parameter) of the background line (pass as parameter) of the main QTableWidget
 * \param nRow_in Line to change the background color
 * \param color_in Color that will be applied to the background
 */
void MainWindow::ColorRow(int nRow_in, QColor color_in)
{
    for(int nCurrentColumn=0;nCurrentColumn<ui->tableWidgetServices->columnCount();nCurrentColumn++)
    {
        QTableWidgetItem *pItem = ui->tableWidgetServices->item(nRow_in, nCurrentColumn);
        pItem->setBackground(QBrush(color_in));
    }
}

/*!
 * \brief MainWindow::ResetServices Allows you to re-initialize services
 */
void MainWindow::ResetServices()
{
    m_nService=0;
    UpdateActivity(ui->comboBoxServices->currentIndex()+1);
    StartButtonConfiguration();
}

/*!
 * \brief MainWindow::StopServices Allows to stop the services
 */
void MainWindow::StopServices()
{
    m_pTimerNextSequence->stop(); //Arreter la sequence
    m_pTimerSeconds->stop();
    m_bServiceIsStart=false;
}

/*!
 * \brief MainWindow::on_pushButtonStartService_clicked Allows to launch the service if it is not launched and to go to the next activity if not
 */
void MainWindow::on_pushButtonStartService_clicked()
{
    if(ui->tableWidgetServices->rowCount()!=0)
    {
        if(!m_bServiceIsStart)
        {
            m_bServiceIsStart=true;
            StartServices();
        }
        else
        {
            if(m_nService<=ui->tableWidgetServices->rowCount()-1) //Si la séquence n'est pas finie
            {
                ReadServices();
                ColorRow(m_nService-2,COLOR_GREEN); //Mettre la ligne en verte
            }
            else //Si la dernière ligne est atteinte
            {
                StopServices();
                ColorRow(m_nService-1,COLOR_GREEN); //Mettre la ligne en verte
                UpdateActivity(ui->comboBoxServices->currentIndex()+1);
                ResetServices();
            }
        }
    }
}

/*!
 * \brief MainWindow::on_pushButtonGoToMainWindow_clicked Allows to return to the main interface
 */
void MainWindow::on_pushButtonGoToMainWindow_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/*!
 * \brief MainWindow::UpdateServices Update the combobox services
 * \param nSelectedSequence_in The service to select after updating the combobox
 */
void MainWindow::UpdateServices(int nSelectedSequence_in)
{
    m_bUpdate=false;
    ui->comboBoxServices->clear();
    ui->comboBoxServicesAdmin->clear();
    ConfigComboBox();
    m_bUpdate=true;
    ui->comboBoxServices->setCurrentIndex(nSelectedSequence_in);
    ui->comboBoxServicesAdmin->setCurrentIndex(nSelectedSequence_in);
    UpdateActivity(nSelectedSequence_in+1);
}

/*!
 * \brief MainWindow::on_pushButtonAddActivity_clicked Allows you to add a new activity to the current service
 */
void MainWindow::on_pushButtonAddActivity_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    db.open();
    int nMaxIdActivite = 0;
    QSqlQuery query;
    query.prepare("SELECT MAX(IdItem) FROM titem");
    if (query.exec())
    {
        if (query.next())
        {
            nMaxIdActivite = query.value(0).toInt();
        }
    }
    db.close();
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Ajouter une activité");
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QLabel *labelNameActivity = new QLabel("Nom d'activité :");
    QLineEdit *lineEditNameActivite = new QLineEdit;
    lineEditNameActivite->setValidator(m_pValidatorName);
    QPushButton *okButton = new QPushButton("OK");
    QLabel *labelTime = new QLabel("Temps:");
    QTimeEdit *timeEdit = new QTimeEdit;
    timeEdit->setDisplayFormat("hh:mm:ss");
    mainLayout->addWidget(labelNameActivity);
    mainLayout->addWidget(lineEditNameActivite);
    mainLayout->addWidget(labelTime);
    mainLayout->addWidget(timeEdit);
    mainLayout->addWidget(okButton);
    dialog->connect(okButton, &QPushButton::clicked, [&]() //When validation button was pressed
    {
        if (!lineEditNameActivite->text().isEmpty()&&timeEdit->time().toString()!="00:00:00")
        {
            dialog->accept();
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Définir le type de la base de donnée
            db.setDatabaseName(m_sDatabasePath); // Assigner le chemin à la variable de la base de donnée
            db.open(); // Ouvrir la base de données
            QString sQuery = QString("INSERT INTO titem (IdItem, NameItem, Time, OrdreItem, fk_titem_tservice) VALUES(%1, '%2', %3, %4, %5)").arg(QString::number(nMaxIdActivite+1)).arg(lineEditNameActivite->text()).arg(QString::number(timeEdit->time().hour()*3600+timeEdit->time().minute()*60+timeEdit->time().second())).arg(QString::number(ui->tableWidgetServicesAdmin->rowCount()+1)).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
            qDebug()<<sQuery;
            QSqlQuery query; // Créer un objet de type QSqlQuery
            query.prepare(sQuery); // Préparer la requête
            if(query.exec()) // Executer la requête
            {
                qDebug() << "Nouvelle ligne ajoutée avec succès"; // Afficher un message de succès
                UpdateActivity(ui->comboBoxServicesAdmin->currentIndex()+1);
            }
            else
            {
                qDebug() << "Erreur lors de l'ajout d'une nouvelle ligne"; // Afficher un message d'erreur
            }
            db.close(); // Fermer la base de données
            db.removeDatabase("QSQLITE");
        }
        else
        {
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

/*!
 * \brief MainWindow::on_pushButtonModifyActivity_clicked Allows you to modify the selected activity
 */
void MainWindow::on_pushButtonModifyActivity_clicked()
{
    QList<QTableWidgetSelectionRange> selectionRanges = ui->tableWidgetServicesAdmin->selectedRanges();
    int nNumberOfRow=0;
    int nFirstRow;
    int nLastRow;
    if(!selectionRanges.isEmpty())
    {
        // Récupérer le numéro de ligne de la première case sélectionnée
        nFirstRow = selectionRanges.first().topRow();
        // Récupérer le numéro de ligne de la dernière case sélectionnée
        nLastRow = selectionRanges.last().bottomRow();
        nNumberOfRow=nLastRow-nFirstRow;
        nNumberOfRow++;
    }
    if(selectionRanges.isEmpty())
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez séléctionnez une ligne a modifier"));
    }
    else if(nNumberOfRow==1)
    {
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Modifier une activité");
        QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
        QLabel *labelNameActivity = new QLabel("Nom d'activité :");
        QLineEdit *lineEditNameActivite = new QLineEdit(ui->tableWidgetServicesAdmin->item(ui->tableWidgetServicesAdmin->selectedRanges().first().topRow(),0)->text()); //nameActivite
        QLabel *labelTimeEdit = new QLabel("Temps (hh:mm:ss)");
        QString sTime = ui->tableWidgetServicesAdmin->item(ui->tableWidgetServicesAdmin->selectedRanges().first().topRow(), 1)->text();
        QStringList timeSplit = sTime.split(' ');
        int hours = timeSplit[0].split('h')[0].toInt();
        int minutes = timeSplit[1].split('m')[0].toInt();
        int seconds = timeSplit[2].split('s')[0].toInt();
        QTimeEdit *timeEdit = new QTimeEdit(QTime(hours, minutes, seconds));
        //QTimeEdit *timeEdit = new QTimeEdit;
        timeEdit->setDisplayFormat("hh:mm:ss");
        QPushButton *okButton = new QPushButton("OK");
        lineEditNameActivite->setValidator(m_pValidatorName);
        mainLayout->addWidget(labelNameActivity);
        mainLayout->addWidget(lineEditNameActivite);
        mainLayout->addWidget(labelTimeEdit);
        mainLayout->addWidget(timeEdit);
        mainLayout->addWidget(okButton);
        dialog->setLayout(mainLayout);
        dialog->connect(okButton, &QPushButton::clicked, [&]() //When validation button was pressed
        {
            if (!lineEditNameActivite->text().isEmpty()&&timeEdit->time().toString()!="00:00:00")
            {
                QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName(m_sDatabasePath);
                if (!db.open())
                {
                    qDebug() << "Erreur lors de l'ouverture de la base de données (update service) : " << db.lastError().text() << '\n';
                }
                QString sQuery = QString("UPDATE titem SET NameItem = '%1', Time = %2, OrdreItem = %3, fk_titem_tservice = %4 WHERE fk_titem_tservice = %5 AND OrdreItem = %6").arg(lineEditNameActivite->text()).arg(QString::number(timeEdit->time().hour()*3600+timeEdit->time().minute()*60+timeEdit->time().second())).arg(QString::number(ui->tableWidgetServicesAdmin->rowCount())).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)).arg(QString::number(ui->tableWidgetServicesAdmin->rowCount()));
                qDebug()<<sQuery;
                QSqlQuery query(sQuery);
                if(!query.exec())
                {
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
                UpdateActivity(ui->comboBoxServicesAdmin->currentIndex()+1);
            }
            else
            {
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
    else
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez ne séléctionnez qu'une seule ligne a modifier"));
    }
}

/*!
 * \brief MainWindow::on_pushButtonDeleteActivity_clicked Allows you to delete the selected activity
 */
void MainWindow::on_pushButtonDeleteActivity_clicked()
{
    // Récupérer les plages sélectionnées
    QList<QTableWidgetSelectionRange> selectionRanges = ui->tableWidgetServicesAdmin->selectedRanges();
    if(!selectionRanges.isEmpty())
    {
        // Récupérer le numéro de ligne de la première case sélectionnée
        int nFirstRow = selectionRanges.first().topRow();
        // Récupérer le numéro de ligne de la dernière case sélectionnée
        int nLastRow = selectionRanges.last().bottomRow();
        qDebug() << "1st row: " <<nFirstRow << " last row: "<< nLastRow;
        QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_sDatabasePath);
        if (!db.open())
        {
            qDebug() << "Erreur lors de l'ouverture de la base de données (update service) : " << db.lastError().text() << '\n';
            return;
        }
        if (!db.isOpen())
        {
            qDebug() << "La connexion à la base de données n'est pas ouverte. "<<db.lastError().text();
            db.close();
            db.removeDatabase("QSQLITE");
        }
        qDebug()<<QString::number(ui->comboBoxServicesAdmin->currentIndex()+1);
        qDebug()<<"DELETE FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)+" AND OrdreItem BETWEEN " + QString::number(nFirstRow+1) + " AND "+QString::number(nLastRow+1);
        QSqlQuery query("DELETE FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)+" AND OrdreItem BETWEEN " + QString::number(nFirstRow+1) + " AND "+QString::number(nLastRow+1));
        if(!query.exec())
        {
            qDebug() << "Erreur lors de la supression du/des srvices : " << query.lastError().text() << '\n';
            db.close();
            db.removeDatabase("QSQLITE");
            return;
        }
        // Fermeture de la connexion
        db.close();
        db.removeDatabase("QSQLITE");
        ReorganizesIdActivity();
        UpdateActivity(ui->comboBoxServicesAdmin->currentIndex()+1);
    }
    else
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez séléctionnez une ou plusieurs lignes a supprimer"));
    }
    return;
}

/*!
 * \brief MainWindow::ReorganizesIdActivity Reorganize the id of the activity in order to put them correctly in order is without hole
 */
void MainWindow::ReorganizesIdActivity()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open())
    {
        qDebug() << "Impossible de se connecter a la base de donnees ";
        return;
    }
    QSqlQuery query;
    query.exec("SELECT * FROM titem");
    int nIdActivity = 0;
    while (query.next())
    {
        nIdActivity++;
        int nCurrentIdActivity = query.value(0).toInt();
        if (nIdActivity != nCurrentIdActivity)
        {
            //L'id n'est pas le bon, on le corrige
            QString sUpdateQuery = QString("UPDATE titem SET IdItem = %1 WHERE IdItem = %2").arg(nIdActivity).arg(nCurrentIdActivity);
            query.exec(sUpdateQuery);
        }
    }
    query.exec("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
    int nOrderActivity=0;
    while (query.next())
    {
        nOrderActivity++;
        int currentIdOrder = query.value(3).toInt();
        if (nOrderActivity != currentIdOrder)
        {
            //L'id n'est pas le bon, on le corrige
            QString sUpdateQuery = QString("UPDATE titem SET OrdreItem = %1 WHERE OrdreItem = %2 AND fk_titem_tservice = %3").arg(nOrderActivity).arg(currentIdOrder).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
            if(!query.exec(sUpdateQuery))
            {
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

/*!
 * \brief MainWindow::on_pushButtonAddService_clicked Allows you to create a new service
 */
void MainWindow::on_pushButtonAddService_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    db.open();
    int nMaxIdService = 0;
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
            nMaxIdService = query.value(0).toInt();
        }
    }
    db.close();
    db.removeDatabase("QSQLITE");
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Ajouter un service");
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QLabel *label = new QLabel("Nom du service :");
    QLineEdit *NameActivitelineEdit = new QLineEdit;
    QPushButton *okButton = new QPushButton("OK");
    NameActivitelineEdit->setValidator(m_pValidatorName);
    mainLayout->addWidget(label);
    mainLayout->addWidget(NameActivitelineEdit);
    mainLayout->addWidget(okButton);
    dialog->setLayout(mainLayout);
    dialog->connect(okButton, &QPushButton::clicked, [&]() //When validation button was pressed
    {
        if (!NameActivitelineEdit->text().isEmpty())
        {
            dialog->accept();
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Définir le type de la base de donnée
            db.setDatabaseName(m_sDatabasePath); // Assigner le chemin à la variable de la base de donnée
            db.open(); // Ouvrir la base de données
            QSqlQuery query; // Créer un objet de type QSqlQuery
            query.prepare("INSERT INTO tservice (IdService, NameService) VALUES(" + QString::number(nMaxIdService+1) + ",'" + NameActivitelineEdit->text() + "')"); // Préparer la requête
            if(query.exec()) // Executer la requête
            {
                qDebug() << "Nouveau service ajouté avec succès"; // Afficher un message de succès
                UpdateServices(ui->comboBoxServicesAdmin->currentIndex());
                ui->comboBoxServicesAdmin->setCurrentIndex(ui->comboBoxServicesAdmin->count()-1);
            }
            else
            {
                qDebug() << "Erreur lors de la création de la séquence" << query.lastError(); // Afficher un message d'erreur
            }
            db.close(); // Fermer la base de données
            db.removeDatabase("QSQLITE");
        }
        else
        {
           if (NameActivitelineEdit->text().isEmpty())
           {
               QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom de séquence"));
           }
        }
    });
    dialog->exec();
}

/*!
 * \brief MainWindow::ReorganizesIdServices Reorganize the id of the services in order to put them correctly in order is without hole
 */
void MainWindow::ReorganizesIdServices()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open())
    {
        qDebug() << "Impossible de se connecter a la base de donnees";
        return;
    }
    QSqlQuery query;
    query.exec("SELECT * FROM tservice");
    int nIdService = 0;
    while (query.next())
    {
        nIdService++;
        int nCurrentIdService = query.value(0).toInt();
        if (nIdService != nCurrentIdService)
        {
            //L'id n'est pas le bon, on le corrige
            QString sUpdateQuery = QString("UPDATE tservice SET IdService = %1 WHERE IdService = %2").arg(nIdService).arg(nCurrentIdService);
            query.exec(sUpdateQuery);
        }
    }
    db.close();
    db.removeDatabase("QSQLITE");
}

/*!
 * \brief MainWindow::on_pushButtonDeleteService_clicked Delete the currently selected service
 */
void MainWindow::on_pushButtonDeleteService_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Suppresion séquence", "Voulez-vous vraiment supprimer le service <b>"+ui->comboBoxServicesAdmin->currentText() +"</b> qui contient <b>" + QString::number(ui->tableWidgetServicesAdmin->rowCount())+ "</b> activité(s) ?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        // Le bouton "Oui" a été cliqué
        // Récupérer les plages sélectionnées
        QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_sDatabasePath);
        if (!db.open())
        {
            qDebug() << "Erreur lors de l'ouverture de la base de données (supprimer sequence) : " << db.lastError().text() << '\n';
            return;
        }
        qDebug()<<QString("DELETE FROM tservice WHERE IdService = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1);
        QSqlQuery query(QString("DELETE FROM tservice WHERE IdService = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1));
        if(!query.exec())
        {
            qDebug() << "Erreur lors de la supression du service: " << query.lastError().text() << '\n';
            db.close();
            db.removeDatabase("QSQLITE");
            return;
        }
        QSqlQuery secondQuery(QString("DELETE FROM titem WHERE fk_titem_tservice = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1));
        if(!secondQuery.exec())
        {
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
        ReorganizesIdServices();
        UpdateServices(ui->comboBoxServicesAdmin->currentIndex()-1);
    }
}

/*!
 * \brief MainWindow::ChangeLastSelectedIndexService Changes the index of the last selected service in the database (for subsequent runs).
 * \param nIndex_in The index of the last selected service
 */
void MainWindow::ChangeLastSelectedIndexService(int nIndex_in)
{
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    db.open();
    QString sQuery = QString("UPDATE tsettings SET ValueOfTheSettings = %1 WHERE IdSettings = 1").arg(nIndex_in);
    qDebug()<<sQuery;
    QSqlQuery query(sQuery);
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

/*!
 * \brief MainWindow::GetLastSelectedIndexService Retrieve from the database the index of the last selected service in the database (for previous executions)
 * \return nIndex_out The index of the last selected service
 */
int MainWindow::GetLastSelectedIndexService()
{
    int nIndex_out = 0;
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    db.open();
    QSqlQuery query("SELECT ValueOfTheSettings FROM tsettings WHERE IdSettings = 1");
    if(query.exec())
    {
        while(query.next())
        {
            nIndex_out = query.value(0).toInt();
        }
    }
    else
    {
        qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
    }
    // Fermeture de la connexion
    db.close();
    db.removeDatabase("QSQLITE");
    return nIndex_out;
}
/*!
 * \brief MainWindow::GetPasswordOfArhm Retrieves the password needed to access the admin interface from the database and returns it
 * \return sPassword_out The password allowing to go on the admin interface
 */
QString MainWindow::GetPasswordOfArhm()
{
    QString sPassword_out = 0;
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    db.open();
    QSqlQuery query("SELECT ValueOfTheSettings FROM tsettings WHERE IdSettings = 2");
    if(query.exec())
    {
        while(query.next())
        {
            sPassword_out = query.value(0).toString();
        }
    }
    else
    {
        qDebug() << "Erreur lors de l'execution de la requête : " << query.lastError().text() << '\n';
    }
    // Fermeture de la connexion
    db.close();
    db.removeDatabase("QSQLITE");
    return sPassword_out;
}
