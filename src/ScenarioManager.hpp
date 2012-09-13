#ifndef __ScenarioManager_hpp
#define __ScenarioManager_hpp

#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;
#include <QDir>

#include "Util.hpp"

#define SCENARIO_DESCRIPTOR_SUFFIX ".ji3c"

                 
typedef enum {
    MODE_INHIBITION, // click all items but on exception items
    MODE_ATTENTION, // click only on exception items
    XX_DIVIDED_ATTENTION, // sound and image not related
    MODE_NONE
} TestMode;

class Scenario
{
    public :
        Scenario(const string & path, int periodMs, int numberOfItems, int numberOfExceptions, bool modeInhibition, bool sound);
        Scenario();
        void generateItemList(); // create a random sequence of items
        bool load();
        bool load(std::string name);

        inline std::string getPath() const { return path; }
        inline std::string getDir() const { return Util::dirname(path); }
        inline std::vector<std::string> getItemSequence() const { return itemSequence; }
        inline int getPeriodMs() const { return periodMs; }
        inline int getNumberOfItems() const { return numberOfItems; }
        inline int getNumberOfExceptions() const { return numberOfExceptions; }
        inline int getWithSound() const { return withSound; }

        inline TestMode getMode() const { return modeInhibition; }
        inline int getRatioOfExceptions() const { return ratioOfExceptions; }

        bool evaluateUserClick(const std::string & item, bool hasClicked);
        void addClickTime(qint64 clickTime);
        void consolidateResult();
        void store(const QString & file);
        static bool loadFromUserFile(const QString & filename, vector<Scenario> & scenarioList);

        std::vector<Scenario> getSameScenario(const std::vector<Scenario> & all) const;

        inline int getAverageClickSpeed() const { return averageClickSpeed; }
        inline int getGlobalGrade() const { return globalGrade; }
        inline std::string getDatetime() const { return datetime; }
        inline int getCorrectRegularItems() const { return correctRegularItems; }
        inline int getCorrectExceptions() const { return correctExceptions; }
        inline std::string getDescription() { return description; }
        static QStringList retrieveScenarios();

        inline int getForcePeriodMs() { return forcePeriodMs; }
        inline int getForceNumberOfItems() { return forceNumberOfItems; }
        inline int getForceModeInhibition() { return forceModeInhibition; }
        inline int getForceRatioOfExceptions() { return forceRatioOfExceptions; }
        inline std::string getForceWithSound() { return forceWithSound; }




    private :
        bool parseFileItems(const std::string & line, std::set<std::string> &items, const string &encoding);
        set<string> listOfAllItems; // including exceptions
        set<string> listOfExceptions;
        string path;
        int periodMs;
        int numberOfItems;
        int numberOfExceptions;
        int ratioOfExceptions;
        TestMode modeInhibition;
        bool withSound;
        std::string description;
        vector<string> createFixedSizeList(vector<string> & inputList, int n);
        vector<string> itemSequence; // sequence actually showed during the game

        // results (updated during the game)
        qint64 cumulatedClickTime; // used to compute average click speed
        int numberOfClick; // used to compute average click speed
        int errorsOnExceptions;
        int errorsOnRegularItems;

        // consolidated results (computed at the end of the game, or loaded from file)
        int averageClickSpeed;
        int correctRegularItems;
        int correctExceptions;
        int globalGrade;
        std::string datetime;
        bool isSame(const Scenario & other) const;
        std::string encoding;

        // force attributes: if the scenario description file
        // contains force-speed, etc.
        // then we store the info in these attributes, and they are
        // displayed in the control panel
        int forcePeriodMs;
        int forceNumberOfItems;
        int forceRatioOfExceptions;
        TestMode forceModeInhibition;
        std::string forceWithSound;

};


#endif
