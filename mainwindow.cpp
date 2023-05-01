/********************************************************************
    created:	2022-01-06
    last update: 2022-04-23
    file path:	mainwindow.cpp
    author:		Lucas Journoud
    copyright:	W.I.P

    purpose: 	file containing all the features:
    - Possibility to display the different services and their activities.
    - Possibility to launch a service.
    - Administrator interface allowing to modify the database associated to the service and activity

    todo:
    - increases the size of QLabel, QLineEdit and QPushButton
    - change the icon of the application
    - ctrl + f: W.I.P
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
    //TableWidget
    ui->tableWidgetServices->setColumnCount(2);
    ui->tableWidgetServices->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetServices->setShowGrid(false);
    ui->tableWidgetServices->setColumnWidth(0, 1000);
    ui->tableWidgetServices->setColumnWidth(1, 350);
    //TableWidgetServicesAdmin
    ui->tableWidgetServicesAdmin->setColumnCount(2);
    ui->tableWidgetServicesAdmin->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetServicesAdmin->setShowGrid(true);
    ui->tableWidgetServicesAdmin->setColumnWidth(0, 1000);
    ui->tableWidgetServicesAdmin->setColumnWidth(1, 500);
    //Create a new element and set its height
    QTableWidgetItem *pHeaderItem = new QTableWidgetItem("Nom de l'activité");
    pHeaderItem->setSizeHint(QSize(0, 40));
    ui->tableWidgetServicesAdmin->setHorizontalHeaderItem(0, pHeaderItem);
}

/*!
 * \brief MainWindow::ResizeRow Allows you to change the size of the lines in relation to the size of the content inside
 */
void MainWindow::ResizeRow()
{
    for(int nNumberOfRow=0; nNumberOfRow<ui->tableWidgetServices->rowCount(); nNumberOfRow++) //Browse each line
    {
        ui->tableWidgetServices->resizeRowToContents(nNumberOfRow);
        ui->tableWidgetServicesAdmin->resizeRowToContents(nNumberOfRow);
    }
}

/*!
 * \brief MainWindow::ConfigComboBox Allows you to configure the combobox
 */
void MainWindow::ConfigComboBox()
{
    //Preparation of the database
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, ConfigComboBox : " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return;
    }
    QSqlQuery query("SELECT NameService FROM tservice");
    if(!query.exec()) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, ConfigComboBox : " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        return;
    }
    //Display of results
    while (query.next())
    {
        ui->comboBoxServices->addItem(query.value(0).toString());
        ui->comboBoxServicesAdmin->addItem(query.value(0).toString());
    }
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
    return;
}

/*!
 * \brief MainWindow::CloseButtonConfiguration Allows you to configure the close button
 */
void MainWindow::CloseButtonConfiguration()
{
    ui->pushButtonClose->setIcon(QIcon(QPixmap(":/images/close.png")));
    ui->pushButtonClose->setIconSize(QSize(75,75));
    ui->pushButtonClose->setFlat(true);
    //Exit the application by pressing
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
    //Goes to the admin interface when you click on the button and enter the correct password
    QObject::connect(ui->pushButtonSettings, &QPushButton::clicked, this, &MainWindow::Settings);
}

/*!
 * \brief MainWindow::Settings Call function when you press the settings button to go to the admin interface if the right password is entered
 */
