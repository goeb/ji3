#include <iostream>
#include <unistd.h>

using namespace std;

#include "Logger.hpp"
#include "GraphPanel.hpp"
#include "User.hpp"



GraphPanel::GraphPanel()
{

    grid = new QGridLayout;
    grid->setSizeConstraint(QLayout::SetNoConstraint);
    grid->addWidget(&graph, 0, 0);

    // table
    table = new QTableWidget(0, 1);
    QStringList labels;
    labels << tr("Date") << tr("Theme") << tr("Son") << tr("Type") << tr("% Exceptions") << tr("Vitesse") << tr("Longueur");
    table->setHorizontalHeaderLabels(labels);
    table->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->setShowGrid(true);
    int row = 0;

    table->insertRow(row);
    QTableWidgetItem *x1 = new QTableWidgetItem("01-01-2012 10:23");
    table->setItem(row, 0, x1);
    QTableWidgetItem *x2 = new QTableWidgetItem("Animaux/volants");
    table->setItem(row, 1, x2);

    row++;
    table->insertRow(row);
    QTableWidgetItem *x3 = new QTableWidgetItem("01-01-2012 10:45");
    table->setItem(row, 0, x3);
    QTableWidgetItem *x4 = new QTableWidgetItem("Animaux/volants");
    table->setItem(row, 1, x4);


    grid->addWidget(table, 2, 0, 1, 7);

    closeButton = createButton(tr("Fermer"), SLOT(close()));

    grid->addWidget(closeButton, 3, 1);

    setLayout(grid);
    setWindowTitle("Jeu Inhibition 3 - graphe");
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
}

