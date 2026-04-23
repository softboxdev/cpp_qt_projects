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
	//Когда клиент пришлет данные, вызовется onReadyRead()
	connect(socket, &QLocalSocket::disconnected, this, &Agent::onClientDisconnected);
    //Когда клиент отключится, вызовется `onClientDisconnected()`.
    m_clients.append(socket);
    // Добавляем новый сокет в список клиентов.
    qDebug() << "GUI client connected";

}
void Agent::onReadyRead()
{
	QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
	if(!socket) return;
	QDataStream stream(socket);
	Message msg;
	stream >> msg;

	if (msg.type == MsgType::GetMetrics) {
		sendMetrics(socket);
	} else if (msg.type == MsgType::Shutdown) {
		qDebug() << "Shutdown requested";
		QCoreApplication::quit();
	}
}
void Agent::sendMetrics(QLocalSocket* socket)
{
	Message response;
	response.type = MsgType::MetricsData;
	response.metrics = m_lastMetrics; // cache

	QByteArray block; // Создаем массив байт для передачи
	QDataStream out(&block, QIODevice::WriteOnly);
	out << response;
	socket->write(block);

}

Metrics Agent::collectMetrics()
{
	Metrics m;
	QFile statFile("/proc/stat");
	if(statFile.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&statFile);
		QString line = stream.readLine();
		if (line.startsWith("cpu ")){
			QStringList parts = line.split(' ', Qt::SkipEmptyParts);
			if(parts.size() >= 5) 
			{
				quint64 user = parts[1].toULongLong();
				quint64 nice = parts[2].toULongLong();
				quint64 system = parts[3].toULongLong();
				quint64 idle = parts[4].toULongLong();
				quint64 total = user + nice + system + idle;
				quint64 idleTime = idle;

				if(m_prevTotal > 0) 
				{
					quint64 totalDiff = total - m_prevTotal;
					quint64 idleDiff = idleTime - m_prevIdle;
					m.cpuPercent = 100.0 * (totalDiff - idleDiff) / totalDiff;
				}
				m_prevTotal = total;
				m_prevIdle = idleTime;
			}
		}
		statFile.close();
	}

	QFile memfile("/proc/meminfo");
	if(memFile.open(QIODevice::ReadOnly)) 
	{
		QTextStream stream(&memFile);
		quint64 memTotal = 0, memAvailable = 0;

		while(!stream.atEnd()) {
			QString line = stream.readLine();
			if(line.startsWith("MemTotal:")) 
			{
				memTotal = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong();
			} else if (line.startsWih("MemAvailable:"))
			{
				memAvailable = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong();
			}
		}
		if(memTotal > 0) 
		{
			m.ramPercent = 100.0 * (memTotal - memAvailable) / memTotal;
		}
		memFile.close();

	}
	QFile netFile("/proc/net/dev");
	if (netFile.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&netFile);
		quint64 totalRx = 0, totalTx = 0;

		while(!stream.atEnd()) 
		{
			QString line = stream.readLine();
			if(line.contains("eth") || line.contains("enp") || line.contains("wlan")) 
			{
				QStringLists parts  = line.split(' ', Qt::SkipEmptyParts);
				if(parts.size() >= 10) {
					totalRx += parts[1].toULongLong();
					totalTx += parts[9].toULongLong();
				}
			}

		}
		m.rxBytes = totalRx;
		m.txBytes = totalTx;
		netFile.close();


	}
	return m;
}

void Agent::onCollectMetrics()
{
	m_lastMetrics = collectMetrics();
	m_lastMetrics.timestamp = QDateTime::currentMSecsSinceEpoch();
	for(QLocalSocket* client : m_clients) 
	{
		sendMetrics(client);
	}
}

void Agent::onClientDisconnected()
{
	QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
	if(socket) 
	{
		m_clients.removeAll(socket);
		socket->deleteLater();
		qDebug() << "Gui client disconnected";
	}

}