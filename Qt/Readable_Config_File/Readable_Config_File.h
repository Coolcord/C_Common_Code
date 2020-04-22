#ifndef READABLE_CONFIG_FILE_H
#define READABLE_CONFIG_FILE_H

#include <QFile>
#include <QMap>
#include <QString>

class Readable_Config_File {
public:
    Readable_Config_File();
    ~Readable_Config_File();
    bool Open(const QString &fileLocation);
    bool Open_Without_Loading(const QString &fileLocation);
    bool Is_Open();
    bool Discard_And_Close();
    bool Save_And_Close();
    bool Save();
    bool Reload();
    void Clear_Values();
    bool Rename_Identifier(const QString &oldIdentifier, const QString &newIdentifier);
    bool Get_Value(const QString &identifier, QString &value);
    bool Get_Value(const QString &identifier, bool &value);
    bool Get_Value(const QString &identifier, int &value);
    bool Get_Value(const QString &identifier, double &value);
    bool Set_Value(const QString &identifier, const QString &value);
    bool Set_Value(const QString &identifier, bool value);
    bool Set_Value(const QString &identifier, int value);
    bool Set_Value(const QString &identifier, double value);

private:
    bool Open_And_Load(const QString &fileLocation, bool load);
    void Close();
    bool Load();
    bool Parse_Line(const QString &line, QString &identifier, QString &value);

    QFile *file;
    QMap<QString, QString> *map;
    QString fileLocation;

    #ifdef Q_OS_WIN32
    const QString STRING_NEW_LINE = "\r\n"; //Qt's endl seems to use only \n for some reason... I'll look into this later
    #else
    const QString STRING_NEW_LINE = "\n";
    #endif
};

#endif // READABLE_CONFIG_FILE_H
