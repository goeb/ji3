
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <QDebug>

#include "User.hpp"
#include "Util.hpp"


vector<User> User::Users;
const string User::FILE_EXTENSION = ".ji3u";
string User::DataDir = "";

void User::loadUsers(string dataDirectory)
{
    Users.clear(); // empty the list
    DataDir = dataDirectory;
    
    vector<string> files;
    bool success = Util::readDirectory(dataDirectory, files, true);
    if (!success)
    {
        LOG_ERROR("Impossible d'ouvrir le repertoire: " << dataDirectory);
        return;
    }
    
    vector<string>::iterator file;
    
    for (file = files.begin(); file != files.end(); file++)
    {
        // look for files with the correct extension
        string filename = *file;
        size_t pos = filename.find_last_of('.');

        string fileExtension = filename.substr(pos);
        if (fileExtension != User::FILE_EXTENSION) continue; // ignore this file
        
        LOG_DEBUG("loadUsers: found file " << filename);
        
        User currentUser;
        currentUser.setName(Util::basename(filename.substr(0, pos)));
        bool success = currentUser.load(filename);
        if (success)
        {
            // add this user in the list of available users
            Users.push_back(currentUser);
        }
    }

}



bool User::load(const string & _filename)
{
    QString filename(_filename.c_str());
    // load from user
    qDebug() << "filename=" << filename;
    QFile f(filename);
    int r = f.open(QIODevice::ReadOnly);
    if (!r) {
        LOG_ERROR("Could not open file: " << filename.toLocal8Bit().constData());
        return false;
    }

    // read lines
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine();
        Scenario s;
        std::string L = line.toLocal8Bit().constData();
        vector<string> tokens = Util::split(";", L);
        if (tokens.size() < 11) {
            LOG_ERROR("Malformed result file: " << filename.toLocal8Bit().constData());
            f.close();
            return false;
        }
        unsigned int i = 0;
        s.setDatetime(tokens[i++]);
        s.setPath(tokens[i++]);
        s.globalGrade = atoi(tokens[i++].c_str());
        s.averageClickSpeed = atoi(tokens[i++].c_str());
        s.correctExceptions = atoi(tokens[i++].c_str());
        s.correctRegularItems = atoi(tokens[i++].c_str());

        if (tokens[i] == "sound-on") s.setWithSound(true);
        else s.setWithSound(false);
        i++;

        if (tokens[i] == "inhibition") s.modeInhibition = MODE_INHIBITION;
        else if (tokens[i] == "attention") s.modeInhibition = MODE_ATTENTION;
        else if (tokens[i] == "att_div_sound") s.modeInhibition = MODE_DIVIDED_ATTENTION_SOUND;
        else s.modeInhibition = MODE_DIVIDED_ATTENTION_VISUAL;
        i++;
        s.ratioOfExceptions = atoi(tokens[i++].c_str());
        s.numberOfItems = atoi(tokens[i++].c_str());
        s.numberOfExceptions = s.ratioOfExceptions*s.numberOfItems/100;

        s.periodMs = atoi(tokens[i++].c_str());
        if (i<tokens.size()) s.errorDistribution = tokens[i++].c_str(); // was not in older versions of ji3
        else s.errorDistribution = "";

        if (i<tokens.size()) s.nDistractorsResponse = atoi(tokens[i++].c_str()); // was not in older versions of ji3
        else s.nDistractorsResponse = 0;

        scenarioList.push_back(s);
    }
    f.close();
    return true;
}


User * User::getUserbyName(string username)
{
    User * foundUser = 0;
    vector<User>::iterator u;
    for (u = Users.begin(); u != Users.end(); u++)
    {
        if (u->name == username)
        {
            foundUser = &(*u);
            break; 
        }
    }
    return foundUser;
}


