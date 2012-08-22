#include <stdlib.h>
#include <strings.h>
#include <QDateTime>
#include <QTextStream>


#include "ScenarioManager.hpp"
#include "Util.hpp"
#include "Logger.hpp"


#define SEPARATOR ';';

map<string, Scenario> ScenarioManager::AvailableScenarios;

vector<string> ScenarioManager::getAvailableScenarios()
{
    // 1. look in the ScenarioHomeDirectory for directories
    vector<string> files;
    bool success = Util::readDirectory(".", files, false);
    if (!success)
    {
        LOG_ERROR("Impossible d'ouvrir le répertoire: .");
    }
    
    vector<string>::iterator file;
    vector<string> result;
    // 2. load the scenarios
    for (file = files.begin(); file != files.end(); file++)
    {
        string scenarioName = *file;
        Scenario s(scenarioName, 0, 0, 0, 0);
        bool isSuccessful = s.load();
        if (isSuccessful)
        {
            LOG_DEBUG("Valid scenario: " << scenarioName);
            AvailableScenarios[scenarioName] = s;
            result.push_back(scenarioName);
        }
        else
        {
            LOG_DEBUG("Invalid scenario: " << scenarioName);
        }
    }
    return result;
}

Scenario::Scenario(const string &_path, int _periodMs, int _numberOfItems, int _numberOfExceptions, bool _modeInhibition)
{
    path = _path;
    periodMs = _periodMs;
    numberOfItems = _numberOfItems;
    numberOfExceptions = _numberOfExceptions;
    modeInhibition = _modeInhibition;

    errorsOnRegularItems = 0;
    errorsOnExceptions = 0;
    cumulatedClickTime = 0;
    numberOfClick = 0;

    withSound = true; // TODO manage this option
}

Scenario::Scenario()
{
}

bool Scenario::parseItems(const std::string & line, std::set<std::string> & items)
{
    // parse a line to get a list of files.
    // Ex:
    // *.png => [ a.png, b.png ]        // use global
    // a.png b.png => [ a.png, b.png ]  // simple case
    // "a b.png" => [ 'a b.png' ]       // manage white spaces when between double quotes

    std::vector<std::string> result;

    // first, resolve ""
    std::vector<std::string> tokens;
    bool r = Util::splitQuotes(line, tokens);
    if (!r) return r;

    std::string dir = Util::dirname(path);
    QDir qdir(QString(dir.c_str()));

    // then resolv wildcards
    // convert token to QStringList type
    QStringList nameFilters;
    std::vector<std::string>::iterator token;
    for (token = tokens.begin(); token != tokens.end(); token++) {
        nameFilters.append(QString(token->c_str()));
    }

    QStringList files = qdir.entryList(nameFilters);

    // convert to std::set<std::string>
    // and add directory in path

    for (int i = 0; i < files.size(); ++i) {
        items.insert(dir + '/' + files.at(i).toLocal8Bit().constData());
    }

    return true;
}

bool Scenario::load()
{
    // load scenario information from file (parse the file, etc.)
    string fileContents = "";
    bool r = Util::readWholeFile(path, fileContents);
    
    if (!r) return false;

    int lineNum = 0;
    
    // parse the file
    vector<string> lines = Util::split("\n", fileContents);
    vector<string>::iterator line;
    for (line=lines.begin(); line != lines.end(); line++)
    {
        lineNum++;
        std::string L = *line;
        // syntax of line is "key:value"
        // "#" characters start comments

        vector<string> toks = Util::split("#", L);
        L = toks[0];

        // remove comments, if any.
        size_t commentMarker = L.find('#');
        if (commentMarker != string::npos) {
            // comment marker (#) found
            // remove characters after #
            L = L.substr(commentMarker);
        }

        vector<string> tokens = Util::split(":", L);
        if (tokens.size() > 1) {
            string left = tokens[0];
            Util::trim(left);
            string right = tokens[1];
            Util::trim(right);
            
            if (0 == left.compare("items")) {
                // parse items
                bool r  = parseItems(right, listOfAllItems);
                if (!r) {
                    LOG_ERROR("Erreur de syntaxe. Fichier: " << path << " Ligne: " << lineNum);
                }
                LOG_DEBUG("items: " << Util::vectorToString(listOfAllItems));

            } else if (0 == left.compare("exceptions")) {
                bool r  = parseItems(right, listOfExceptions);
                if (!r) {
                    LOG_ERROR("Erreur de syntaxe. Fichier: " << path << " Ligne: " << lineNum);
                }
                LOG_DEBUG("exceptions: " << Util::vectorToString(listOfExceptions));

            } else if (0 == left.compare("description")) {
                description = right;
            }
        } // else go to next line
    }
    // TODO check some things: listOfItemsToBeIgnored ! =0, etc
    
    return true;
}


