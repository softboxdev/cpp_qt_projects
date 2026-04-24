#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QDataStream>
#include <QVector>
#include <QHash>

struct Metrics {
	double cpuPercent = 0.0;
	double ramPercent = 0.0;
	quint64 rxBytes = 0;
	quint64 txBytes = 0;
	quint64 timestamp = 0;
	// serializtion
	friend QDataStream& operator<<(QDataStream& stream, const Metrics& m){
		stream << m.cpuPercent << m.ramPercent << m.rxBytes << m.txBytes << m.timestamp;
		return stream;
	}
	friend QDataStream& operator>>(QDataStream& stream, Metrics& m){
		stream >> m.cpuPercent >> m.ramPercent >> m.rxBytes >> m.txBytes >> m.timestamp;
		return stream;
	}
};

enum class MsgType : quint32 {
	GetMetrics, // GUI -> Agent
	MetricsData, // Agent -> GUI
	Shutdown // GUI -> Agent
};

struct Message {
	MsgType type;
	Metrics metrics;

	friend QDataStream& operator<<(QDataStream& stream, const Message& msg) {
		stream << static_cast<quint32>(msg.type) << msg.metrics;
		return stream;
	}
	friend QDataStream& operator>>(QDataStream& stream, Message& msg) {
		quint32 temp;
		stream >> temp >> msg.metrics;
		msg.type = static_cast<MsgType>(temp);
		return stream;
	}


};

#endif