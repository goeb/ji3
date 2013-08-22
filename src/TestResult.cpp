
#include <stdlib.h>
#include <sstream>

#include "TestResult.hpp"
#include "Util.hpp"
#include "Logger.hpp"

const char * SEP = ";"; // field separator
const char * SEP2 = ","; // field separator for nested fields

bool TestResult::loadFromString(string line)
{
    vector<string> elements = Util::split(";", line);
    if (elements.size() != 7)
    {
        LOG_ERROR("Cannot load result: " << line);
        return false;
    }
    
    datasetFolder = elements[0];
    datasetCategory = elements[1];
    scenarioName = elements[2];
    
    if (0 == elements[3].compare("attention"))
    {
        attentionOrInhibition = MODE_ATTENTION;

    } else if (0 == elements[3].compare("inhibition")) {
        attentionOrInhibition = MODE_INHIBITION;

    } else {
        attentionOrInhibition=  MODE_DIVIDED_ATTENTION;
    }
    
    string correctnessStr = elements[4];
    correctness = atoi(correctnessStr.c_str());
    
    string clickSpeedStr = elements[5];
    clickSpeed = atof(clickSpeedStr.c_str());
    
    date = elements[6];

    return true;
}


string TestResult::serialize()
{
    ostringstream res;
    res << datasetFolder << SEP;
    res << datasetCategory << SEP;
    res << scenarioName << SEP;
    if (attentionOrInhibition == MODE_ATTENTION)
    {
        res << "attention" << SEP;

    } else if (attentionOrInhibition == MODE_INHIBITION) {
        res << "inhibition" << SEP;

    } else {
        res << "attention-divisee" << SEP;
    }
    res << correctness << SEP;
    res << clickSpeed << SEP;
    res << date;
    
    return res.str();
}
