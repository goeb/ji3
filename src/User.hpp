#ifndef __User_hpp
#define __User_hpp


#include <vector>
using namespace std;

#include "Logger.hpp"
#include "ScenarioManager.hpp"

class User
{

public:
    static void loadUsers(string dataDirectory); // store found users in table 'Users'
    static void init(string directory); // load all the users found in the dir
    static inline vector<User> & getUsers() { return User::Users; }
    inline std::string getName() { return name; }
    inline void setName(const std::string _name) { name = _name; }

    static User* getUserbyName(string name);

    void store(); // store into the user's file
    inline vector<Scenario> getScenarioList() { return scenarioList; }

private:
    static vector<User> Users;
    string filename;
    static const string FILE_EXTENSION;
    bool load(const string & filename);
    static string DataDir;
    vector<Scenario> scenarioList;
    string name;
};

#endif
