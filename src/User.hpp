#ifndef __User_hpp
#define __User_hpp


#include <vector>
using namespace std;

#include "Logger.hpp"
#include "TestResult.hpp"

class User
{
    private:
        static vector<User> Users;
        string filename;
        static const string FILE_EXTENSION;
        static const string VERSION_MARKER;
        bool load(const string & filename);
        static string DataDir;

    public:
        vector<TestResult> results;
        static void loadUsers(string dataDirectory); // store found users in table 'Users'
        static void init(string directory); // load all the users found in the dir
        static vector<User> & getUsers();
                
        static User* getUserbyName(string name);
        static User* createUser(string name);

        string name;
        TestResult currentTestResult;
        void store(); // store into the user's file
        string printResults(); // print a synthesis of th results
        
        //void storeNewResult(Scenario scenario, DataSet dataset, TestMode mode, int finalScore, double clickSpeed);
};

#endif
