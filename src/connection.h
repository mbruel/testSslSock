#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>

QT_FORWARD_DECLARE_CLASS(QSslSocket)
QT_FORWARD_DECLARE_CLASS(QSslError)
QT_FORWARD_DECLARE_CLASS(QByteArray)


class Connection : public QObject
{
    Q_OBJECT

public:
    explicit Connection(bool ssl = false);
    ~Connection() override;

    void startTcpConnection(const QString &aHost, ushort aPort);

signals:
    void connected();
    void closed();

    void socketError(QString aError);

public slots:
    void onConnected();
    void onDisconnected();

    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

    void onEncryptedSocket();
    void onSslErrors(const QList<QSslError> &errors);

    void _log(const QString &msg) const;

protected:
    QTcpSocket *iSocket;           //!< Real TCP socket
    bool        isSsl;             //!< is the connection encrypted?
    bool        isConnected;       //!< to avoid to rely on iSocket && iSocket->isOpen()
};
#endif // CONNECTION_H
