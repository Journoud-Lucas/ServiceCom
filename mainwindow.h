#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <regex>
#include <QTableWidget>
#include <QTimeEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ShowAdministratorInterface();
    void CloseButtonConfiguration();
    void SettingsButtonConfiguration();
    void PushButtonStartConfiguration();
    int ConfigComboBox();
    void InitialConfiguration();
    int UpdateService(int index);
    bool initialConfigurationIsDone();
    void StartSequence();
    void TableView();
    void ColorRow(int row, QColor);
    void ReadServices();
    void ResetServices();
    void StopSequence();
    void UpdateSequence();

private:
    Ui::MainWindow *ui;
    QTextToSpeech *m_engine;
    QSqlDatabase *m_db;
    bool initialConfigurationDone;
    int m_sequence;
    QTimer *m_timerNextSequence;
    QTimer *m_timerSeconds;
    int m_secondsRemaining;
    bool m_sequenceIsStart;
    QString m_databasePath;
    bool m_update;


public slots:
    void Settings();

private slots:
    void on_comboBoxChoixService_9_currentIndexChanged(int index);
    void on_pushButtonStart_clicked();
    void Timer();
    void ElapsedTime();
    void ResizeRow();

    void on_pushButton_clicked();
    void on_comboBoxService_currentIndexChanged(int index);
    void ComboboxChange(int index);

    void on_ButtonAjouter_clicked();
    void on_ButtonModifier_clicked();
};

#endif // MAINWINDOW_H
