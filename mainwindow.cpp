#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();


    temp = new NetworkVariable<double>("temp",this);
    flag = new NetworkVariable<bool>("flag",this);
    connect(temp,SIGNAL(valueUpdated()),this,SLOT(tempUpdated()));
    connect(flag,SIGNAL(valueUpdated()),this,SLOT(flagUpdated()));

    temp->addClient(otherIp,otherPort);
    flag->addClient(otherIp,otherPort);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSettings()
{
    QSettings s("settings.ini",QSettings::IniFormat);
    otherIp = s.value("otherIp").toString();
    otherPort = s.value("otherPort").toInt();
    port = s.value("port").toInt();

    ui->plainTextEdit->appendPlainText("Other:" + otherIp + ":" + QString::number(otherPort));
}




void MainWindow::on_checkBox_clicked(bool checked)
{
    flag->setValue(checked);
    flag->synchronize();
}

void MainWindow::on_doubleSpinBox_editingFinished()
{
    temp->setValue(ui->doubleSpinBox->value());
    temp->synchronize();
}

void MainWindow::tempUpdated()
{
    ui->doubleSpinBox->setValue(temp->getValue());
}

void MainWindow::flagUpdated()
{
    ui->checkBox->setChecked(flag->getValue());
}
