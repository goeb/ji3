#ifndef __Util_hpp
#define __Util_hpp

#include <iostream>
#include <vector>
#include <set>
using namespace std;

typedef struct timeval Timestamp;
        
class Util
{
    public:
        static string readLine(const string& text, int & i);
        static void skipBlanks(const string& text, int & i);
        static void trim(string & s);
        static bool readWholeFile(const string & filepath, string & fileContents);
        static vector<string> split(const char *c, const string & s, int limit = -1);
        static bool readDirectory(const string &dir, vector<string> & files, bool fullPath);
        static string basename(string path);
        static string dirname(string path);
        static Timestamp getTimestamp();
        static double getElapsedTime(Timestamp t);
        static bool splitQuotes(const std::string & line, std::vector<std::string> & tokens);
        static std::string vectorToString(const std::vector<std::string> & v);
        static std::string vectorToString(const std::set<std::string> & v);
        static std::vector<std::string> shuffle(std::vector<std::string> L, bool forbidSameNeighbors);



};

#endif
