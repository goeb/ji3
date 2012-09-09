#ifdef _WIN32
  #include <windows.h>
#endif

#include <fstream>

#include "Logger.hpp"

Logger * Logger::TheLogger = 0;

string Logger::getTime()
{
    struct timeval tv;
    string timestamp; // will hold the stringified date and time
            
    int rc = gettimeofday(&tv, 0);
    if (-1 == rc)
    {
        timestamp = "Error";
    }
    else
    {
        // convert the time into a readable string
        char buffer[100];

#ifdef _WIN32
        SYSTEMTIME lt;
        GetLocalTime(&lt);
        sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                lt.wYear,
                lt.wMonth,
                lt.wDay,
                lt.wHour,
                lt.wMinute,
                lt.wSecond,
                lt.wMilliseconds);
        timestamp = buffer;
#else
        // linux
        struct tm localT;
        localtime_r(&tv.tv_sec, &localT);
        string dateFormat = "%Y-%m-%d %H:%M:%S";
        strftime(buffer, 99, dateFormat.c_str(), &localT);
        int milliseconds = tv.tv_usec / 1000;
        char millisecBuffer[4];
        sprintf(millisecBuffer, "%03d", milliseconds);
        timestamp = buffer;
        timestamp += ".";
        timestamp += millisecBuffer;
#endif
    }
    return timestamp;
}

Logger::Logger()
{
}

void Logger::init(const string & dir)
{
    TheLogger = new Logger();
    TheLogger->directory = dir;
}

ostringstream & Logger::getStream()
{
    return TheLogger->logStream;
}
void Logger::updateOutput()
{
    // flush the contents of the logStream to the appropriate target
    // flush to stdout
    cout << TheLogger->logStream.str();

    
    // in all cases, write the log to a file
    string filePath = TheLogger->directory + "/Data/jeu_inhibition.log";
    ofstream file(filePath.c_str(), ofstream::app | ifstream::binary);
    
    if (!file.fail())
    {
        file << TheLogger->logStream.str();
    }

    TheLogger->logStream.str(""); // empty the buffer

}




