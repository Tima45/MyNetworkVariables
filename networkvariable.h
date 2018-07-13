#ifndef NETWORKVARIABLE_H
#define NETWORKVARIABLE_H

#include <QObject>
#include <QUdpSocket>


class AbstractNetworkVariable : public QObject
{
    Q_OBJECT
public:
    static void setPort(int port);

    explicit AbstractNetworkVariable(QObject *parent = 0);
    virtual ~AbstractNetworkVariable();


signals:
    void valueUpdated();
    void error(QString);

public slots:

private slots:
    static void handleMessage();
    virtual void decodeValue() = 0;

protected:
    static QList<AbstractNetworkVariable*> variables;
    static QUdpSocket *socket;
    static QUdpSocket *writeSocket;
    static int _port;

    static int variablesCounter;
    QString networkName;
    char *undecodedValue = nullptr;

};


template<typename T>
class NetworkVariable: public AbstractNetworkVariable
{

public:

    explicit NetworkVariable(QString networkName,QObject *parent = 0): AbstractNetworkVariable(parent)
    {
        this->networkName = networkName;
        variables.append(this);
    }

    ~NetworkVariable()
    {
        variables.removeOne(this);
    }

    void setValue(T value)
    {
        this->value = value;
    }

    T getValue()
    {
        return value;
    }

    void addClient(QString ip,int port)
    {
        QPair<QString,int> pair;
        pair.first = ip;
        pair.second = port;
        clients.append(pair);
    }

    void synchronize()
    {
        uint hash = qHash(networkName);
        char* hashName = new char[sizeof(uint)];
        hashName = (char*)&hash;

        char *bytesValue = new char[sizeof(value)];
        bytesValue = (char*)&value;

        for(int i = 0; i < clients.count(); i++){
            QByteArray byteName(hashName,sizeof(uint));
            QByteArray byteValue(bytesValue,sizeof(value));

            QByteArray datagram = byteName + ":" +byteValue;

            writeSocket->writeDatagram(datagram.data(),datagram.size(),QHostAddress(clients.at(i).first),clients.at(i).second);
        }
    }

private:
    T value;
    QList<QPair<QString,int>> clients;

    void decodeValue()
    {
        value = *((T*)undecodedValue);
        emit valueUpdated();
    }



};

#endif // NETWORKVARIABLE_H
