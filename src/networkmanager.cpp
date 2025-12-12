#include "networkmanager.h"

#include <QDataStream>
#include <QHostAddress>

NetworkManager::NetworkManager(Role role, QObject *parent) : QObject(parent), m_role(role) {
    if (role == Role::Host) {
        connect(&m_server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);
    }
}

void NetworkManager::startListening(quint16 port) {
    if (m_role != Role::Host) {
        return;
    }
    m_server.listen(QHostAddress::Any, port);
}

void NetworkManager::connectToHost(const QString &host, quint16 port) {
    if (m_role != Role::Client) {
        return;
    }
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    m_socket->connectToHost(host, port);
}

void NetworkManager::sendMove(const Move &move) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("MOVE") << move.from << move.to;
    sendPacket(data);
}

void NetworkManager::sendDrawOffer() {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("DRAW");
    sendPacket(data);
}

void NetworkManager::sendDrawAccepted() {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QString("ACCEPT");
    sendPacket(data);
}

void NetworkManager::sendPacket(const QByteArray &data) {
    if (!m_socket) {
        return;
    }
    QDataStream stream(m_socket);
    stream << data;
    m_socket->flush();
}

void NetworkManager::onNewConnection() {
    m_socket = m_server.nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
}

void NetworkManager::onReadyRead() {
    if (!m_socket) {
        return;
    }
    QDataStream stream(m_socket);
    QByteArray payload;
    stream >> payload;
    QDataStream in(&payload, QIODevice::ReadOnly);
    QString type;
    in >> type;
    if (type == "MOVE") {
        Move move;
        in >> move.from >> move.to;
        emit moveReceived(move);
    } else if (type == "DRAW") {
        emit drawOffered();
    } else if (type == "ACCEPT") {
        emit drawAccepted();
    }
}
