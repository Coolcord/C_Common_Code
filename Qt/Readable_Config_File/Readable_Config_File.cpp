#include "Readable_Config_File.h"
#include <assert.h>
#include <iterator>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

Readable_Config_File::Readable_Config_File() {
    this->file = nullptr;
    this->map = new QMap<QString, QString>();
    this->fileLocation = QString();
}

Readable_Config_File::~Readable_Config_File() {
    this->Save_And_Close();
    delete this->map;
}

bool Readable_Config_File::Open(const QString &fileLocation) {
    return this->Open_And_Load(fileLocation, true);
}

bool Readable_Config_File::Open_Without_Loading(const QString &fileLocation) {
    return this->Open_And_Load(fileLocation, false);
}

bool Readable_Config_File::Is_Open() {
    return static_cast<bool>(this->file);
}

bool Readable_Config_File::Discard_And_Close() {
    if (!this->Is_Open()) return false;
    this->Close();
    return true;
}

bool Readable_Config_File::Save_And_Close() {
    bool success = this->Save();
    this->Discard_And_Close();
    return success;
}

bool Readable_Config_File::Save() {
    if (!this->Is_Open()) return false;
    if (!this->file->open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    QTextStream stream(this->file);
    for (QMap<QString, QString>::iterator iter = this->map->begin(); iter != this->map->end(); ++iter) {
        stream << iter.key() << "=" << iter.value() << endl;
    }
    this->file->close();
    return true;
}

bool Readable_Config_File::Reload() {
    if (!this->Is_Open()) return false;
    this->map->clear();
    return this->Load();
}

void Readable_Config_File::Clear_Values() {
    this->map->clear();
}

bool Readable_Config_File::Rename_Identifier(const QString &oldIdentifier, const QString &newIdentifier) {
    if (!this->Is_Open()) return false;
    QMap<QString, QString>::iterator iter = this->map->find(oldIdentifier);
    if (iter == this->map->end()) return false;
    this->map->insert(newIdentifier, iter.value());
    return true;
}

bool Readable_Config_File::Get_Value(const QString &identifier, QString &value) {
    if (!this->Is_Open()) return false;
    QMap<QString, QString>::iterator iter = this->map->find(identifier);
    if (iter == this->map->end()) return false;
    value = iter.value();
    return true;
}

bool Readable_Config_File::Get_Value(const QString &identifier, bool &value) {
    QString string = QString();
    if (!this->Get_Value(identifier, string)) return false;
    string = string.toLower();
    if (string == "true") {
        value = true;
        return true;
    } else if (string == "false") {
        value = false;
        return true;
    } else { //the value might be an int
        int intValue = 0;
        if (!this->Get_Value(identifier, intValue)) return false;
        value = static_cast<bool>(intValue);
        return true;
    }
}

bool Readable_Config_File::Get_Value(const QString &identifier, int &value) {
    QString string = QString();
    if (!this->Get_Value(identifier, string)) return false;
    bool valid = false;
    int tmpValue = string.toInt(&valid);
    if (!valid) return false;
    value = tmpValue;
    return true;
}

bool Readable_Config_File::Get_Value(const QString &identifier, double &value) {
    QString string = QString();
    if (!this->Get_Value(identifier, string)) return false;
    bool valid = false;
    double tmpValue = string.toDouble(&valid);
    if (!valid) return false;
    value = tmpValue;
    return true;
}

bool Readable_Config_File::Set_Value(const QString &identifier, const QString &value) {
    if (!this->Is_Open()) return false;
    this->map->insert(identifier, value);
    return true;
}

bool Readable_Config_File::Set_Value(const QString &identifier, bool value) {
    if (value) return this->Set_Value(identifier, QString("true"));
    else return this->Set_Value(identifier, QString("false"));
}

bool Readable_Config_File::Set_Value(const QString &identifier, int value) {
    return this->Set_Value(identifier, QString::number(value));
}

bool Readable_Config_File::Set_Value(const QString &identifier, double value) {
    return this->Set_Value(identifier, QString::number(value));
}

bool Readable_Config_File::Open_And_Load(const QString &fileLocation, bool load) {
    if (!QDir().mkpath(QFileInfo(fileLocation).path())) return false;
    if (this->Is_Open()) this->Save_And_Close();
    assert(this->map->isEmpty());
    this->file = new QFile(fileLocation);
    if (!load || (load && this->Load())) {
        this->fileLocation = fileLocation;
        return true;
    } else {
        this->Discard_And_Close();
        return false;
    }
}

void Readable_Config_File::Close() {
    assert(!this->file->isOpen());
    delete this->file;
    this->file = nullptr;
    this->map->clear();
}

bool Readable_Config_File::Load() {
    if (!this->file->open(QIODevice::ReadOnly)) return false;
    QTextStream stream(this->file);
    while (!stream.atEnd()) {
        QString identifier = QString(), value = QString();
        if (this->Parse_Line(stream.readLine(), identifier, value)) {
            this->map->insert(identifier, value);
        }
    }
    this->file->close();
    return true;
}

bool Readable_Config_File::Parse_Line(const QString &line, QString &identifier, QString &value) {
    QStringList strings = line.split('=');
    if (strings.size() <= 1 || strings.first().at(0) == '=') return false;
    QString tmpIdentifier = strings.first();
    strings.removeFirst(); //ignore the identifier
    value = QString();
    QTextStream stream(&value);
    for (QString string : strings) {
        stream << string << "=";
    }
    stream.flush();
    value.chop(1); //drop the last =
    identifier = tmpIdentifier;
    return true;
}
