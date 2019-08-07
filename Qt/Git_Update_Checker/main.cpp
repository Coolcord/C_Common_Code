//This code will build a working Git ls-remote binary directory for use with the Update_Checker class
//Simply download a full Git install and copy the files into the program directory under ./Git
//When the code is run, the unnecessary files from the ./Git folder will be removed!

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <assert.h>

const static QString GIT_EXE_LOCATION = "/Git/bin/git.exe";

bool Get_Version_Numbers_From_String(const QString &version, int &significantVersion, int &majorVersion, int &minorVersion, int &patchVersion) {
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

bool Is_Configuration_Valid(const QString &applicationLocation) {
    QString version = "0.0.0";
    int significantVersion = 0, majorVersion = 0, minorVersion = 0, patchVersion = 0;
    assert(Get_Version_Numbers_From_String(version, significantVersion, majorVersion, minorVersion, patchVersion));

    //Run Git to check the remote server for updates
    QProcess process;
    QStringList arguments;
    process.setProgram(applicationLocation+GIT_EXE_LOCATION);
    if (!QFileInfo(process.program()).exists()) return false;
    arguments << "ls-remote" << "--tags" << "https://github.com/Coolcord/Level-Headed.git";
    process.setArguments(arguments);
    process.start(process.program(), process.arguments());
    process.waitForFinished(-1);
    QByteArray bytes = process.readAllStandardOutput();
    if (bytes.isEmpty()) return false; //cannot connect to the server

    //Check for a new version based upon the output
    bool newVersionAvailable = false;
    QString newVersion = QString();
    QTextStream stream(bytes);
    while (!stream.atEnd()) {
        int newSignificantVersion = 0, newMajorVersion = 0, newMinorVersion = 0, newPatchVersion = 0;
        QString line = stream.readLine();
        QStringList strings = line.split("refs/tags/v");
        if (strings.size() != 2) continue;
        if (!Get_Version_Numbers_From_String(strings.at(1), newSignificantVersion, newMajorVersion, newMinorVersion, newPatchVersion)) continue;
        if (newSignificantVersion > significantVersion) { newVersionAvailable = true; newVersion = strings.at(1); break; }
        if (newMajorVersion > majorVersion) { newVersionAvailable = true; newVersion = strings.at(1); break; }
        if (newMinorVersion > minorVersion) { newVersionAvailable = true; newVersion = strings.at(1); break; }
        if (newPatchVersion > patchVersion) { newVersionAvailable = true; newVersion = strings.at(1); break; }
    }

    return newVersionAvailable;
}

bool Check_Folder(const QString &applicationLocation, const QString &folderPath) {
    //Check for unnecessary files
    QDir dir(folderPath);
    QStringList files = dir.entryList(QDir::Files);
    for (QString file : files) {
        //Move the file outside of the environment
        QString path = dir.path() + "/" + file;
        if (!QFile::rename(path, applicationLocation + "/tmp/" + file)) { qInfo() << "Unable to move " + path + "!"; return false; }

        //Check if that file was necessary
        if (!Is_Configuration_Valid(applicationLocation)) {
            //Move it back if it is
            if (!QFile::rename(applicationLocation + "/tmp/" + file, path)) { qInfo() << "Unable to move " + path + "!"; return false; }
            if (!Is_Configuration_Valid(applicationLocation)) { qInfo() << "Something went wrong! Was the internet disconnected?"; return false; }
        } else if (!QFile::remove(applicationLocation + "/tmp/" + file)) { qInfo() << "Unable to delete " + applicationLocation + "/tmp/" + file + "!"; return false; }
    }

    //Check in subfolders
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (QString folder : folders) {
        //Check if the entire folder is necessary
        if (!dir.rename(folderPath+"/"+folder, applicationLocation+"/tmp/"+folder)) { qInfo() << "Unable to move " + folder + "!"; return false; }
        if (!Is_Configuration_Valid(applicationLocation)) {
            if (!dir.rename(applicationLocation+"/tmp/"+folder, folderPath+"/"+folder)) { qInfo() << "Unable to move " + folder + "!"; return false; }
            if (!Check_Folder(applicationLocation, folderPath + "/" + folder)) return false; //check the entire folder
        } else if (!QDir(applicationLocation+"/tmp/"+folder).removeRecursively()) { qInfo() << "Unable to delete " + folder + "!"; return false; }
    }

    //Remove any empty folders
    if (dir.isEmpty()) {
        if (!dir.removeRecursively()) { qInfo() << "Unable to delete the following folder: " + folderPath + "!"; return false; }
    }
    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QString applicationLocation = a.applicationDirPath();
    if (QFileInfo(applicationLocation+"/tmp").exists() && !QDir(applicationLocation+"/tmp").removeRecursively()) { qInfo() << "Unable to delete the tmp folder!"; return 1; }
    if (!QFileInfo(applicationLocation+"/tmp").exists() && !QDir(applicationLocation).mkdir("tmp")) { qInfo() << "Unable to create the tmp folder!"; return 1; }
    if (!QFileInfo(applicationLocation+GIT_EXE_LOCATION).exists()) { qInfo() << "Git could not be loaded! Put a Git install in: " + a.applicationDirPath() + "/Git"; return 1; }
    if (!Is_Configuration_Valid(applicationLocation)) { qInfo() << "Configuration is not valid! Make sure that Git is installed properly and that you are connected to the internet!"; return 1; }

    qInfo() << "Building environment... Please wait...";
    if (!Check_Folder(applicationLocation, applicationLocation + "/Git")) return 1;
    if (!QDir(applicationLocation+"/tmp").removeRecursively()) { qInfo() << "Unable to remove the tmp folder!"; return 1; }
    qInfo() << "Git environment prepared successfully!";
    return 0;
}
