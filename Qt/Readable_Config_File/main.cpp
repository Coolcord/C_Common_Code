#include <QCoreApplication>
#include "Readable_Config_File.h"
#include <QtDebug>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Readable_Config_File configFile;
    configFile.Open("test.cfg");
    QString value;
    configFile.Get_Value("TEST1", value);
    qDebug() << "TEST1 was: " << value;
    configFile.Set_Value("TEST1", "123");
    configFile.Set_Value("TEST2", "321");
    configFile.Set_Value("TEST3", "333");
    configFile.Save_And_Close();
    return a.exec();
}
