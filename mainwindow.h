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
    void administratorInterface();
    void closeButtonConfiguration();
    void settingsButtonConfiguration();
    int database();
public slots:
    void settings();
};

#endif // MAINWINDOW_H
