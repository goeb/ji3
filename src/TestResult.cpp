
#include <stdlib.h>
#include <sstream>

#include "TestResult.hpp"
#include "Util.hpp"
#include "Logger.hpp"

const char * SEP = ";"; // field separator
const char * SEP2 = ","; // field separator for nested fields


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
