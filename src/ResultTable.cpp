#include <iostream>
#include <unistd.h>

#include <QTranslator>
using namespace std;

#include "Logger.hpp"
#include "ResultTable.hpp"

#define tr(_x) QTranslator::tr(_x)

void ResultTable::updateTable(QTableWidget * table, const std::vector<Scenario> & scenarioList, enum TableStyle style)
{
    QStringList labels;

    table->clearContents();
    table->setRowCount(0);

    if (style == TABLE_GRAPH) labels << tr("Numéro");
    labels << tr("Date");
    if (style == TABLE_CONTROL) labels << tr("Thème") << tr("Son") << tr("Type") << tr("% Exc.") << tr("Vitesse") << tr("Long.");
    labels << tr("Vitesse\nde click") << tr("Score") << tr("Réussite\n items\nstandards")
           << tr("Réussite\nexception") << tr("Répartition");

    table->setColumnCount(labels.size());
    table->setHorizontalHeaderLabels(labels);
    table->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->setShowGrid(true);
    int row = 0;
    int rowNum = 1;


    // set tooltip for column errors
    int i = 0;
    if (style == TABLE_CONTROL) i = 11;
    else i = 6;
    QTableWidgetItem *x = table->horizontalHeaderItem(i);
    x->setToolTip(tr("début-milieu-fin"));

    std::vector<Scenario>::const_iterator s;
    QTableWidgetItem *wItem = 0;
    for (s = scenarioList.begin(); s != scenarioList.end(); s++) {
        table->insertRow(row);
        int col = 0;

        if (style == TABLE_GRAPH) {
            // Numero
            QString n = QString("%1").arg(rowNum);
            wItem = new QTableWidgetItem(n);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++;
        }

        // date
        const char * dt = s->getDatetime().c_str();
        wItem = new QTableWidgetItem(dt);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        if (style == TABLE_CONTROL) {
            // scenario name
            QString path(s->getPath().c_str());
            wItem = new QTableWidgetItem(path);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++;

            // sound
            QString sound = QString("%1").arg(s->getWithSound());
            wItem = new QTableWidgetItem(sound);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++;

            QString mode;
            if (s->getMode() == MODE_INHIBITION) mode = tr("inhibition");
            else mode = tr("attention");
            wItem = new QTableWidgetItem(mode);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++; //skip sound info;


            // % exception
            QString ex = QString("%1").arg(s->getRatioOfExceptions());
            wItem = new QTableWidgetItem(ex);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++;

            // speed
            double speed = s->getPeriodMs()/1000;
            QString sp = QString("%1").arg(speed, 0, 'f', 1);
            wItem = new QTableWidgetItem(sp);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++;

            // length
            QString n = QString("%1").arg(s->getNumberOfItems());
            wItem = new QTableWidgetItem(n);
            wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(row, col, wItem);
            col ++;
        }

        QString avcs = QString("%1").arg(s->getAverageClickSpeed());
        wItem = new QTableWidgetItem(avcs);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        QString gg = QString("%1").arg(s->getGlobalGrade());
        wItem = new QTableWidgetItem(gg);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        QString is = QString("%1").arg(s->getCorrectRegularItems());
        is += "/";
        is += QString("%1").arg(s->getNumberOfItems()-s->getNumberOfExceptions());
        wItem = new QTableWidgetItem(is);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        QString ip = QString("%1").arg(s->getCorrectExceptions());
        ip += "/";
        ip += QString("%1").arg(s->getNumberOfExceptions());
        wItem = new QTableWidgetItem(ip);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        wItem = new QTableWidgetItem(s->getErrorDistribution());
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        rowNum++;
    }

    for (int i=0; i<table->columnCount(); i++) table->resizeColumnToContents(i);
}


