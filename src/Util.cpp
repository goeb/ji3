
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/time.h>

using namespace std;

#include "Util.hpp"
//#include "Logger.hpp"

string Util::readLine(const string& text, int & i)
{
    string line = "";
    int j = text.find_first_of("\n\r", i);
    line = text.substr(i, j-i);
    //LOG_DEBUG("readline: " << line);
    j++; // skip the \n
    i = j; // advance the index
    return line;
}

void Util::skipBlanks(const string& text, int & i)
{
    bool skip = true;
    bool insideComment = false;
    int L = text.size();
    while ((i < L) && skip)
    {
        switch (text[i])
        {
            case '#':
                insideComment = true;
                break;
            case '\n':
                insideComment = false;
                break;
            case ' ':
            case '\t':
            case '\r':
                break;
            default:
                if (!insideComment) skip = false;
                break;
        }
        if (skip) i++;
    }
}

void Util::trim(string & s)
{
    // remove leading and trailing blanks
    int i = 0;
    int L = s.size();
    while (i < L)
    {
        char c = s[i];
        if ( (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') ) i++;
        else break;
    }
    int j = s.size()-1;
    while (j >= 0)
    {
        char c = s[j];
        if ( (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') ) j--;
        else break;
    }
    s = s.substr(i, j-i+1);
}

bool Util::readWholeFile(const string & filepath, string & fileContents)
{
    //LOG_DEBUG("Reading file: " << filepath);
    ifstream file(filepath.c_str(), ifstream::in | ifstream::binary);
    
    if (file.fail())
    {
        // the file could not be opened
        return false;
    }
    
    // read and parse the scenario file
    // read the entire file
    file.seekg (0, ios::end); // go to the end
    int length = file.tellg(); // get the length
    file.seekg (0, ios::beg); // go back to the beginning

    // allocate memory:
    char * buffer = new char [length+1];

    // read data as a block:
    file.read(buffer, length);
    file.close();
    
    buffer[length] = 0;

    fileContents = buffer;
    delete[] buffer;
    
    return true;
}

vector<string> Util::split(const char *c, const string & s, int limit)
{
    // use limit = -1 for no limit (almost)
    vector<string> fields;
    size_t found;

    int index = 0;
    found = s.find_first_of(c, index);
    while ( (found != string::npos) && (limit != 0) )
    {
        fields.push_back(s.substr(index, found-index));
        
        index = found + 1;
        found = s.find_first_of(c, index);
        limit --;
    }
    fields.push_back(s.substr(index));

    return fields;
}

bool Util::readDirectory(const string & dirPath, vector<string> & files, bool fullPath)
{
    DIR * dir = opendir(dirPath.c_str());
    if (!dir)
    {
        return false;
    }
    
    struct dirent *f;
    while ( (f = readdir(dir)) )
    {
        char * filename = f->d_name;
        string filePath = "";
        if (fullPath)
        {
            filePath = dirPath + '/' + filename;
        }
        else
        {
            filePath = filename;
        }
        files.push_back(filePath);
    }
    return true;

}

string Util::basename(string path)
{
    // remove trailing '/' or '\'
    int n = path.size();
    while (n>=0 && ( (path[n] == '/') || (path[n] == '\\') ) )
    {
        n--;
    }
        
    string result;
    size_t pos = path.find_last_of("/\\", n);
    if (pos == string::npos)
    {
        return path; // no change
    }
    else
    {
        result = path.substr(pos+1);
    }
    
    return result;
}

string Util::dirname(string path)
{
    // remove trailing '/' or '\'
    int n = path.size();
    while (n>=0 && ( (path[n] == '/') || (path[n] == '\\') ) )
    {
        n--;
    }
        
    string result;
    size_t pos = path.find_last_of("/\\", n);
    if (pos == string::npos)
    {
        result = "."; // no change
    }
    else
    {
        result = path.substr(0, pos);
        if (result.size() == 0) result = ".";
    }
    
    return result;

}

Timestamp Util::getTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    
    Timestamp t0;
    t0.tv_sec = tv.tv_sec;
    t0.tv_usec = tv.tv_usec;
    //LOG_DEBUG("getTimestamp: sec=" << t0.tv_sec << ", usec=" << t0.tv_usec);
    return t0;
}

double Util::getElapsedTime(Timestamp t0)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    
    
    double result = tv.tv_sec - t0.tv_sec + (double)(tv.tv_usec-t0.tv_usec)/1000000; 
    //LOG_DEBUG("getElapsedTime: sec=" << tv.tv_sec << ", usec=" << tv.tv_usec << ", result=" << result);
    return result;
}


enum SPLIT_QUOTES_STATE { READY, IN_QUOTES };

bool Util::splitQuotes(const std::string & line, std::vector<std::string> & tokens)
{
    // "a" b "c d" e f => [ 'a', 'b', 'c d', 'e', 'f' ]
    bool result = true;
    size_t n = line.size();
    size_t i;
    enum SPLIT_QUOTES_STATE state = READY;
    std::string currentToken;
    for (i=0; i<n; i++) {
        char c = line[i];
        if (c == '"') {
            // close previous token
            if (currentToken.size()>0) tokens.push_back(currentToken);
            // start new token
            currentToken = "";

            if (state == READY) state = IN_QUOTES;
            else state = READY;
        } else if (isblank(c) && state == IN_QUOTES) {
            currentToken += c;
        } else if (isblank(c) && state == READY) {
            // close previous token
            if (currentToken.size()>0) tokens.push_back(currentToken);
            // start new token
            currentToken = "";
        } else currentToken += c;
    }
    if (state != READY) return false;
    if (currentToken.size()>0) tokens.push_back(currentToken);
    return result;
}

std::string Util::vectorToString(const std::vector<std::string> & v)
{
    std::vector<std::string>::const_iterator it;
    std::ostringstream s;
    s << "[ ";
    for (it = v.begin(); it != v.end(); it++) {
        if (it != v.begin()) s << ", ";
        s << *it;
    }
    s << " ]";
    return s.str();
}

std::string Util::vectorToString(const std::set<std::string> & v)
{
    std::set<std::string>::const_iterator it;
    std::ostringstream s;
    s << "(set)[ ";
    for (it = v.begin(); it != v.end(); it++) {
        if (it != v.begin()) s << ", ";
        s << *it;
    }
    s << " ]";
    return s.str();
}

std::vector<std::string> Util::shuffle(const std::vector<std::string> & itemList, bool & forbidSameNeighbors)
{
    // if forbidSameNeighbors, then try to avoid 2 same items next each other

    std::vector<std::string> result;
    srand(time(0));
    int completeRetry = 2;
    while (completeRetry >= 0) {
        std::vector<std::string> Lcopy = itemList;
        result.clear();

        int n;
        while ( (n = Lcopy.size()) )
        {
            // take a random item
            int position = rand()%n;
            string item = Lcopy[position];

            if (forbidSameNeighbors) {
                if (result.size() > 0 && (item == result[result.size()-1])) {
                    // try again
                    position = rand()%n;
                    item = Lcopy[position];
                    if (result.size() > 0 && (item == result[result.size()-1])) {
                        // try one last time
                        position = rand()%n;
                        item = Lcopy[position];
                        if (result.size() > 0 && (item == result[result.size()-1])) {
                            // do a complete retry
                            if (completeRetry > 0) break;
                            else forbidSameNeighbors = false; // indicate that result could not be achieved
                        }
                    }
                }
            }

            vector<string>::iterator it = Lcopy.begin() + position;
            Lcopy.erase(it);

            // and place it into the resulting list
            result.push_back(item);
            completeRetry = 0;
        }
        completeRetry --;
    }
    return result;
}

