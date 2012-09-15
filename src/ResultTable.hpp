#ifndef _ResultTable_hpp
#define _ResultTable_hpp

#include <vector>
#include <string>
#include <QtGui>

#include "ScenarioManager.hpp"

enum TableStyle {
    TABLE_CONTROL, // specific display for control panel
    TABLE_GRAPH // specific display for graph panel
};

class ResultTable
{

public:
    static void updateTable(QTableWidget * table, const std::vector<Scenario> & scenarioList, enum TableStyle style);

};

#endif
