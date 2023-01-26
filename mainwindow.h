#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "administratorinterface.h"

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

private:
    Ui::MainWindow *ui;
    void ShowAdministratorInterface();
    void CloseButtonConfiguration();
    void SettingsButtonConfiguration();
    int Database();
    void InitialConfiguration();

public slots:
    void Settings();

};

#endif // MAINWINDOW_H
