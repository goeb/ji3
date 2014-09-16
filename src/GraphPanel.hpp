#ifndef _GraphPanel_hpp
#define _GraphPanel_hpp

#include <vector>
#include <string>
#include <QtGui>

#include "ScenarioManager.hpp"
#include "Graph.hpp"

class GraphPanel : public QWidget
{
    Q_OBJECT

public:
    GraphPanel(const QString &username, const Scenario &refScenario);
    ~GraphPanel();

private slots:
    void close();

protected:
    void keyReleaseEvent(QKeyEvent *e);

private:
    Graph *graph;
    QTableWidget *table;
    QPushButton *closeButton;
    QPushButton *createButton(const QString &text, const char *member);
    QVBoxLayout *grid;

    std::vector<int> getClickSpeedCurve(const std::vector<Scenario> & sList);
    std::vector<int> getGlobalGradeCurve(const std::vector<Scenario> & sList);
    std::vector<int> getDistractorCurve(const std::vector<Scenario> & sList);
    QTableWidget* createTable(const std::vector<Scenario> & sList);



};

#endif