vector<string> Scenario::createFixedSizeList(vector<string> & inputList, int n)
{
    // eg:
    // [1, 2, 3, 4] and n=6 => [1, 2, 3, 4, 1, 2]
    // [1, 2, 3, 4] and n=3 => [1, 2, 3]


    vector<string> result;

    // given a set of strings, create a list of N elements
    vector<string>::iterator item = inputList.begin();
    if (item == inputList.end())
    {
        LOG_ERROR("createFixedSizeList: inputList empty!");
        return result;
    }
    
    int i;
    for (i = 0; i < n; i++)
    {
        string itemText = *item;
        result.push_back(itemText);
        
        item++;
        if (inputList.end() == item)
        {
             // wrap around
            item = inputList.begin();
        }
    }
    return result;
}

void Scenario::generateItemList()
{
    // Create a list of elements, conforming to the configuration of the scenario:
    // - random order
    // - etc.

    // algorithm:
    // 1. create list L1 with N1 exceptions
    // 2. create list L2 with N2 items (without exceptions)
    // 3. merge these 2 lists and randomize the sequence

    // 1. create list L1 with N1 exceptions
    // 1.a convert 'set' into 'vector' in order to be able to shuffle
    vector<string> exceptions;
    set<string>::iterator it;
    for (it = listOfExceptions.begin(); it != listOfExceptions.end(); it++) {
        exceptions.push_back(*it);
    }
    // 1.b shuffle
    exceptions = Util::shuffle(exceptions);
    // 1.c create list with right number of items
    exceptions = createFixedSizeList(exceptions, numberOfExceptions);

    // 2. create list L2 with N2 items (without exceptions)
    // 2.a create list without exceptions: L3 = L2-L1
    vector<string> regularItems;
    for (it = listOfAllItems.begin(); it != listOfAllItems.end(); it++) {
        if (listOfExceptions.count(*it) == 0) {
            regularItems.push_back(*it);
        }
    }
    // 2.b shuffle
    regularItems = Util::shuffle(regularItems);

    // 2.c from L3, build a list with right number of items
    regularItems = createFixedSizeList(regularItems, numberOfItems - numberOfExceptions);

    LOG_DEBUG("regularItems: " << Util::vectorToString(regularItems));

    // 3. merge these 2 lists
    vector<string> mergedList;
    mergedList.insert(mergedList.begin(), exceptions.begin(), exceptions.end());
    mergedList.insert(mergedList.begin(), regularItems.begin(), regularItems.end());
    
    // shuffle (and randomize the sequence)
    itemSequence = Util::shuffle(mergedList);

    LOG_DEBUG("createItemList: " << Util::vectorToString(itemSequence));
}

bool Scenario::evaluateUserClick(const std::string & item, bool hasClicked)
{
    bool result;
    // mode attention TODO manage 3rd mode

    if (listOfExceptions.count(item) == 1) {
        // item is an exception
        if (modeInhibition) {
            result = !hasClicked;

        } else {
            // mode attention
            result = hasClicked;
        }
        if (!result) errorsOnExceptions ++;

    } else {
        // item is a regular item
        if (modeInhibition) {
            result = hasClicked;
        } else {
            // mode attention
            result = !hasClicked;
        }
        if (!result) errorsOnRegularItems ++;
    }
    return result;
}