void MainWindow::Settings()
{
    //Hash and salt (randomly generated salt) of the password
    QRandomGenerator generator;
    quint32 saltValue = generator.generate();
    QByteArray hashedPassword = HashingAndSalling(GetPasswordOfArhm(), saltValue);
    //Configure the window to enter the password
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
    pDialogPassword->connect(pCancelButtonPassword, &QPushButton::clicked, this, [&, pDialogPassword]() //When you click on the cancel button
    {
        pDialogPassword->reject();
    });
    pDialogPassword->connect(pOkButtonPassword, &QPushButton::clicked, this, [&, pLineEditPassword,saltValue,hashedPassword,pDialogPassword]() //When you click on the ok button
    {
        //Hash of the entered text
        QString sEntryText = pLineEditPassword->text().toLower();
        QByteArray hashedEnterText = HashingAndSalling(sEntryText, saltValue);

        if(hashedEnterText==hashedPassword) //If the password entered is correct
        {
            pDialogPassword->accept();
            ui->stackedWidget->setCurrentIndex(1);
        }
        else //If the password entered is incorrect
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
    m_bInitialConfigurationIsDone=false;
    showFullScreen();
    m_bUpdate=true;
    m_bServiceIsStart=false;
    m_pValidatorName = new QRegExpValidator(QRegExp("[A-Za-z0-9 éèêàâæçôöùûüïî!?.,]*"));
    ResetServices();
    CloseButtonConfiguration();
    SettingsButtonConfiguration();
    TableView();
    #if _WIN32 //Window 32 and 64bit
        m_sDatabasePath="D://Program Files//Arhm//ServiceCOM-2023//BDD_ServiceCom.db";
    #elif __ANDROID__ //Android device
        m_sDatabasePath="/storage/emulated/0/Arhm/BDD_ServiceCom.db";
    #endif
    UpdateServices(GetLastSelectedIndexService());
    m_pEngine = new QTextToSpeech;
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
    if(m_bServiceIsStart) //If a service is started
    {
        StopServices();
        ResetServices();
    }
    if(m_bInitialConfigurationIsDone&&m_bUpdate) //If the initial configuration is completed and the update of the services enabled
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
    //Preparation of the database
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, UpdateActivity : " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return;
    }
    QSqlQuery query("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(nIndexOfServices_in));
    if(!query.exec()) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, UpdateActivity : " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        return;
    }
    //Display of results
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
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
    //Resize the Row
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
    m_pTimerNextSequence = new QTimer();
    m_pTimerSeconds = new QTimer();
    //When the service time is over
    m_pTimerNextSequence->connect(m_pTimerNextSequence, &QTimer::timeout, this, &MainWindow::ElapsedTime);
    //Every second
    m_pTimerSeconds->connect(m_pTimerSeconds, &QTimer::timeout, this, &MainWindow::Timer);
    //Launch timers
    m_pTimerNextSequence->start(0);
    m_pTimerSeconds->start(0);
}

/*!
 * \brief MainWindow::Timer Function call every second when the service is launched. Allows to decrement the timer
 */
void MainWindow::Timer()
{
    m_pTimerSeconds->setInterval(1000);
    m_nSecondsRemaining--;
    ui->tableWidgetServices->item(m_nService-1,1)->setText(QString("%1h %2m %3s").arg(m_nSecondsRemaining/3600).arg((m_nSecondsRemaining%3600)/60).arg(m_nSecondsRemaining%60));
}

/*!
 * \brief MainWindow::ElapsedTime Call function when the service time is over. Allows you to move to the next activity and turn your current line red
 */
void MainWindow::ElapsedTime() //If the time is over
{
    if(m_nService<=ui->tableWidgetServices->rowCount()-1) //If the service is not finished
    {
        if(m_nService!=0)
        {
            ColorRow(m_nService-1,COLOR_RED);
            ui->tableWidgetServices->item(m_nService-1,1)->setText("0h 0m 0s");
        }
        ReadServices();
    }
    else //If the last service activity is reached
    {
        StopServices();
        ColorRow(m_nService-1,COLOR_RED);
        ui->tableWidgetServices->item(m_nService-1,1)->setText("0h 0m 0s");
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
    QString sTimeToSplit = ui->tableWidgetServices->item(m_nService,1)->text(); //Split hours, minutes and seconds into 3 int
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
    for(int nCurrentColumn=0;nCurrentColumn<ui->tableWidgetServices->columnCount();nCurrentColumn++) //All the cells of the column
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
    m_pTimerNextSequence->stop();
    m_pTimerSeconds->stop();
    m_bServiceIsStart=false;
}

/*!
 * \brief MainWindow::on_pushButtonStartService_clicked Allows to launch the service if it is not launched and to go to the next activity if not
 */
void MainWindow::on_pushButtonStartService_clicked()
{
    if(ui->tableWidgetServices->rowCount()!=0) //If there is at least 1 activity in the department
    {
        if(!m_bServiceIsStart) //If the service is not started
        {
            m_bServiceIsStart=true;
            StartServices();
        }
        else //If the service is started
        {
            if(m_nService<=ui->tableWidgetServices->rowCount()-1) //If the service is not finished
            {
                ReadServices();
                ColorRow(m_nService-2,COLOR_GREEN);
            }
            else //If the last line is reached
            {
                StopServices();
                ColorRow(m_nService-1,COLOR_GREEN);
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
    int nMaxIdActivity= GetMaxIdActivity();

    QDialog *pDialog = new QDialog(this);
    pDialog->setWindowTitle("Ajouter une activité");
    QVBoxLayout *mainLayout = new QVBoxLayout(pDialog);
    QLabel *pLabelNameActivity = new QLabel("Nom d'activité");
    QLineEdit *pLineEditNameActivity = new QLineEdit;
    pLineEditNameActivity->setValidator(m_pValidatorName);
    QPushButton *pOkButton = new QPushButton("OK");
    QLabel *pLabelTime = new QLabel("Temps:");
    QTimeEdit *pTimeEdit = new QTimeEdit;
    pTimeEdit->setDisplayFormat("hh:mm:ss");
    mainLayout->addWidget(pLabelNameActivity);
    mainLayout->addWidget(pLineEditNameActivity);
    mainLayout->addWidget(pLabelTime);
    mainLayout->addWidget(pTimeEdit);
    mainLayout->addWidget(pOkButton);
    pDialog->connect(pOkButton, &QPushButton::clicked, pDialog,[&,pLineEditNameActivity, pTimeEdit, nMaxIdActivity]() //When validation button was pressed
    {
        if (!pLineEditNameActivity->text().isEmpty()&&pTimeEdit->time().toString()!="00:00:00") //If the user has entered an activity name and a time
        {
            pDialog->accept();
            //Preparation of the database
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(m_sDatabasePath);
            if (!db.open()) //Try to open the database
            {
                qDebug() << "Error when opening the database, AddActivity : " << db.lastError().text() << '\n';
                db.removeDatabase("QSQLITE");
                return;
            }
            QString sQuery = QString("INSERT INTO titem (IdItem, NameItem, Time, OrdreItem, fk_titem_tservice) VALUES(%1, '%2', %3, %4, %5)")
                                 .arg(QString::number(nMaxIdActivity+1),
                                      pLineEditNameActivity->text(),
                                      QString::number(pTimeEdit->time().hour()*3600+pTimeEdit->time().minute()*60+pTimeEdit->time().second()),
                                      QString::number(ui->tableWidgetServicesAdmin->rowCount()+1),QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
            QSqlQuery query; //W.I.P
            query.prepare(sQuery);
            if(!query.exec()) //Try to execute the query
            {
                qDebug() << "Error during the execution of the request, AddActivity : " << query.lastError().text() << '\n';
                //Closing the connection
                db.close();
                db.removeDatabase("QSQLITE");
                return;
            }
            UpdateActivity(ui->comboBoxServicesAdmin->currentIndex()+1);
            //Closing the connection
            db.close();
            db.removeDatabase("QSQLITE");
        }
        else
        {
            if (pLineEditNameActivity->text().isEmpty()) //If the user has not entered an activity name
            {
                QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom d'activité"));
            }
            if (pTimeEdit->time().toString()=="00:00:00") //If the user has not entered any time
            {
                QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un temps pour l'activité"));
            }
        }
    });
    ResizeRow();
    pDialog->exec();
}

/*!
 * \brief MainWindow::GetMaxIdActivity Function that returns the maximum id of the activities
 * \return The maximum id of the activities
 */
int MainWindow::GetMaxIdActivity()
{
    int nMaxIdActivity = 0;
    //Preparation of the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, GetMaxIdActivity : " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return 1;
    }
    QSqlQuery query("SELECT MAX(IdItem) FROM titem");
    if(!query.exec()) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, GetMaxIdActivity : " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        return 1;
    }
    //Take the results
    if (query.next())
    {
        nMaxIdActivity = query.value(0).toInt();
    }
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
    //return the max id activity
    return nMaxIdActivity;
}


/*!
 * \brief MainWindow::on_pushButtonModifyActivity_clicked Allows you to modify the selected activity
 */
void MainWindow::on_pushButtonModifyActivity_clicked()
{
    QList<QTableWidgetSelectionRange> selectionRanges = ui->tableWidgetServicesAdmin->selectedRanges();
    int nNumberOfRow=0;
    int nFirstRow=0;
    int nLastRow;
    if(!selectionRanges.isEmpty()) //If the user has selected the right lines
    {
        // Récupérer le numéro de ligne de la première case sélectionnée
        nFirstRow = selectionRanges.first().topRow();
        // Récupérer le numéro de ligne de la dernière case sélectionnée
        nLastRow = selectionRanges.last().bottomRow();
        nNumberOfRow=nLastRow-nFirstRow;
        nNumberOfRow++;
    }
    if(selectionRanges.isEmpty()) //If the user has not selected any lines
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez séléctionnez une ligne a modifier"));
    }
    else if(nNumberOfRow==1) //If the user has selected only one line
    {
        //Initialization of the dialog window
        QDialog *pDialog = new QDialog(this);
        pDialog->setWindowTitle("Modifier une activité");
        QVBoxLayout *pMainLayout = new QVBoxLayout(pDialog);
        QLabel *pLabelNameActivity = new QLabel("Nom d'activité");
        QLineEdit *pLineEditNameActivite = new QLineEdit(ui->tableWidgetServicesAdmin->item(nFirstRow,0)->text()); //nameActivite
        QLabel *pLabelTimeEdit = new QLabel("Temps (hh:mm:ss)");
        QString sTime = ui->tableWidgetServicesAdmin->item(nFirstRow, 1)->text();
        QStringList timeSplit = sTime.split(' ');
        int hours = timeSplit[0].split('h')[0].toInt();
        int minutes = timeSplit[1].split('m')[0].toInt();
        int seconds = timeSplit[2].split('s')[0].toInt();
        QTimeEdit *pTimeEdit = new QTimeEdit(QTime(hours, minutes, seconds));
        //QTimeEdit *timeEdit = new QTimeEdit;
        pTimeEdit->setDisplayFormat("hh:mm:ss");
        QPushButton *pOkButton = new QPushButton("OK");
        pLineEditNameActivite->setValidator(m_pValidatorName);
        pMainLayout->addWidget(pLabelNameActivity);
        pMainLayout->addWidget(pLineEditNameActivite);
        pMainLayout->addWidget(pLabelTimeEdit);
        pMainLayout->addWidget(pTimeEdit);
        pMainLayout->addWidget(pOkButton);
        pDialog->setLayout(pMainLayout);
        pDialog->connect(pOkButton, &QPushButton::clicked, pDialog,[&, pLineEditNameActivite, pTimeEdit, nFirstRow]() //When validation button was pressed
        {
            if (!pLineEditNameActivite->text().isEmpty()&&pTimeEdit->time().toString()!="00:00:00") //If the user has entered a text and a time
            {
                //Preparation of the database
                QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName(m_sDatabasePath);
                if (!db.open()) //Try to open the database
                {
                    qDebug() << "Error when opening the database, ModifyActivity : " << db.lastError().text() << '\n';
                    return;
                }
                /*
                QString sQuery = QString("UPDATE titem SET NameItem = '%1', Time = %2, OrdreItem = %3, fk_titem_tservice = %4 WHERE fk_titem_tservice = %4 AND OrdreItem = %3")
                                     .arg(pLineEditNameActivite->text())
                                     .arg(QString::number(pTimeEdit->time().hour()*3600+pTimeEdit->time().minute()*60+pTimeEdit->time().second()))
                                     .arg(QString::number(nFirstRow+1))
                                     .arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
                */


                QString sQuery = QString("UPDATE titem SET NameItem = '%1', Time = %2, OrdreItem = %3, fk_titem_tservice = %4 WHERE fk_titem_tservice = %4 AND OrdreItem = %3").arg(
                    pLineEditNameActivite->text(),
                    QString::number(pTimeEdit->time().hour()*3600+pTimeEdit->time().minute()*60+pTimeEdit->time().second()),
                    QString::number(nFirstRow+1),
                    QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));

                QSqlQuery query(sQuery);
                if(!query.exec()) //Try to execute the query
                {
                    qDebug() << "Error during the execution of the request, ModifyActivity : " << query.lastError().text() << '\n';
                    //Closing the connection
                    db.close();
                    db.removeDatabase("QSQLITE");
                    return;
                }
                //Closing the connection
                db.close();
                db.removeDatabase("QSQLITE");
                //Close the window and update the TableWidget
                pDialog->accept();
                UpdateActivity(ui->comboBoxServicesAdmin->currentIndex()+1);
            }
            else
            {
                if (pLineEditNameActivite->text().isEmpty()) //If the user has not entered any text for the activity name
                {
                    QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom d'activité"));
                }
                if (pTimeEdit->time().toString()=="00:00:00") //If the user has not entered any time
                {
                    QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un temps"));
                }
            }
        });
        pDialog->exec();
    }
    else //If the user has selected more than one line to modify
    {
        QMessageBox::critical(this,tr("Erreur"), tr("Veuillez ne séléctionnez qu'une seule ligne a modifier"));
    }
}

/*!
 * \brief MainWindow::on_pushButtonDeleteActivity_clicked Allows you to delete the selected activity
 */
void MainWindow::on_pushButtonDeleteActivity_clicked()
{
    //Retrieve the selected ranges
    QList<QTableWidgetSelectionRange> selectionRanges = ui->tableWidgetServicesAdmin->selectedRanges();
    if(!selectionRanges.isEmpty()) //If the user has selected lines
    {
        //Retrieve the line number of the first selected box
        int nFirstRow = selectionRanges.first().topRow();
        //Retrieve the line number of the last selected box
        int nLastRow = selectionRanges.last().bottomRow();
        //Preparation of the database
        QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_sDatabasePath);
        if (!db.open()) //Try to open the database
        {
            qDebug() << "Error when opening the database, DeleteActivity : " << db.lastError().text() << '\n';
            db.removeDatabase("QSQLITE");
            return;
        }
        QSqlQuery query("DELETE FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)+" AND OrdreItem BETWEEN " + QString::number(nFirstRow+1) + " AND "+QString::number(nLastRow+1));
        if(!query.exec()) //Try to execute the query
        {
            qDebug() << "Error during the execution of the request, DeleteActivity : " << query.lastError().text() << '\n';
            //Closing the connection
            db.close();
            db.removeDatabase("QSQLITE");
            return;
        }
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        //Put the right id for the activities and update the tableView
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
    //Preparation of the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, ReorganizesIdActivity (id): " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return;
    }
    QSqlQuery query("SELECT * FROM titem");
    int nIdActivity = 0;
    //For all the results of the query
    while (query.next()) //Try to execute the query
    {
        nIdActivity++;
        int nCurrentIdActivity = query.value(0).toInt();
        if (nIdActivity != nCurrentIdActivity) //The id is not the right one, we correct it
        {
            //L'id n'est pas le bon, on le corrige
            if(!query.exec(QString("UPDATE titem SET IdItem = %1 WHERE IdItem = %2").arg(nIdActivity).arg(nCurrentIdActivity))) //Try to execute the query
            {
                qDebug() << "Error during the execution of the request, ReorganizesIdActivity (id): " << query.lastError().text() << '\n';
                //Closing the connection
                db.close();
                db.removeDatabase("QSQLITE");
                return;
            }
            db.close();
            db.removeDatabase("QSQLITE");
            ReorganizesIdActivity();
        }
    }
    db.close();
    db.removeDatabase("QSQLITE");
    //Preparation of the database
    QSqlDatabase secondDb = QSqlDatabase::addDatabase("QSQLITE");
    secondDb.setDatabaseName(m_sDatabasePath);
    if (!secondDb.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, ReorganizesIdActivity (id): " << secondDb.lastError().text() << '\n';
        secondDb.removeDatabase("QSQLITE");
        return;
    }
    QSqlQuery secondQuery("SELECT * FROM titem");
    secondQuery.exec("SELECT * FROM titem WHERE fk_titem_tservice = "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1)); //W.I.P (pas besoin de exec)
    int nOrderActivity=0;
    while (secondQuery.next())     //For all the results of the query
    {
        nOrderActivity++;
        int currentIdOrder = secondQuery.value(3).toInt();
        if (nOrderActivity != currentIdOrder) //The order is not the right one, we correct it
        {
            QString sUpdateQuery = QString("UPDATE titem SET OrdreItem = %1 WHERE OrdreItem = %2 AND fk_titem_tservice = %3").arg(nOrderActivity).arg(currentIdOrder).arg(QString::number(ui->comboBoxServicesAdmin->currentIndex()+1));
            //W.I.P pas besoin du QString
            if(!secondQuery.exec(sUpdateQuery)) //Try to execute the query
            {
                qDebug() << "Error during the execution of the request, ReorganizesIdActivity (order): " << secondQuery.lastError().text() << '\n';
                //Closing the connection
                secondDb.close();
                secondDb.removeDatabase("QSQLITE");
                return;
            }
            secondDb.close();
            secondDb.removeDatabase("QSQLITE");
            ReorganizesIdActivity();
        }
    }
    //Closing the connection
    secondDb.close();
    secondDb.removeDatabase("QSQLITE");
}

/*!
 * \brief MainWindow::on_pushButtonAddService_clicked Allows you to create a new service
 */
void MainWindow::on_pushButtonAddService_clicked()
{
    int nMaxIdService = GetMaxIdService();

    //Initialization of the dialog window
    QDialog *pDialog = new QDialog(this);
    pDialog->setWindowTitle("Ajouter un service");
    QVBoxLayout *pMainLayout = new QVBoxLayout(pDialog);
    QLabel *pLabel = new QLabel("Nom du service");
    QLineEdit *pNameActivitelineEdit = new QLineEdit;
    QPushButton *pOkButton = new QPushButton("OK");
    pNameActivitelineEdit->setValidator(m_pValidatorName);
    pMainLayout->addWidget(pLabel);
    pMainLayout->addWidget(pNameActivitelineEdit);
    pMainLayout->addWidget(pOkButton);
    pDialog->setLayout(pMainLayout);
    pDialog->connect(pOkButton, &QPushButton::clicked, pDialog,[&, pNameActivitelineEdit, nMaxIdService]() //When validation button was pressed
    {
        if (!pNameActivitelineEdit->text().isEmpty()) //If the user has entered a name to the service
        {
            pDialog->accept();
            //Preparation of the database
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // Définir le type de la base de donnée
            db.setDatabaseName(m_sDatabasePath); // Assigner le chemin à la variable de la base de donné
            if (!db.open()) //Try to open the database
            {
                qDebug() << "Error when opening the database, ReorganizesIdActivity (id): " << db.lastError().text() << '\n';
                db.removeDatabase("QSQLITE");
                return;
            }
            QString sQuery("INSERT INTO tservice (IdService, NameService) VALUES(" + QString::number(nMaxIdService+1) + ",'" + pNameActivitelineEdit->text() + "')"); //W.I.P pas besoin du QString
            QSqlQuery query;
            if(!query.exec(sQuery)) //Try to execute the query
            {
                qDebug() << "Error during the execution of the request, AddService: " << query.lastError().text() << '\n';
                //Closing the connection
                db.close();
                db.removeDatabase("QSQLITE");
                return;
            }
            //Update Service
            UpdateServices(ui->comboBoxServicesAdmin->currentIndex());
            ui->comboBoxServicesAdmin->setCurrentIndex(ui->comboBoxServicesAdmin->count()-1);
            //Closing the connection
            db.close();
            db.removeDatabase("QSQLITE");
        }
        else //If the user has not entered a service name
        {
            QMessageBox::critical(this,tr("Erreur"), tr("Veuillez entrez un nom de service"));
        }
    });
    pDialog->exec();
}

/*!
 * \brief MainWindow::GetMaxIdService Function that returns the maximum id of the service
 * \return The maximum id of the service
 */
int MainWindow::GetMaxIdService()
{
    //Preparation of the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, GetMaxIdService: " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return 1;
    }
    int nMaxIdService = 0;
    QSqlQuery query("SELECT MAX(IdService) FROM tservice");
    if(!query.exec()) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, GetMaxIdService: " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        return 1;
    }
    //Take the results
    if (query.next())
    {
       nMaxIdService = query.value(0).toInt();
    }
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
    //return the max id service
    return nMaxIdService;
}

