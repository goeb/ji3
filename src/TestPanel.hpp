#ifndef __TestPanel_hpp
#define __TestPanel_hpp

#ifdef _WIN32
  #include <windows.h>
#endif

#include <vector>
#include <set>
using namespace std;

#include <fltk/Group.h>
#include <fltk/Button.h>
using namespace fltk;
         
#include "ScenarioManager.hpp"
#include "User.hpp"
#include "DataSet.hpp"
#include "Util.hpp"


class TestPanel : public Group {
    private :
        TestPanel(int _x, int _y, int _width, int _height);

        void showIntro(); // show the first screen whith the instructions
        void hideIntro(); // show the first screen whith the instructions
        
        void startTest(); // start running the items

        void next(); // go to the next item
        bool evaluateUserAction(bool clicked); // look if the user did correct or wrong
        void playSound(string &itemId, bool isDing);
        void displayItem(string itemId);
        void darkenItem();
        void endScreen(); // show the end screen
        void playDing(); // play a 'ding' sound to indicate a mistake
        string getSoundFile(string itemId);

        // 
        enum { NOT_STARTED, RUNNING, ENDED, CLICK_PENDING, DING_PENDING } currentState;
        Widget * currentItem; // widget used to display the current item
        Widget * darkeningItem; // widget used to darken the item, and indicate the click
        Widget * counterItem;
        int counter;
        vector<string> listOfItems;
        int currentPosition; // position of the current item in the list
        int itemChannel;
        int dingChannel;
        
        int incorrectClicks;
        int incorrectIgnores;
        Scenario scenario;
        DataSet dataset;
        TestMode mode;
        User user;
        
        vector<pair<int, double> > resultTable; // table of (correctness, click-speed)
        void storeResult(bool correctness, bool hasClicked);
        void processClick(); // process the click of the user
        Timestamp t0; // used to compute elapsed time
        
        // scale an image so that it appears properly inside the window
        void scaleImage(int &imageWidth, int &imageHeight, int windowWidth, int windowHeight);
        
        int evaluateFinalScore();
        double evaluateClickSpeed();
    public :
        static TestPanel * TheTestPanel;
        static void start(Group * parent, User selectedUser, TestMode mode, DataSet selectedDataSet, Scenario selectedScenario);
        virtual int handle(int eventType);
};

#endif
