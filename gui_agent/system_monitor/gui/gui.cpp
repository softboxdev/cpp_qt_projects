#include "gui.h"
#include <QMessageBox>
#include <QDateTime>

Gui::Gui(QWidget *parent)
    : QMainWindow(parent)
    , m_socket(new QLocalSocket(this))
    , m_timer(new QTimer(this))
    , m_tcpServer(new QTcpServer(this))
{
    // Создаём центральный виджет
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    
    // Заголовок
    QLabel* title = new QLabel("System Monitor", this);
    title->setAlignment(Qt::AlignCenter);
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    title->setFont(titleFont);
    mainLayout->addWidget(title);
    
    // Метки для показа метрик
    m_cpuLabel = new QLabel("CPU: --%", this);
    m_ramLabel = new QLabel("RAM: --%", this);
    m_netLabel = new QLabel("Network: RX: -- MB, TX: -- MB", this);
    
    QFont labelFont;
    labelFont.setPointSize(12);
    m_cpuLabel->setFont(labelFont);
    m_ramLabel->setFont(labelFont);
    m_netLabel->setFont(labelFont);
    
    mainLayout->addWidget(m_cpuLabel);
    mainLayout->addWidget(m_ramLabel);
    mainLayout->addWidget(m_netLabel);
    
    // Лог-область
    m_logDisplay = new QTextEdit(this);
    m_logDisplay->setReadOnly(true);
    m_logDisplay->setMaximumHeight(200);
    mainLayout->addWidget(m_logDisplay);
    
    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* connectBtn = new QPushButton("Connect to Agent", this);
    QPushButton* refreshBtn = new QPushButton("Refresh Now", this);
    QPushButton* shutdownBtn = new QPushButton("Shutdown Agent", this);
    
    buttonLayout->addWidget(connectBtn);
    buttonLayout->addWidget(refreshBtn);
    buttonLayout->addWidget(shutdownBtn);
    mainLayout->addLayout(buttonLayout);
    
    // Статус бар
    m_statusLabel = new QLabel("Disconnected");
    statusBar()->addWidget(m_statusLabel);
    
    // Подключаем сигналы
    connect(connectBtn, &QPushButton::clicked, this, &Gui::connectToAgent);
    connect(refreshBtn, &QPushButton::clicked, this, &Gui::requestMetrics);
    connect(shutdownBtn, &QPushButton::clicked, [this]() {
        if (m_socket->state() == QLocalSocket::ConnectedState) {
            Message msg;
            msg.type = MsgType::Shutdown;
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out << msg;
            m_socket->write(block);
            m_logDisplay->append("Shutdown command sent to agent");
        }
    });
    
    connect(m_timer, &QTimer::timeout, this, &Gui::requestMetrics);
    
    startTcpServer();
    
    setWindowTitle("System Monitor");
    resize(600, 500);
}

Gui::~Gui()
{
    if (m_socket->state() == QLocalSocket::ConnectedState) {
        m_socket->disconnectFromServer();
    }
}

void Gui::connectToAgent()
{
    if (m_socket->state() == QLocalSocket::ConnectedState) return;
    
    m_socket->connectToServer("/tmp/monitor.sock");
    
    connect(m_socket, &QLocalSocket::connected, [this]() {
        m_statusLabel->setText("Connected to agent");
        m_logDisplay->append("Connected to agent");
        m_timer->start(1000);
        requestMetrics();
    });
    
    connect(m_socket, &QLocalSocket::readyRead, this, &Gui::onAgentData);
    connect(m_socket, &QLocalSocket::errorOccurred, this, &Gui::onAgentError);
}

void Gui::onAgentData()
{
    QDataStream stream(m_socket);
    Message msg;
    stream >> msg;
    
    if (msg.type == MsgType::MetricsData) {
        updateDisplay(msg.metrics);
    }
}

void Gui::onAgentError()
{
    m_statusLabel->setText("Error: " + m_socket->errorString());
    m_logDisplay->append("Connection error: " + m_socket->errorString());
    m_timer->stop();
}

void Gui::requestMetrics()
{
    if (m_socket->state() != QLocalSocket::ConnectedState) return;
    
    Message msg;
    msg.type = MsgType::GetMetrics;
    
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << msg;
    
    m_socket->write(block);
}

void Gui::updateDisplay(const Metrics& m)
{
    // Обновляем метки
    m_cpuLabel->setText(QString("CPU: %1%").arg(m.cpuPercent, 0, 'f', 1));
    m_ramLabel->setText(QString("RAM: %1%").arg(m.ramPercent, 0, 'f', 1));
    m_netLabel->setText(QString("Network: RX: %1 MB, TX: %2 MB")
        .arg(m.rxBytes / 1024.0 / 1024, 0, 'f', 2)
        .arg(m.txBytes / 1024.0 / 1024, 0, 'f', 2));
    
    // Добавляем в лог
    QString logEntry = QString("[%1] CPU: %2%, RAM: %3%")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(m.cpuPercent, 0, 'f', 1)
        .arg(m.ramPercent, 0, 'f', 1);
    
    m_logDisplay->append(logEntry);
    
    // Ограничиваем лог 100 строками
    if (m_logDisplay->document()->blockCount() > 100) {
        QTextCursor cursor(m_logDisplay->document());
        cursor.movePosition(QTextCursor::Start);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
    }
    
    m_statusLabel->setText(QString("CPU: %1% | RAM: %2%")
        .arg(m.cpuPercent, 0, 'f', 1)
        .arg(m.ramPercent, 0, 'f', 1));
}

void Gui::startTcpServer()
{
    if (!m_tcpServer->listen(QHostAddress::Any, 12345)) {
        m_logDisplay->append("TCP server failed to start: " + m_tcpServer->errorString());
        return;
    }
    
    connect(m_tcpServer, &QTcpServer::newConnection, this, &Gui::onTcpConnection);
    m_logDisplay->append("TCP command server listening on port 12345");
}

void Gui::onTcpConnection()
{
    QTcpSocket* client = m_tcpServer->nextPendingConnection();
    if (!client) return;
    
    connect(client, &QTcpSocket::readyRead, this, &Gui::onTcpCommand);
    connect(client, &QTcpSocket::disconnected, client, &QTcpSocket::deleteLater);
    
    m_tcpClients.append(client);
    m_logDisplay->append("TCP client connected from " + client->peerAddress().toString());
}

void Gui::onTcpCommand()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;
    
    QByteArray data = client->readAll();
    QString cmd = QString::fromUtf8(data).trimmed();
    
    parseCommand(cmd, client);
}

void Gui::parseCommand(const QString& cmd, QTcpSocket* client)
{
    if (cmd == "metrics") {
        requestMetrics();
        client->write("OK: Metrics requested\n");
        
    } else if (cmd == "status") {
        QString status = QString("Agent connected: %1\n")
            .arg(m_socket->state() == QLocalSocket::ConnectedState ? "yes" : "no");
        client->write(status.toUtf8());
        
    } else if (cmd == "quit") {
        client->write("OK: Shutting down GUI\n");
        client->flush();
        QTimer::singleShot(100, this, &QCoreApplication::quit);  // ← ТЕПЕРЬ РАБОТАЕТ
        
    } else {
        client->write("Unknown command. Available: metrics, status, quit\n");
    }
    
    client->flush();
}