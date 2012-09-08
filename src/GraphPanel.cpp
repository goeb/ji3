#include <iostream>
#include <unistd.h>

using namespace std;


#include "Logger.hpp"
#include "GraphPanel.hpp"
#include "User.hpp"



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

    graph->addCurve(clickSpeedCurve, 0, 0, tr("Vitesse de click").toLocal8Bit().constData(), true, clickAxis);

    Axis gradeAxis;
    gradeAxis.side = RIGHT;
    gradeAxis.labels << tr("0 %") << tr("100 %");

    std::vector<int> correctnessCurve = getGlobalGradeCurve(sList);
    graph->addCurve(correctnessCurve, 0, 100, tr("Réussite").toLocal8Bit().constData(), false, gradeAxis);

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
    QTableWidget *table = new QTableWidget(0, 6);
    QStringList labels;
    labels << QString::fromUtf8("Numéro") << tr("Date") << tr("Vitesse de click") << QString::fromUtf8("Réussite") << tr("Item standard") << tr("Item particulier");
    table->setHorizontalHeaderLabels(labels);
    table->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->setShowGrid(true);
    int row = 0;
    int row0 = 0;

    std::vector<Scenario>::const_iterator s;
    for (s=sList.begin(); s!=sList.end(); s++) {
        table->insertRow(row0);
        QTableWidgetItem *wItem;
        int col = 0;

        QString n = QString("%1").arg(row+1);
        wItem = new QTableWidgetItem(n);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row0, col, wItem);
        col ++;

        const char * dt = s->getDatetime().c_str();
        wItem = new QTableWidgetItem(dt);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row0, col, wItem);
        col ++;

        QString avcs = QString("%1").arg(s->getAverageClickSpeed());
        wItem = new QTableWidgetItem(avcs);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row0, col, wItem);
        col ++;

        QString gg = QString("%1").arg(s->getGlobalGrade());
        wItem = new QTableWidgetItem(gg);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row0, col, wItem);
        col ++;

        QString is = QString("%1").arg(s->getCorrectRegularItems());
        is += "/";
        is += QString("%1").arg(s->getNumberOfItems()-s->getNumberOfExceptions());
        wItem = new QTableWidgetItem(is);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row0, col, wItem);
        col ++;

        QString ip = QString("%1").arg(s->getCorrectExceptions());
        ip += "/";
        ip += QString("%1").arg(s->getNumberOfExceptions());
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        wItem = new QTableWidgetItem(ip);
        table->setItem(row0, col, wItem);
        col ++;

        row++;
    }

    for (int i=0; i<table->columnCount(); i++) table->resizeColumnToContents(i);
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


