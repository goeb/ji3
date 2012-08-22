
#ifndef __Logger_hpp
#define __Logger_hpp

#include <iostream>
#include <sstream>

using namespace std;

#include <sys/time.h>
#include <time.h>

#define LOG(level, msg) { \
    Logger::getStream() << Logger::getTime() << " " << level << " " << \
    __FILE__ << ":" << __LINE__ << " " << msg << endl; Logger::updateOutput(); }
#define LOG_ERROR(x) LOG("ERROR", x)
#define LOG_DEBUG(x) LOG("DEBUG", x)
#define LOG_INFO(x) LOG("INFO", x)

class Logger
{
    private:
        Logger();
        ostringstream logStream;
        string directory;

    public:
        static Logger * TheLogger;
        static void init(const string & dir);
        static string getTime();
        static ostringstream & getStream();
        static void updateOutput();

};

#endif
