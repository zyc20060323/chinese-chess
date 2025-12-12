#pragma once

#include "boardstate.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    enum class Role { Host, Client };

    NetworkManager(Role role, QObject *parent = nullptr);

    void startListening(quint16 port);
    void connectToHost(const QString &host, quint16 port);

    void sendMove(const Move &move);
    void sendDrawOffer();
    void sendDrawAccepted();

signals:
    void moveReceived(const Move &move);
    void drawOffered();
    void drawAccepted();

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    void sendPacket(const QByteArray &data);

    Role m_role;
    QTcpServer m_server;
    QTcpSocket *m_socket{nullptr};
};
