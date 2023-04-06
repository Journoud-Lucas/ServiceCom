#ifndef ADMINISTRATORINTERFACE_H
#define ADMINISTRATORINTERFACE_H

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
class AdministratorInterface;
}

class AdministratorInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdministratorInterface(QWidget *parent = nullptr);
    ~AdministratorInterface();

private slots:
    int on_comboBoxService_currentIndexChanged(int index);

    void on_ButtonAjouter_clicked();

private:
    Ui::AdministratorInterface *ui;
    void ResetButtonAdmin();
    int ComboInit();
    int UpdateService(int indexNombre);
    bool initialConfigurationDone;
    bool initialConfigurationIsDone();
    QString m_databasePath;
    void ResizeRow();
};

#endif // ADMINISTRATORINTERFACE_H
