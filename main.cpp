#include "qaudiodialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("QAudioPlayer");
    QCoreApplication::setOrganizationName("QAudioPlayer");
    QCoreApplication::setApplicationVersion("1.0");
    QApplication a(argc, argv);
    QAudioDialog w;
    w.show();
    return QApplication::exec();
}
