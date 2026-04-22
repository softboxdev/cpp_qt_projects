#ifndef AGENT_H
#define AGENT_H

#include <QObject> //basic claa for all objects
#include <QLocalServer> //local server class
#include <QLocalSocket> // class for client connection to the local server

#include <QTimer> // 

class Agent : public QObject
{
	Q_OBJECT // QT macros !
    
public:
	explicit Agent(QObject *parent = nullptr);
	bool start();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onCollectMetrics();
    void onClientDisconnected();
private:
	void SendMetrics(QLocalSocket* client);
	Metrics collectMetrics(); // collect from filedescriptor /proc

	QLocalServer* m_server;
	QList<QLocalSocket*> m_clients;
	QTimer* m_timer;
	Metrics m_lastMetrics; // cache

	quint64 m_prevTotal = 0; // 0-18 00000000...
	quint64 m_prevIdle = 0;

};

#endif