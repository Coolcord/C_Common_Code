#include "Git_Update_Checker.h"
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

QString Git_Update_Checker::Check_For_Updates(const QString &currentVersion, const QString &remoteUrl) {
    return this->Check_For_Updates(currentVersion, remoteUrl, QString());
}

QString Git_Update_Checker::Check_For_Updates(const QString &currentVersion, const QString &remoteUrl, QString gitLocation) {
    if (currentVersion.contains("dev")) return QString(); //don't check for updates in development builds
    int significantVersion = 0, majorVersion = 0, minorVersion = 0, patchVersion = 0;
    assert(this->Get_Version_Numbers_From_String(currentVersion, significantVersion, majorVersion, minorVersion, patchVersion));

    //Run Git to check the remote server for updates
    if (!QFileInfo(gitLocation).exists() && !QFileInfo(gitLocation+".exe").exists()) gitLocation = "git"; //run system version
    QProcess process;
    QStringList arguments;
    process.setProgram(gitLocation);
    arguments << "ls-remote" << "--tags" << remoteUrl;
    process.setArguments(arguments);
    process.start(process.program(), process.arguments());
    process.waitForFinished(-1);
    QByteArray bytes = process.readAllStandardOutput();
    if (bytes.isEmpty()) return QString(); //cannot connect to the server

    //Check for a new version based upon the output
    QString newVersion = QString();
    QTextStream stream(bytes);
    while (!stream.atEnd()) {
        int newSignificantVersion = 0, newMajorVersion = 0, newMinorVersion = 0, newPatchVersion = 0;
        bool thisVersionIsNewer = false;
        QString line = stream.readLine();
        QStringList strings = line.split("refs/tags/v");
        if (strings.size() != 2) continue;
        if (!this->Get_Version_Numbers_From_String(strings.at(1), newSignificantVersion, newMajorVersion, newMinorVersion, newPatchVersion)) continue;

        //Compare Versions
        if (newSignificantVersion > significantVersion) thisVersionIsNewer = true;
        else if (newSignificantVersion < significantVersion) continue;
        else if (newMajorVersion > majorVersion) thisVersionIsNewer = true;
        else if (newMajorVersion < majorVersion) continue;
        else if (newMinorVersion > minorVersion) thisVersionIsNewer = true;
        else if (newMinorVersion < minorVersion) continue;
        else if (newPatchVersion > patchVersion) thisVersionIsNewer = true;
        else if (newPatchVersion < patchVersion) continue;

        //This version is the newest so far, so save it
        if (thisVersionIsNewer) {
            newVersion = strings.at(1);
            assert(this->Get_Version_Numbers_From_String(newVersion, significantVersion, majorVersion, minorVersion, patchVersion));
        }
    }
    return newVersion;
}

bool Git_Update_Checker::Get_Version_Numbers_From_String(const QString &version, int &significantVersion, int &majorVersion, int &minorVersion, int &patchVersion) {
    //Read the patch version first
    QStringList numbers = version.split('-');
    if (numbers.size() < 1 || numbers.size() > 2) return false;
    bool valid = false;
    patchVersion = 0;
    if (numbers.size() == 2) patchVersion = numbers.at(1).toInt(&valid);
    if (!valid) patchVersion = 0; //ignore anything that isn't a number

    //Read the other version numbers
    numbers = numbers.at(0).split('.');
    if (numbers.size() != 3) return false;
    significantVersion = numbers.at(0).toInt(&valid);
    if (!valid) return false;
    majorVersion = numbers.at(1).toInt(&valid);
    if (!valid) return false;
    minorVersion = numbers.at(2).toInt(&valid);
    if (!valid) return false;

    return true;
}
