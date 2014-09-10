#include <stdlib.h>
#include <strings.h>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>

#include "ScenarioManager.hpp"
#include "Util.hpp"
#include "Logger.hpp"


#define SEPARATOR ';';
const char * THEME_DIR = "Themes";

Scenario::Scenario(const string &_path, int _periodMs, int _numberOfItems, int _ratioOfExceptions, TestMode _modeInhibition, bool sound)
{
    path = _path; // this is the path of the file minus the suffix .ji3c
    periodMs = _periodMs;
    numberOfItems = _numberOfItems;
    ratioOfExceptions = _ratioOfExceptions;
    numberOfExceptions = ratioOfExceptions*numberOfItems/100;

    modeInhibition = _modeInhibition;

    errorsOnRegularItems = 0;
    errorsOnExceptions = 0;
    cumulatedClickTime = 0;
    numberOfClick = 0;

    withSound = sound;
}

Scenario::Scenario()
{
}

bool Scenario::parseFileItems(const std::string & line, std::set<std::string> & items, const std::string & encoding)
{
    // parse a line to get a list of files.
    // Ex:
    // *.png => [ a.png, b.png ]        // use global
    // a.png b.png => [ a.png, b.png ]  // simple case
    // "a b.png" => [ 'a b.png' ]       // manage white spaces when between double quotes

    LOG_DEBUG("parseItems(encoding=" << encoding << ")");
    // handle encoding of file names.
    QTextCodec *saveCodecCStrings = QTextCodec::codecForCStrings();
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName(encoding.c_str()));
    QTextCodec *saveCodecLocale = QTextCodec::codecForLocale();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(encoding.c_str()));


    std::vector<std::string> result;

    // first, resolve ""
    std::vector<std::string> tokens;
    bool r = Util::splitQuotes(line, tokens);
    if (!r) return r;

    std::string filename = THEME_DIR;
    filename += "/" + path;
    std::string dir = Util::dirname(filename);
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
        LOG_DEBUG("toAscii=" << files.at(i).toLocal8Bit().constData());
        items.insert(dir + '/' + files.at(i).toLocal8Bit().constData());
    }

    QTextCodec::setCodecForCStrings(saveCodecCStrings);
    QTextCodec::setCodecForCStrings(saveCodecLocale);

    return true;
}

bool Scenario::load()
{
    return load(path);
}

bool Scenario::load(std::string name)
{
    // load scenario information from file (parse the file, etc.)
    string fileContents = "";
    string filename = THEME_DIR;
    filename += "/" + name + SCENARIO_DESCRIPTOR_SUFFIX;
    bool r = Util::readWholeFile(filename, fileContents);
    
    if (!r) return false;

    int lineNum = 0;
    encoding = "UTF-8"; // default value
    std::string exceptionsPattern = "";
    std::string itemsPattern = "";
    std::string textItems = "";
    std::string textExceptions = "";
    description = "Description manquante";

    forcePeriodMs = 0;
    forceNumberOfItems = 0;
    forceRatioOfExceptions = 0;
    forceModeInhibition = MODE_NONE;
    forceWithSound = -1;

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

        vector<string> tokens = Util::split(":", L, 1);
        if (tokens.size() > 1) {
            string left = tokens[0];
            Util::trim(left);
            string right = tokens[1];
            Util::trim(right);
            
            if (0 == left.compare("items")) {
                itemsPattern = right;

            } else if (0 == left.compare("exceptions")) {
                exceptionsPattern = right;

            } else if (0 == left.compare("text-items")) {
                textItems = right;

            } else if (0 == left.compare("text-exceptions")) {
                textExceptions = right;

            } else if (0 == left.compare("description-inhibition")) {
                if (modeInhibition == MODE_INHIBITION) description = right;

            } else if (0 == left.compare("description-attention")) {
                if (modeInhibition == MODE_ATTENTION) description = right;

            } else if (0 == left.compare("description-att-div-son")) {
                if (modeInhibition == MODE_DIVIDED_ATTENTION_SOUND) description = right;

            } else if (0 == left.compare("description-att-div-visuel")) {
                if (modeInhibition == MODE_DIVIDED_ATTENTION_VISUAL) description = right;

            } else if (0 == left.compare("force-speed")) {
                forcePeriodMs = atoi(right.c_str());

            } else if (0 == left.compare("force-sound")) {
                forceWithSound = right;

            } else if (0 == left.compare("force-exception")) {
                forceRatioOfExceptions = atoi(right.c_str());

            } else if (0 == left.compare("force-number")) {
                forceNumberOfItems = atoi(right.c_str());

            } else if (0 == left.compare("force-type")) {
                if (right == "attention") forceModeInhibition = MODE_ATTENTION;
                else if (right == "inhibition") forceModeInhibition = MODE_INHIBITION;
                else if (right == "att-div-son") forceModeInhibition = MODE_DIVIDED_ATTENTION_SOUND;
                else if (right == "att-div-visuel") forceModeInhibition = MODE_DIVIDED_ATTENTION_VISUAL;
                // else nothing set

            } else if (0 == left.compare("encoding")) {
                encoding = right;
            }
        } // else go to next line
    }

    // resolve items
    if (itemsPattern.size() > 0) {
        r  = parseFileItems(itemsPattern, listOfAllItems, encoding);
        if (!r) {
            LOG_ERROR("Erreur de syntaxe sur 'items'. Fichier: " << path << "  " << lineNum);
        }
    }
    if (textItems.size() > 0) {
        // first, resolve ""
        std::vector<std::string> tokens;
        bool r = Util::splitQuotes(textItems, tokens);
        if (!r) {
            LOG_ERROR("Cannot resolve quotes in: " << textItems);
        }
        listOfAllItems.insert(tokens.begin(), tokens.end());
    }
    LOG_DEBUG("items: " << Util::vectorToString(listOfAllItems));

    // resolve exceptions
    if (exceptionsPattern.size() > 0) {
        r  = parseFileItems(exceptionsPattern, listOfExceptions, encoding);
        if (!r) {
            LOG_ERROR("Erreur de syntaxe. Fichier: " << path << " Ligne: " << lineNum);
        }
    }
    if (textExceptions.size() > 0) {
        // first, resolve ""
        std::vector<std::string> tokens;
        bool r = Util::splitQuotes(textExceptions, tokens);
        if (!r) {
            LOG_ERROR("Cannot resolve quotes in: " << textExceptions);
        }
        listOfExceptions.insert(tokens.begin(), tokens.end());
    }

    LOG_DEBUG("exceptions: " << Util::vectorToString(listOfExceptions));


    // TODO check some things: listOfItemsToBeIgnored ! =0, etc
    
    return true;
}