void Scenario::addClickTime(qint64 clickTime)
{
    numberOfClick ++;
    cumulatedClickTime += clickTime;
}
void Scenario::consolidateResult()
{
    LOG_DEBUG("cumulatedClickTime=" << cumulatedClickTime << ", numberOfClick=" << numberOfClick);
    averageClickSpeed = cumulatedClickTime / numberOfClick;
    correctRegularItems = (numberOfItems - numberOfExceptions - errorsOnRegularItems);
    correctExceptions = numberOfExceptions - errorsOnExceptions;
    globalGrade = (correctRegularItems + 3*correctExceptions - errorsOnRegularItems - 3*errorsOnExceptions) * 100 /
                      (correctRegularItems + 3*correctExceptions);

    if (globalGrade<0) globalGrade = 0;

    LOG_DEBUG("vitesse de click=" << averageClickSpeed << " ms, score=" << globalGrade <<
              ", errorsOnExceptions="<<errorsOnExceptions << "/" << numberOfExceptions <<
              ", errorsOnRegularItems=" << errorsOnRegularItems << "/" << (numberOfItems - numberOfExceptions));
}

void Scenario::store(const QString & filename)
{
    QFile f(filename);
    int r = f.open(QIODevice::Append);
    if (!r) {
        LOG_ERROR("Could not open file: " << filename.toLocal8Bit().constData());
        return;
    }

    // write the result row
    QTextStream row(&f);
    QDateTime now =	QDateTime::currentDateTime();
    QString formattedDatetime = now.toString("yyyy-MM-dd hh:mm:ss");
    datetime = formattedDatetime.toAscii().constData();
    row << formattedDatetime.toLocal8Bit().constData() << SEPARATOR;
    row << path.c_str() << SEPARATOR;
    row << globalGrade << SEPARATOR;
    row << averageClickSpeed << SEPARATOR;
    row << correctExceptions << SEPARATOR;
    row << correctRegularItems << SEPARATOR;
    row << (withSound?"sound-on":"sound-off") << SEPARATOR;
    row << (modeInhibition?"inhibition":"attention") << SEPARATOR;
    row << numberOfExceptions << SEPARATOR;
    row << numberOfItems << SEPARATOR;
    row << periodMs;
    row << "\n";
    f.close();
}

bool Scenario::load(const QString & filename, vector<Scenario> & scenarioList)
{
    vector<Scenario> result;

    QFile f(filename);
    int r = f.open(QIODevice::ReadOnly);
    if (!r) {
        LOG_ERROR("Could not open file: " << filename.toLocal8Bit().constData());
        return false;
    }

    // read lines
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine();
        Scenario s;
        std::string L = line.toLocal8Bit().constData();
        vector<string> tokens = Util::split(";", L);
        if (tokens.size() != 11) {
            LOG_ERROR("Malformed result file: " << filename.toLocal8Bit().constData());
            return false;
        }
        int i = 0;
        s.datetime = tokens[i++];
        s.path = tokens[i++];
        s.globalGrade = atoi(tokens[i++].c_str());
        s.averageClickSpeed = atoi(tokens[i++].c_str());
        s.correctExceptions = atoi(tokens[i++].c_str());
        s.correctRegularItems = atoi(tokens[i++].c_str());
        //s.withSound = tokens[i++]; TODO
        // s.modeInhibition = tokens[i++]; TOOD
        s.numberOfExceptions = atoi(tokens[i++].c_str());
        s.numberOfItems = atoi(tokens[i++].c_str());
        s.periodMs = atoi(tokens[i++].c_str());
        result.push_back(s);
    }
    return true;

}