/*!
 * \brief MainWindow::ReorganizesIdServices Reorganize the id of the services in order to put them correctly in order is without hole
 */
void MainWindow::ReorganizesIdServices()
{
    //Preparation of the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
       qDebug() << "Error when opening the database, ReorganizesIdServices: " << db.lastError().text() << '\n';
       db.removeDatabase("QSQLITE");
       return;
    }
    QSqlQuery query;
    query.exec("SELECT * FROM tservice"); //W.I.P
    int nIdService = 0;
    while (query.next()) //For all the results of the query
    {
        nIdService++;
        int nCurrentIdService = query.value(0).toInt();
        if (nIdService != nCurrentIdService) //The id is not the right one, we correct it
        {
            QString sUpdateQuery = QString("UPDATE tservice SET IdService = %1 WHERE IdService = %2").arg(nIdService).arg(nCurrentIdService); //W.I.P pas besoin du QString
            if(!query.exec(sUpdateQuery)) //Try to execute the query
            {
                qDebug() << "Error during the execution of the request, GetMaxIdService: " << query.lastError().text() << '\n';
                //Closing the connection
                db.close();
                db.removeDatabase("QSQLITE");
                return;
            }
            db.close();
            db.removeDatabase("QSQLITE");
            ReorganizesIdServices();
        }
    }
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
}