QStringList Scenario::retrieveScenarios()
{
    QDir themeDir(THEME_DIR);
    QStringList dirs = themeDir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
    // for each of these dir, look for *.ji3c (in other words: Themes/*/*.ji3c)

    QStringList scenarioFiles;
    for (int i = 0; i < dirs.size(); i++) {
        QDir dir(QString(THEME_DIR) + "/" + dirs.at(i));
        QString filter = "*";
        filter += SCENARIO_DESCRIPTOR_SUFFIX;
        QStringList filters;
        filters << filter;
        QStringList confFiles = dir.entryList(filters);
        for (int j = 0; j < confFiles.size(); j++) {
            QString withPath = dirs.at(i) + "/" + confFiles.at(j);
            scenarioFiles += withPath;
        }

    }
    qDebug() << scenarioFiles;
    return scenarioFiles;
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
    bool sameNeighbors = false;
    exceptions = Util::shuffle(exceptions, sameNeighbors);
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
    regularItems = Util::shuffle(regularItems, sameNeighbors);

    // 2.c from L3, build a list with right number of items
    regularItems = createFixedSizeList(regularItems, numberOfItems - numberOfExceptions);

    LOG_DEBUG("regularItems: " << Util::vectorToString(regularItems));

    // 3. merge these 2 lists
    // work quarter by quarter
    const int N_Q = 4; // 4 quarters
    int takeFromL1 = 0;
    int takeFromL2 = 0;
    int offset = 0;
    for (int q = 1; q <= N_Q; q++) {
        takeFromL1 = regularItems.size()/(N_Q + 1 - q);
        takeFromL2 = exceptions.size()/(N_Q + 1 - q);

        itemSequence.insert(itemSequence.end(), regularItems.begin(), regularItems.begin() + takeFromL1);
        itemSequence.insert(itemSequence.end(), exceptions.begin(), exceptions.begin() + takeFromL2);

        regularItems.erase(regularItems.begin(), regularItems.begin() + takeFromL1);
        exceptions.erase(exceptions.begin(), exceptions.begin() + takeFromL2);

        // shuffle this "in place"
        sameNeighbors = true;
        Util::shuffle(itemSequence, offset, takeFromL1+takeFromL2, sameNeighbors);
        if (!sameNeighbors) LOG_ERROR("Could not forbid same neighbors");
        offset += takeFromL1 + takeFromL2;
    }
    
    LOG_DEBUG("createItemList: " << Util::vectorToString(itemSequence));
}

