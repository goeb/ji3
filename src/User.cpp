
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

#include "User.hpp"
#include "Util.hpp"


vector<User> User::Users;
const string User::FILE_EXTENSION = ".jin";
const string User::VERSION_MARKER = "jeu inhibition;2.0";
string User::DataDir = ".";

void User::loadUsers(string dataDirectory)
{
    Users.clear(); // empty the list
    DataDir = dataDirectory;
    
    vector<string> files;
    bool success = Util::readDirectory(dataDirectory, files, true);
    if (!success)
    {
        LOG_ERROR("Impossible d'ouvrir le repertoire: " << dataDirectory);
    }
    
    vector<string>::iterator file;
    
    for (file = files.begin(); file != files.end(); file++)
    {
        // look for files with the correct extension
        string filename = *file;
        if (filename.size() < User::FILE_EXTENSION.size()) continue; // ignore this file
        
        int pos = filename.size() - User::FILE_EXTENSION.size();
        string fileExtension = filename.substr(pos, User::FILE_EXTENSION.size());
        if (fileExtension != User::FILE_EXTENSION) continue; // ignore this file
        
        LOG_DEBUG("loadUsers: found file " << filename);
        
        User currentUser;
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
    filename = _filename; // store the file path
    
    // compute and store the name
    name = Util::basename(filename);
    // remove the file extension
    name = name.substr(0, name.size()-4);
    

    // load the data contained in the file and store it in the current object
    string fileContents = "";

    bool r = Util::readWholeFile(filename, fileContents);
        
    LOG_DEBUG("User::load: " << filename << ", r=" << r);

    if (!r) return false;
    
    vector<string> lines = Util::split("\n", fileContents);

    if (lines.size() <= 0) return false;
    
    string firstLine = lines[0];
    Util::trim(firstLine);
    if (User::VERSION_MARKER != firstLine)
    {
        LOG_INFO("User file " << filename << ": incorrect version: " << firstLine);
        return false;
    }
    
    // now parse the following lines and store the values
    vector<string>::iterator line;
    line = lines.begin();
    line++; // skip the version line (processed above)
    for (/* start already initialized */; line != lines.end(); line++)
    {
        if (line->size() == 0) continue;
        
        
        TestResult result;
        bool success = result.loadFromString(*line);
        if (success)
        {
            results.push_back(result);
        }
    }

    return true;

}





vector<User> & User::getUsers()
{
    return User::Users;
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

User * User::createUser(string username)
{
    string userPath = DataDir + '/' + username + FILE_EXTENSION;
    
    LOG_INFO("Creating new user file: " << userPath);
    ofstream file(userPath.c_str(), ofstream::out | ifstream::binary);
    
    if (file.fail())
    {
        // the file could not be opened
        LOG_ERROR("Impossible de creer le fichier: " << userPath);
        return 0;
    }
    else
    {
        file << VERSION_MARKER << endl;
        file.close();
    }

    User u;
    u.name = username;
    u.filename = userPath;
    Users.push_back(u); // copy into a static table
    User * userPointer = &(Users.back());
    return userPointer;
}

#if 0
void User::storeNewResult(Scenario scenario, DataSet dataset, TestMode mode, int finalScore, double clickSpeed)
{
    TestResult result;
    result.date = Logger::getTime();
    result.datasetFolder = dataset.directoryName;
    result.datasetCategory = dataset.subsetName;
    result.scenarioName = scenario.getPath();
    result.attentionOrInhibition = mode;
    result.correctness = finalScore;
    result.clickSpeed = clickSpeed;
    
    string line = result.serialize();
    // open the file
    ofstream file(filename.c_str(), ofstream::app | ifstream::binary);
    if (file.fail())
    {
        // the file could not be opened
        LOG_ERROR("Impossible d''enregistrer dans: " << filename);
        return;
    }
    file << line << endl;
}

#endif