/*!
 * \brief MainWindow::on_pushButtonDeleteService_clicked Delete the currently selected service
 */
void MainWindow::on_pushButtonDeleteService_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Suppresion service", "Voulez-vous vraiment supprimer le service <b>"+ui->comboBoxServicesAdmin->currentText() +"</b> qui contient <b>" + QString::number(ui->tableWidgetServicesAdmin->rowCount())+ "</b> activité(s) ?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) //If the user answered yes to the confirmation
    {
        //Preparation of the database
        QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_sDatabasePath);
        if (!db.open()) //Try to open the database
        {
            qDebug() << "Error when opening the database, DeleteService: " << db.lastError().text() << '\n';
            db.removeDatabase("QSQLITE");
            return;
        }
        //Delete the service
        QSqlQuery query(QString("DELETE FROM tservice WHERE IdService = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1));
        if(!query.exec()) //Try to execute the query
        {
            qDebug() << "Error during the execution of the request, DeleteService (service): " << query.lastError().text() << '\n';
            //Closing the connection
            db.close();
            db.removeDatabase("QSQLITE");
            return;
        }
        //Delete all the activites of the service
        QSqlQuery secondQuery(QString("DELETE FROM titem WHERE fk_titem_tservice = %1").arg(ui->comboBoxServicesAdmin->currentIndex()+1));
        if(!secondQuery.exec()) //Try to execute the query
        {
            qDebug() << "Error during the execution of the request, DeleteService (activity): " << query.lastError().text() << '\n';
            //Closing the connection
            db.close();
            db.removeDatabase("QSQLITE");
            return;
        }
        qDebug()<<"UPDATE titem SET fk_titem_tservice = fk_titem_tservice - 1 WHERE fk_titem_tservice > "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1);
        //Decrements the foreign key of the services being after the one delete
        QSqlQuery thirdQuery;
        if(!thirdQuery.exec("UPDATE titem SET fk_titem_tservice = fk_titem_tservice - 1 WHERE fk_titem_tservice > "+QString::number(ui->comboBoxServicesAdmin->currentIndex()+1))) //Try to execute the query
        {
            qDebug() << "Error during the execution of the request, DeleteService (update): " << query.lastError().text() << '\n';
            //Closing the connection
            db.close();
            db.removeDatabase("QSQLITE");
            return;
        }

        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        //Updates and reorganizes the id of the activities
        ReorganizesIdServices();
        ReorganizesIdActivity();
        UpdateServices(ui->comboBoxServicesAdmin->currentIndex()-1);
        ui->comboBoxServices->setCurrentIndex(0);
    }
}

