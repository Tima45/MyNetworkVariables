#include "networkvariable.h"
#include <typeinfo>



QUdpSocket * AbstractNetworkVariable::socket = nullptr;
QUdpSocket * AbstractNetworkVariable::writeSocket = nullptr;

QList<AbstractNetworkVariable*> AbstractNetworkVariable::variables;

int AbstractNetworkVariable::_port = -1;

int AbstractNetworkVariable::variablesCounter = 0;


AbstractNetworkVariable::AbstractNetworkVariable(QObject *parent) : QObject(parent)
{
    if(socket == nullptr){
        socket = new QUdpSocket(0);
        writeSocket = new QUdpSocket(0);
        if(_port != -1){
            if(socket->bind(QHostAddress::Any,_port)){
                connect(socket,&QUdpSocket::readyRead,handleMessage);
            }else{
                qDebug() << "AbstractNetworkVariable::AbstractNetworkVariable bind failed";
                emit error("AbstractNetworkVariable::AbstractNetworkVariable bind failed");
            }
        }else{
            qDebug() << "AbstractNetworkVariable::AbstractNetworkVariable bad port";
            emit error("AbstractNetworkVariable::AbstractNetworkVariable bad port");
        }
    }
    variablesCounter++;

}

AbstractNetworkVariable::~AbstractNetworkVariable()
{
    variablesCounter--;
    if(variablesCounter == 0){
        socket->deleteLater();
        socket = nullptr;
    }
}

void AbstractNetworkVariable::setPort(int port)
{
    _port = port;
}

void AbstractNetworkVariable::handleMessage()
{
    while (socket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        socket->readDatagram(datagram.data(), datagram.size());
        QByteArrayList list = datagram.split(':');
        if(list.count() == 2){
            int varId = -1;
            for(int i = 0; i < variables.count(); i++){
                uint hashName = *((uint*)list.at(0).data());
                if(qHash(variables.at(i)->networkName) == hashName){
                    varId = i;
                    break;
                }
            }
            if(varId == -1){
                qDebug() << "an unknown variable is received";
                return;
            }
            if(variables.at(varId)->undecodedValue == nullptr){
                variables.at(varId)->undecodedValue = new char[list.at(1).count()];
            }
            for(int i = 0; i < list.at(1).count(); i++){
                variables.at(varId)->undecodedValue[i] = list.at(1).data()[i];
            }
            variables.at(varId)->decodeValue();
        }else{
            qDebug() << "datagram.count is not 2";
        }
    }
}

