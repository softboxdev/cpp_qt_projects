#include "agent.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "unistd.h"

Agent::Agent(QObject *parent) : QObject(parent) , m_server(new QLocalServer(this)), m_timer(new QTimer(this))
{
	connect(m_timer, &QTimer::timeout, this, &Agent::onCollectMetrics);

}
bool Agent::start()
{
	QLocalServer::removeServer("/tmp/monitor.sock");

	connect(m_server, &QLocalServer::newConnection, this, &Agent::onNewCollection);

	m_timer->start(2000);

	qDebug() << "Agent started on /tmp/monitor.sock";
	return true;

}
void Agent::onNewConnection()
{
	QLocalSocket* socket = m_server->nextPendingConnection();
	if(!socket) return;

	connect(socket, &QLocalSocket::readyRead, this, &Agent::onReadyRead);
	connect(socket, &QLocalSocket::disconnected, this, &Agent::onClientDisconnected);
    m_clients.append(socket);
    qDebug() << "GUI client connected";

}