/*!
 * \brief MainWindow::ChangeLastSelectedIndexService Changes the index of the last selected service in the database (for subsequent runs).
 * \param nIndex_in The index of the last selected service
 */
void MainWindow::ChangeLastSelectedIndexService(int nIndex_in)
{
    //Preparation of the database
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, ChangeLastSelectedIndexService: " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return;
    }
    QString sQuery = QString("UPDATE tsettings SET ValueOfTheSettings = %1 WHERE IdSettings = 1").arg(nIndex_in); //W.I.P pas besoin du QString
    QSqlQuery query;
    if(!query.exec(sQuery)) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, DeleteService (service): " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        return;
    }
    //Closing the connection
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
    //Preparation of the database
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, ChangeLastSelectedIndexService: " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return 1;
    }
    QSqlQuery query("SELECT ValueOfTheSettings FROM tsettings WHERE IdSettings = 1");
    if(!query.exec()) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, GetLastSelectedIndexService: " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        return 1;
    }
    //Take the results
    while(query.next())
    {
        nIndex_out = query.value(0).toInt();
    }
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
    //Return the index of the last selected service
    return nIndex_out;
}

/*!
 * \brief MainWindow::GetPasswordOfArhm Retrieves the password needed to access the admin interface from the database and returns it
 * \return sPassword_out The password allowing to go on the admin interface
 */
QString MainWindow::GetPasswordOfArhm()
{
    QString sPassword_out;
    //Preparation of the database
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_sDatabasePath);
    if (!db.open()) //Try to open the database
    {
        qDebug() << "Error when opening the database, GetPasswordOfArhm: " << db.lastError().text() << '\n';
        db.removeDatabase("QSQLITE");
        return "error";
    }
    QSqlQuery query("SELECT ValueOfTheSettings FROM tsettings WHERE IdSettings = 2");
    if(!query.exec()) //Try to execute the query
    {
        qDebug() << "Error during the execution of the request, GetLastSelectedIndexService: " << query.lastError().text() << '\n';
        //Closing the connection
        db.close();
        db.removeDatabase("QSQLITE");
        return "error";
    }
    //Take the results
    while(query.next())
    {
        sPassword_out = query.value(0).toString();
    }
    //Closing the connection
    db.close();
    db.removeDatabase("QSQLITE");
    //Return the password allowing to go on the admin interface
    return sPassword_out;
}
