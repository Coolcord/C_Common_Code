#include "Git_Update_Checker.h"
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

QString Git_Update_Checker::Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QString &remoteUrl) {
    QStringList remoteUrls; remoteUrls << remoteUrl;
    return this->Check_For_Updates(currentVersion, foundViaUrl, remoteUrls, QString());
}

QString Git_Update_Checker::Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QStringList &remoteUrls) {
    return this->Check_For_Updates(currentVersion, foundViaUrl, remoteUrls, QString());
}

QString Git_Update_Checker::Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QString &remoteUrl, QString gitLocation) {
    QStringList remoteUrls; remoteUrls << remoteUrl;
    return this->Check_For_Updates(currentVersion, foundViaUrl, remoteUrls, gitLocation);
}

QString Git_Update_Checker::Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QStringList &remoteUrls, QString gitLocation) {
    if (currentVersion.contains("dev")) return QString(); //don't check for updates in development builds
    int significantVersion = 0, majorVersion = 0, minorVersion = 0, patchVersion = 0;
    assert(this->Get_Version_Numbers_From_String(currentVersion, significantVersion, majorVersion, minorVersion, patchVersion));

    //Run Git to check the remote server for updates
    QByteArray bytes;
    bool localExists = QFileInfo(gitLocation).exists() || QFileInfo(gitLocation+".exe").exists();
    for (QString remoteUrl : remoteUrls) {
        if (!bytes.isEmpty()) break; //something was returned, so don't check any other URLs
        if (localExists) bytes = this->Run_Git_LS_Remote_Command(gitLocation, remoteUrl, true); //try local Git first
        if (bytes.isEmpty()) bytes = this->Run_Git_LS_Remote_Command("git", remoteUrl, true); //try the system version
        if (bytes.isEmpty() && localExists) bytes = this->Run_Git_LS_Remote_Command(gitLocation, remoteUrl, false); //try once more without SSL
        if (!bytes.isEmpty()) foundViaUrl = remoteUrl;
    }
    if (bytes.isEmpty()) return QString(); //no internet or the server is down

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

bool Git_Update_Checker::Is_Version_Newer_Than_Current(const QString &version, const QString &currentVersion) {
    int significantVersion = 0, majorVersion = 0, minorVersion = 0, patchVersion = 0;
    int currentSignificantVersion = 0, currentMajorVersion = 0, currentMinorVersion = 0, currentPatchVersion = 0;
    if (!this->Get_Version_Numbers_From_String(version, significantVersion, majorVersion, minorVersion, patchVersion)) return false;
    if (!this->Get_Version_Numbers_From_String(currentVersion, currentSignificantVersion, currentMajorVersion, currentMinorVersion, currentPatchVersion)) return false;
    if (significantVersion > currentSignificantVersion) return true;
    else if (significantVersion < currentSignificantVersion) return false;
    else if (majorVersion > currentMajorVersion) return true;
    else if (majorVersion < currentMajorVersion) return false;
    else if (minorVersion > currentMinorVersion) return true;
    else if (minorVersion < currentMinorVersion) return false;
    else if (patchVersion > currentPatchVersion) return true;
    else if (patchVersion < currentPatchVersion) return false;
    else return false;
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

QByteArray Git_Update_Checker::Run_Git_LS_Remote_Command(const QString &gitLocation, const QString &remoteUrl, bool useSSL) {
    if (remoteUrl.startsWith("http") && !useSSL) return QByteArray();
    QProcess process;

    //Set the Environment Flags
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (useSSL) env.insert("GIT_SSL_NO_VERIFY", "false");
    else env.insert("GIT_SSL_NO_VERIFY", "true");
    process.setProcessEnvironment(env);

    //Set the arguments
    QStringList arguments;
    process.setProgram(gitLocation);
    arguments << "ls-remote" << "--tags" << remoteUrl;
    process.setArguments(arguments);
    process.start(process.program(), process.arguments());
    process.waitForFinished(-1);
    return process.readAllStandardOutput();
}
