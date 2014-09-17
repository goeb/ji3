#ifndef __Util_hpp
#define __Util_hpp

#include <iostream>
#include <vector>
#include <set>
#include <time.h>
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
        static void shuffle(std::vector<std::string> & items, int offset, int N, bool & forbidSameNeighbors);
        template <typename T> static std::vector<T> shuffle(const std::vector<T> &itemList, bool &forbidSameNeighbors)
        {
            // if forbidSameNeighbors, then try to avoid 2 same items next each other

            std::vector<T> result;
            srand(time(0));
            int completeRetry = 2;
            while (completeRetry >= 0) {
                std::vector<T> Lcopy = itemList;
                result.clear();

                int n;
                while ( (n = Lcopy.size()) )
                {
                    // take a random item
                    int position = rand()%n;
                    T item = Lcopy[position];

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

                    typename std::vector<T>::iterator it = Lcopy.begin() + position;
                    Lcopy.erase(it);

                    // and place it into the resulting list
                    result.push_back(item);
                    completeRetry = 0;
                }
                completeRetry --;
            }
            return result;
        }

};

#endif
