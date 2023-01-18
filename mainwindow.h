#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void AdministratorInterface();
    void CloseButtonConfiguration();
    void SettingsButtonConfiguration();
    int Database();
    void InitialConfiguration();
public slots:
    void Settings();
};

#endif // MAINWINDOW_H
