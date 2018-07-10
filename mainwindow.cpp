#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();

    setterSocket = new QUdpSocket(this);
    getterSocket = new QUdpSocket(this);

    getterSocket->bind(QHostAddress::Any,port);
    connect(getterSocket, SIGNAL(readyRead()),this, SLOT(readFromSocket()));
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

void MainWindow::readFromSocket()
{
    while (getterSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(getterSocket->pendingDatagramSize());
        getterSocket->readDatagram(datagram.data(), datagram.size());
        QString text = datagram;
        ui->plainTextEdit->appendPlainText("get " + text);
        if(!text.isEmpty()){
            QStringList list = text.split(":");
            if(list.count() == 2){
                if(list.at(0) == "checkBox"){
                    bool value = (list.at(1)=="true");
                    ui->checkBox->setChecked(value);
                }else if(list.at(0) == "doubleBox"){
                    double value = list.at(1).toDouble();
                    ui->doubleSpinBox->setValue(value);
                }
            }
        }
    }
}


void MainWindow::on_checkBox_clicked(bool checked)
{
    QByteArray datagram = "checkBox:";
    if(checked){
        datagram += "true";
    }else{
        datagram += "false";
    }
    setterSocket->writeDatagram(datagram.data(),datagram.size(),QHostAddress(otherIp),otherPort);
    ui->plainTextEdit->appendPlainText("set " + QString(datagram.data()));
}

void MainWindow::on_doubleSpinBox_editingFinished()
{
    double value = ui->doubleSpinBox->value();
    QByteArray datagram = "doubleBox:";
    datagram += QString::number(value);
    setterSocket->writeDatagram(datagram.data(),datagram.size(),QHostAddress(otherIp),otherPort);
    ui->plainTextEdit->appendPlainText("set " + QString(datagram.data()));
}
