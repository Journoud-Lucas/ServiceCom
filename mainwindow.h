#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "administratorinterface.h"
#include <QMainWindow>
#include <QtSql>
#include <iostream>
#include <QApplication>
#include <QPushButton>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextToSpeech>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    AdministratorInterface* administratorInterface;
    void ShowAdministratorInterface();
    void CloseButtonConfiguration();
    void SettingsButtonConfiguration();
    int Database();
    void InitialConfiguration();
    int UpdateService(int index);
    bool initialConfigurationIsDone();
    void StartSequence();
    void TableView();
    void ColorRow(int row);

private:
    Ui::MainWindow *ui;
    QTextToSpeech *m_engine;
    QSqlDatabase *m_db;
    bool initialConfigurationDone;
    int m_sequence;
    QTimer *m_timerNextSequence;

public slots:
    void Settings();

private slots:
    int on_comboBoxChoixService_9_currentIndexChanged(int index);
    void on_pushButtonStart_clicked();
    void ReadServices();

};

#endif // MAINWINDOW_H
