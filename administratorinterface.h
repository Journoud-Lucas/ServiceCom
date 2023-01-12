#ifndef ADMINISTRATORINTERFACE_H
#define ADMINISTRATORINTERFACE_H

#include <QMainWindow>

namespace Ui {
class AdministratorInterface;
}

class AdministratorInterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdministratorInterface(QWidget *parent = nullptr);
    ~AdministratorInterface();

private:
    Ui::AdministratorInterface *ui;
};

#endif // ADMINISTRATORINTERFACE_H
