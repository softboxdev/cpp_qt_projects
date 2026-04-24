#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QLocalSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QCoreApplication>      // ← ДОБАВИТЬ
#include <QDateTime>              // ← ДОБАВИТЬ
#include "../common/protocol.h"

class Gui : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = nullptr);
    ~Gui();

private slots:
    void connectToAgent();
    void onAgentData();
    void onAgentError();
    void requestMetrics();
    void updateDisplay(const Metrics& m);
    void startTcpServer();
    void onTcpConnection();
    void onTcpCommand();

private:
    void parseCommand(const QString& cmd, QTcpSocket* client);
    
    QLocalSocket* m_socket;
    QTimer* m_timer;
    QTcpServer* m_tcpServer;
    QList<QTcpSocket*> m_tcpClients;
    
    // UI элементы
    QTextEdit* m_logDisplay;
    QLabel* m_cpuLabel;
    QLabel* m_ramLabel;
    QLabel* m_netLabel;
    QLabel* m_statusLabel;
};

#endif