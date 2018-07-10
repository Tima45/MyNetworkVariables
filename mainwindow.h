#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readSettings();


private:

    Ui::MainWindow *ui;

    QString otherIp;
    int otherPort = 0;
    int port = 0;
    QUdpSocket *getterSocket;
    QUdpSocket *setterSocket;

public slots:
    void readFromSocket();
private slots:
    void on_checkBox_clicked(bool checked);
    void on_doubleSpinBox_editingFinished();
};

#endif // MAINWINDOW_H
