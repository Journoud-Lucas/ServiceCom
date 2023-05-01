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
    - increases the size of QLabel and QLineEdit
    - change the icon of the application
*********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextToSpeech>
#include <QTimeEdit>
#include <QScrollBar>

const QColor COLOR_RED = QColor(237, 28, 36);
const QColor COLOR_GREEN = QColor(146,208,79);
const QColor COLOR_ORANGE = QColor(255, 165, 0);


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void CloseButtonConfiguration();
    void SettingsButtonConfiguration();
    void StartButtonConfiguration();
    void ConfigComboBox();
    void InitialConfiguration();
    void UpdateActivity(int nIndexOfServices_in);
    void UpdateServices(int nSelectedServices_in);
    void StartServices();
    void TableView();
    void ColorRow(int nRow_in, QColor color_in);
    void ReadServices();
    void StopServices();
    void ResetServices();
    void ReorganizesIdActivity();
    void ReorganizesIdActivityId();
    void ReorganizesIdActivityOrder();
    void ReorganizesIdServices();
    void ChangeLastSelectedIndexService(int nIndex_in);
    int GetMaxIdActivity();
    int GetMaxIdService();
    int GetLastSelectedIndexService();
    QString GetPasswordOfArhm();
    QByteArray HashingAndSalling(QString sTextToHash, quint32 saltValue);

private:
    Ui::MainWindow *ui;
    QTextToSpeech *m_pEngine;
    QRegExpValidator *m_pValidatorName;
    QTimer *m_pTimerNextSequence;
    QTimer *m_pTimerSeconds;
    QString m_sDatabasePath;
    bool m_bInitialConfigurationIsDone;
    bool m_bServiceIsStart;
    bool m_bUpdate;
    int m_nSecondsRemaining;
    int m_nService;


private slots:
    //Activity button
    void on_pushButtonAddActivity_clicked();
    void on_pushButtonModifyActivity_clicked();
    void on_pushButtonDeleteActivity_clicked();
    //Service button
    void on_pushButtonAddService_clicked();
    void on_pushButtonDeleteService_clicked();
    //Other button
    void on_pushButtonStartService_clicked();
    void on_pushButtonGoToMainWindow_clicked();
    //Index change
    void on_comboBoxServicesAdmin_currentIndexChanged(int nIndex_in);
    void on_comboBoxServices_currentIndexChanged(int nIndex_in);
    void ComboboxIndexChange(int nIndex_in);
    //Timer
    void Timer();
    void ElapsedTime();
    //Other
    void ResizeRow();
    void Settings();
};

#endif // MAINWINDOW_H
