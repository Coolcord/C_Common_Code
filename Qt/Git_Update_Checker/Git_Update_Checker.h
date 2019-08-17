#ifndef GIT_UPDATE_CHECKER_H
#define GIT_UPDATE_CHECKER_H

#include <QString>

class Git_Update_Checker {
public:
    Git_Update_Checker() {}

    //Returns a version number if there is an update. Empty string if not.
    QString Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QString &remoteUrl);
    QString Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QStringList &remoteUrls);
    QString Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QString &remoteUrl, QString gitLocation);
    QString Check_For_Updates(const QString &currentVersion, QString &foundViaUrl, const QStringList &remoteUrls, QString gitLocation);
    bool Is_Version_Newer_Than_Current(const QString &version, const QString &currentVersion);

private:
    bool Get_Version_Numbers_From_String(const QString &version, int &significantVersion, int &majorVersion, int &minorVersion, int &patchVersion);
    QByteArray Run_Git_LS_Remote_Command(const QString &gitLocation, const QString &remoteUrl, bool useSSL);
};

#endif // GIT_UPDATE_CHECKER_H