void Scenario::generateDistractors()
{
    if (modeInhibition != MODE_DIVIDED_ATTENTION_SOUND && modeInhibition != MODE_DIVIDED_ATTENTION_VISUAL) return;

    // generate a random sequence of zeroes and ones :
    // 0 : no distractor
    // 1 : distractor
    // number of elements : same as itemSequence
    // number of distrators : min 3 and max 20 (or max itemSequence.size()-1)
    int n = itemSequence.size();
    int max = 20;
    if (n-1 < max) max = n-1;
    if (max <= 3) max = 4;
    int min = 3;
    int nDistractors = rand()%(max-min+1) + min;
    distractors.insert(distractors.begin(), nDistractors, 1);
    distractors.insert(distractors.begin(), n-nDistractors, 0);
    bool neighbors = false;
    distractors = Util::shuffle<int>(distractors, neighbors);
}



bool Scenario::evaluateUserClick(const std::string & item, bool hasClicked)
{
    bool result;
    // mode attention TODO manage 3rd mode

    if (listOfExceptions.count(item) == 1) {
        // item is an exception
        if (MODE_INHIBITION == modeInhibition) {
            result = !hasClicked;

        } else {
            // mode attention or divided attention
            result = hasClicked;
        }
        if (!result) errorsOnExceptions ++;
        resultVector.push_back(result?EXCEPTION_OK:EXCEPTION_ERROR);

    } else {
        // item is a regular item
        if (MODE_INHIBITION == modeInhibition) {
            result = hasClicked;
        } else {
            // mode attention or divided attention
            result = !hasClicked;
        }
        if (!result) errorsOnRegularItems ++;
        resultVector.push_back(result?REGULAR_ITEM_OK:REGULAR_ITEM_ERROR);
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
    if (numberOfClick == 0) numberOfClick = 1;
    averageClickSpeed = cumulatedClickTime / numberOfClick;
    correctRegularItems = (numberOfItems - numberOfExceptions - errorsOnRegularItems);
    correctExceptions = numberOfExceptions - errorsOnExceptions;
    if (correctRegularItems + 3*correctExceptions != 0) {
        globalGrade = (correctRegularItems + 3*correctExceptions - errorsOnRegularItems - 3*errorsOnExceptions) * 100 /
                      (correctRegularItems + 3*correctExceptions);
    } else globalGrade = 0;

    if (globalGrade<0) globalGrade = 0;

    computeErrorDetails();

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

    if (modeInhibition == MODE_INHIBITION) row << "inhibition";
    else if (modeInhibition == MODE_ATTENTION) row << "attention";
    else if (modeInhibition == MODE_DIVIDED_ATTENTION_SOUND) row << "att-div-sound";
    else row << "att-div-visual";
    row << SEPARATOR;

    row << ratioOfExceptions << SEPARATOR;
    row << numberOfItems << SEPARATOR;
    row << periodMs << SEPARATOR;
    row << errorDistribution;
    if (modeInhibition == MODE_DIVIDED_ATTENTION_SOUND || modeInhibition == MODE_DIVIDED_ATTENTION_VISUAL) {
        row << SEPARATOR;
        row << nDistractorsResponse;
    }
    row << "\n";
    f.close();
}

bool Scenario::isSame(const Scenario & other) const
{
    if (getPath() != other.getPath()) return false;
    if (getPeriodMs() != other.getPeriodMs()) return false;
    if (getNumberOfItems() != other.getNumberOfItems()) return false;
    if (getMode() != other.getMode()) return false;
    if (getRatioOfExceptions() != other.getRatioOfExceptions()) return false;
    if (getWithSound() != other.getWithSound()) return false;
    return true;
}

std::vector<Scenario> Scenario::getSameScenario(const std::vector<Scenario> & all) const
{
    // here, we filter the list of scenarios in order
    // to keep only those that match 'this'
    std::vector<Scenario> result;
    std::vector<Scenario>::const_iterator s;
    for (s=all.begin(); s!=all.end(); s++) {
        if (isSame(*s)) result.push_back(*s);
    }
    return result;
}
void Scenario::computeErrorDetails()
{
    // get errors on exceptions for 1st, 2nd and 3rd parts
    // and format these '(r1-r2-r3)'
    int n = resultVector.size();
    int q1 = 0;
    int q2 = 0;
    int q3 = 0;
    int q4 = 0;
    int i = 0;
    for (i = 0; i < n; i++) {
        if (resultVector[i] == EXCEPTION_ERROR ||
            resultVector[i] == REGULAR_ITEM_ERROR) {
            if (i < n/4) q1 ++;
            else if (i < 2*n/4) q2 ++;
            else if (i < 3*n/4) q3 ++;
            else q4 ++;
        }
    }

    errorDistribution = QString("%1-%2-%3-%4").arg(q1).arg(q2).arg(q3).arg(q4);
}
