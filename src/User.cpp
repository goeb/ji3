
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

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
    QString file(_filename.c_str());
    return Scenario::load(file, scenarioList);
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


