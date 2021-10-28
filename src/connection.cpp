#include "connection.h"

#include <QSslSocket>
#include <QSslCertificate>

Connection::Connection(bool ssl):
    QObject(), isSsl(ssl), isConnected(false)
{}

Connection::~Connection(){
    if (iSocket && iSocket->isOpen())
        iSocket->disconnectFromHost();
    delete iSocket;
}

void Connection::startTcpConnection(const QString &aHost, ushort aPort){
    _log(tr("> Starting connection to %1:%2").arg(aHost).arg(aPort));

    iSocket = isSsl ? new QSslSocket() : new QTcpSocket();

    qRegisterMetaType<QAbstractSocket::SocketError>("SocketError" );
    connect(iSocket, &QAbstractSocket::connected, this, &Connection::onConnected);
    connect(iSocket, &QAbstractSocket::disconnected, this, &Connection::onDisconnected);
    connect(iSocket, &QIODevice::readyRead, this, &Connection::onReadyRead);
    connect(iSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    iSocket->connectToHost(aHost, aPort);
}

void Connection::onConnected()
{
    _log("> Connected to server");
    if (isSsl)
    {
        QSslSocket *sslSock = static_cast<QSslSocket*>(iSocket);
        connect(iSocket, SIGNAL(encrypted()), this, SLOT(onEncryptedSocket()));
        connect(iSocket, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(onSslErrors(QList<QSslError>)));
        _log("> Starting SSL Handshake");
        emit sslSock->startClientEncryption();
    }
    else {
        isConnected = true;
        emit connected();
    }
}

void Connection::onDisconnected()
{
    _log("> Disconnected");
    isConnected    = false;
    emit closed();
}


void Connection::onEncryptedSocket(){
    _log("> Socket encrypted!");
    isConnected = true;
    emit connected();
}

void Connection::onReadyRead()
{

    while (isConnected && iSocket->canReadLine())
    {
        QByteArray line = iSocket->readLine();
        _log(line);
    }
}

void Connection::onSslErrors(const QList<QSslError> &errors)
{
    QTextStream ostream(stderr);
    ostream << "Error SSL Socket:\n";
    for(int i=0;i<errors.size();i++)
        ostream << "\t- #" << errors[i].error() << " : " <<  errors[i].errorString() << "\n" << flush;

    if (errors.size() == 1 && errors.first().error() == QSslError::CertificateExpired){
        static_cast<QSslSocket*>(iSocket)->ignoreSslErrors();
        ostream << "Ignored SSL Certificate:\n";
        ostream << errors.first().certificate().toText();

        QList<QSslCertificate> certs = static_cast<QSslSocket*>(iSocket)->peerCertificateChain();
        ostream << "nb Certificates: " << certs.size() << "\n";
        for (QSslCertificate &c : certs)
            ostream << c.toText();

        ostream << flush;
    }
    else
        emit socketError("SSL error...");
}


void Connection::onError(QAbstractSocket::SocketError)
{
    emit socketError(tr("Error Socket: %1").arg(iSocket->errorString()));
}

void Connection::_log(const QString &msg) const
{
    QTextStream cout(stdout);
    cout << msg << endl << flush;
}
