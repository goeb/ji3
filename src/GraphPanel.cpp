#include <iostream>
#include <unistd.h>

using namespace std;


#include "Logger.hpp"
#include "GraphPanel.hpp"
#include "User.hpp"
#include "ResultTable.hpp"


GraphPanel::GraphPanel(const QString & username, const Scenario & refScenario)
{
    graph = new Graph();

    resize(800, 600);
    User u;
    u.load(username.toStdString());

    std::vector<Scenario> sList = refScenario.getSameScenario(u.getScenarioList());

    std::vector<int> clickSpeedCurve = getClickSpeedCurve(sList);
    Axis clickAxis;
    clickAxis.side = LEFT;
    clickAxis.labels << tr("Lent") << tr("Rapide");

    graph->addCurve(clickSpeedCurve, 0, 2000, tr("Vitesse de click"), true, clickAxis);

    Axis gradeAxis;
    gradeAxis.side = RIGHT;
    gradeAxis.labels << tr("0 %") << tr("100 %");

    std::vector<int> correctnessCurve = getGlobalGradeCurve(sList);
    graph->addCurve(correctnessCurve, 0, 100, tr("Réussite"), false, gradeAxis);

    // do the grid
    grid = new QVBoxLayout(this);

    grid->addWidget(graph);

    table = createTable(sList);
    grid->addWidget(table);

    closeButton = createButton(tr("Fermer"), SLOT(close()));
    grid->addWidget(closeButton);

    setLayout(grid);
    setWindowTitle("Jeu Inhibition 3 - graphe");
}

QTableWidget* GraphPanel::createTable(const std::vector<Scenario> & sList)
{
    QTableWidget *table = new QTableWidget();

    ResultTable::updateTable(table, sList, TABLE_GRAPH);

    return table;
}

GraphPanel::~GraphPanel()
{
    delete grid;
    delete table;
    delete closeButton;
}




void GraphPanel::close()
{
    LOG_DEBUG("close");
    QCoreApplication::exit(1); // back to main, that will actually start the game
}


QPushButton *GraphPanel::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void GraphPanel::keyReleaseEvent(QKeyEvent *e) {
    const char c = e->key();

    if ( c == 'q' || c == 'Q') close();
    else if (c == 'f' || c == 'F') {
        // toggle full screen
        if (isFullScreen()) showNormal();
        else showFullScreen();
    }

}


std::vector<int> GraphPanel::getClickSpeedCurve(const std::vector<Scenario> & sList)
{
    std::vector<int> result;
    std::vector<Scenario>::const_iterator s;
    for (s=sList.begin(); s!= sList.end(); s++) {
        result.push_back(s->getAverageClickSpeed());
    }
    return result;
}
std::vector<int> GraphPanel::getGlobalGradeCurve(const std::vector<Scenario> & sList)
{
    std::vector<int> result;
    std::vector<Scenario>::const_iterator s;
    for (s=sList.begin(); s!= sList.end(); s++) {
        result.push_back(s->getGlobalGrade());
    }
    return result;
}


