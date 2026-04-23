#include <QCoreApplication>
#include "agent.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setApplicationName("MonitorAgent");
	Agent agent;
	if(!agent.start())
	{
		return 1;
	}
	qDebug() << "Agent is running ...";
	return a.exec();
}