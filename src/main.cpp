#include <QCoreApplication>
#include <QTextStream>
#include "connection.h"

int errorSyntax() {
    QTextStream cerr(stderr);
    cerr << "Error Syntax: you need 2 or 3 args: <host> <port> <isSsl>?" << endl;
    return 1;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    if (args.size() != 3 && args.size() != 4)
        return errorSyntax();

    bool portOk = false;
    ushort port = args.at(2).toUShort(&portOk);
    if (!portOk)
        return errorSyntax();

    Connection c(args.size() == 4);
    c.startTcpConnection(args.at(1), port);

    QObject::connect(&c, &Connection::socketError, &c, &Connection::_log);
    QObject::connect(&c, &Connection::connected, &app, &QCoreApplication::quit);
    QObject::connect(&c, &Connection::closed, &app, &QCoreApplication::quit);

    return app.exec();
}
