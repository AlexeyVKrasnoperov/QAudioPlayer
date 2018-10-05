#include "qaudiodialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(QStringLiteral("QAudioPlayer"));
    QCoreApplication::setOrganizationName(QStringLiteral("QAudioPlayer"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));
    QApplication a(argc, argv);
    QAudioDialog w;
    w.show();
    return QApplication::exec();
}
