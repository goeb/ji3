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
    inline vector<Scenario> getScenarioList() const { return scenarioList; }

    bool load(const string & filename);
    static inline QString getUserfile(const QString &name) { return getUserfile(std::string(name.toAscii())).c_str(); }
    static inline std::string getUserfile(const  std::string & name) { return std::string("Data/") + name + ".ji3u"; }
private:
    static vector<User> Users;
    string filename;
    static const string FILE_EXTENSION;
    static string DataDir;
    vector<Scenario> scenarioList;
    string name;
};

#endif
