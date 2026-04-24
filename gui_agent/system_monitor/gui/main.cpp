#include <QApplication>
#include "gui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("SystemMonitorGUI");
    
    Gui window;
    window.show();
    
    return a.exec();
}