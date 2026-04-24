#include "agent.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>              // ← ДОБАВИТЬ
#include <QCoreApplication>       // ← ДОБАВИТЬ

Agent::Agent(QObject *parent) 
    : QObject(parent)
    , m_server(new QLocalServer(this))
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &Agent::onCollectMetrics);
}

bool Agent::start()
{
    // Удаляем старый сокет
    QLocalServer::removeServer("/tmp/monitor.sock");
    
    // Запускаем сервер
    if (!m_server->listen("/tmp/monitor.sock")) {
        qCritical() << "Failed to start server:" << m_server->errorString();
        return false;
    }
    
    // ИСПРАВЛЕНО: onNewConnection (было onNewCollection)
    connect(m_server, &QLocalServer::newConnection, this, &Agent::onNewConnection);
    
    // Сбор метрик каждые 2 секунды
    m_timer->start(2000);
    
    qDebug() << "Agent started on /tmp/monitor.sock";
    return true;
}

void Agent::onNewConnection()
{
    QLocalSocket* socket = m_server->nextPendingConnection();
    if (!socket) return;
    
    connect(socket, &QLocalSocket::readyRead, this, &Agent::onReadyRead);
    connect(socket, &QLocalSocket::disconnected, this, &Agent::onClientDisconnected);
    
    m_clients.append(socket);
    qDebug() << "GUI client connected";
}

void Agent::onReadyRead()
{
    QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) return;
    
    QDataStream stream(socket);
    Message msg;
    stream >> msg;
    
    if (msg.type == MsgType::GetMetrics) {
        sendMetrics(socket);
    } else if (msg.type == MsgType::Shutdown) {
        qDebug() << "Shutdown requested";
        QCoreApplication::quit();  // ← ТЕПЕРЬ РАБОТАЕТ (добавлен include)
    }
}

void Agent::sendMetrics(QLocalSocket* socket)
{
    Message response;
    response.type = MsgType::MetricsData;
    response.metrics = m_lastMetrics;
    
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << response;
    
    socket->write(block);
}

void Agent::onCollectMetrics()
{
    m_lastMetrics = collectMetrics();
    m_lastMetrics.timestamp = QDateTime::currentMSecsSinceEpoch();  // ← ТЕПЕРЬ РАБОТАЕТ
    
    for (QLocalSocket* client : m_clients) {
        sendMetrics(client);
    }
}

Metrics Agent::collectMetrics()
{
    Metrics m;
    
    // 1. CPU usage из /proc/stat
    QFile statFile("/proc/stat");
    if (statFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&statFile);
        QString line = stream.readLine();
        if (line.startsWith("cpu ")) {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 5) {
                quint64 user = parts[1].toULongLong();
                quint64 nice = parts[2].toULongLong();
                quint64 system = parts[3].toULongLong();
                quint64 idle = parts[4].toULongLong();
                
                quint64 total = user + nice + system + idle;
                quint64 idleTime = idle;
                
                if (m_prevTotal > 0) {
                    quint64 totalDiff = total - m_prevTotal;
                    quint64 idleDiff = idleTime - m_prevIdle;
                    if (totalDiff > 0) {
                        m.cpuPercent = 100.0 * (totalDiff - idleDiff) / totalDiff;
                    }
                }
                
                m_prevTotal = total;
                m_prevIdle = idleTime;
            }
        }
        statFile.close();
    }
    
    // 2. RAM из /proc/meminfo
    QFile memFile("/proc/meminfo");  // ← ИСПРАВЛЕНО: memFile (было memfile в одном месте)
    if (memFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&memFile);
        quint64 memTotal = 0, memAvailable = 0;
        
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.startsWith("MemTotal:")) {
                memTotal = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong();
            } else if (line.startsWith("MemAvailable:")) {  // ← ИСПРАВЛЕН
                memAvailable = line.split(' ', Qt::SkipEmptyParts)[1].toULongLong();
            }
        }
        
        if (memTotal > 0) {
            m.ramPercent = 100.0 * (memTotal - memAvailable) / memTotal;
        }
        memFile.close();
    }
    
    // 3. Сеть из /proc/net/dev
    QFile netFile("/proc/net/dev");
    if (netFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&netFile);
        quint64 totalRx = 0, totalTx = 0;
        
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.contains("eth") || line.contains("enp") || line.contains("wlan")) {
                QStringList parts = line.split(' ', Qt::SkipEmptyParts);  // ← ИСПРАВЛЕНО
                if (parts.size() >= 10) {
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

void Agent::onClientDisconnected()
{
    QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
        qDebug() << "GUI client disconnected";
    }
}