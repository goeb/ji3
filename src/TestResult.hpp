
#ifndef __TestResult_hpp
#define __TestResult_hpp

#include <iostream>
#include <vector>
using namespace std;

#include "ScenarioManager.hpp"
        
class TestResult
{
    // hold the result of 1 test
    public:
        string date; // format 2009-10-21 19:03:12
        int correctness; // how many correct strating clicks
        double clickSpeed; // unit: second
        vector<int> detailedResult; // which clicks were correct/wrong
        TestMode attentionOrInhibition;
        string datasetFolder;
        string datasetCategory;
        string scenarioName; // name of the scenario
        
        bool loadFromString(string s); // fulfil the current instance
        string serialize(); // build the string, ready for storing in file
};

#endif
