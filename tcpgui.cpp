// GUI TCP Client and Server
// install Qt https://www.qt.io/download-dev
// Ensure you have a C++ compiler set up with Qt.

// tcp_server.cpp
#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <iostream>

class TcpServer : public QTcpServer {
    Q_OBJECT

public:
    TcpServer(QObject *parent = nullptr) : QTcpServer(parent) {
        connect(this, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    }

private slots:
    void onNewConnection() {
        QTcpSocket *clientSocket = nextPendingConnection();
        connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() {
            QTextStream stream(clientSocket);
            QString message = stream.readAll();
            std::cout << "Received: " << message.toStdString() << std::endl;
            clientSocket->write("Message received");
            clientSocket->flush();
        });
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    TcpServer server;
    if (!server.listen(QHostAddress::Any, 1234)) {
        std::cerr << "Server could not start!" << std::endl;
        return 1;
    }
    std::cout << "Server started on port 1234" << std::endl;
    return a.exec();
}

#include "tcp_server.moc"
