#ifndef GIT_UPDATE_CHECKER_H
#define GIT_UPDATE_CHECKER_H

#include <QString>

class Git_Update_Checker {
public:
    Git_Update_Checker() {}

    //Returns a version number if there is an update. Empty string if not.
    QString Check_For_Updates(const QString &currentVersion, const QString &remoteUrl);
    QString Check_For_Updates(const QString &currentVersion, const QString &remoteUrl, QString gitLocation);

private:
    bool Get_Version_Numbers_From_String(const QString &version, int &significantVersion, int &majorVersion, int &minorVersion, int &patchVersion);
};

#endif // GIT_UPDATE_CHECKER_H
