#include <iostream>
#include <unistd.h>

using namespace std;
#include <QtGui>
#include <QtCore/qstate.h>

#include "Logger.hpp"
#include "SoundManager.hpp"
#include "Viewer.hpp"
#include "ScenarioManager.hpp"
#include "ControlPanel.hpp"
#include "Graph.hpp"

void usage()
{
    std::cout << "Usage:\n"
                 "ji3 [OPTIONS] SCENARIO\n"
                 "OPTIONS:\n"
                 "-p PERIOD       Period in second between 2 items.\n"
                 "-n N            Number of items showed (including exceptions).\n"
                 "-x X            Ratio of exceptions showed (percent of N).\n"
                 "-i              Inhibition mode (player MUST NOT click on exceptions).\n"
                 "-a              Attention mode (player MUST click on exceptions).\n"
                 "                -i and -a are exclusive.\n"
                 "\n";
    exit(1);

}

int main(int argc, char **argv)
{
    Logger::init(".");
    SoundManager::init();

    LOG_INFO("Starting...");
    QApplication app(argc, argv);
    QString scenario;

    // set default values
    float period = 1; // seconds
    int numberOfItems = 10;
    int ratioOfExceptions = 10; // percent
    bool modeInhibition = true;
    bool commandLine = false;


    QStringList args = QCoreApplication::arguments();
    int n = args.size();
    int i = 1;
    while (i<n) {
        QString arg = args.at(i);
        //LOG_DEBUG("arg=" << arg.toLocal8Bit().constData());
        if (arg == "-p") { // period
            i++; if (i >= n) usage();
            period = args.at(i).toFloat();
        } else if (arg == "-n") {
            i++; if (i >= n) usage();
            numberOfItems = args.at(i).toInt();
        } else if (arg == "-x") {
            i++; if (i >= n) usage();
            ratioOfExceptions = args.at(i).toInt();
        } else if (arg == "-a") {
            modeInhibition = false;
        } else if (arg == "-i") {
            modeInhibition = true;
        } else {
            // scenario
            scenario = args.at(i);
            commandLine = true;
        }
        i++;
    }

    ControlPanel *c = 0;
    QString player = "";
    bool playGame = false;
    bool showGraph = false;
    while (1) {
        if (commandLine) {
            // command line mode:
            // start immediately test with values given on the command line.

            LOG_DEBUG("scenario=" << scenario.toLocal8Bit().constData() << ", period=" << period <<
                      ", numberOfItems=" << numberOfItems << ", ratioOfExceptions=" << ratioOfExceptions <<
                      ", modeInhibition=" << modeInhibition);

            playGame = true;
        } else {

            // show control panel
            LOG_INFO("Show Contol Panel");

            if (!c) c = new ControlPanel();

            c->show();
            int r = app.exec();
            LOG_DEBUG("after app.exec(), r=" << r);
            if (0 == r) { // ctrl-Q, really quit
                exit(0);
            } else if (2 == r) {
                // show graph
                playGame = false;
                showGraph = true;

            } else {
                playGame = true;

                // retrieve values from object 'c'.
                LOG_DEBUG("player=" << c->getPlayer().toLocal8Bit().constData() <<
                          ", scenario=" << c->getScenario().toLocal8Bit().constData() <<
                          ", sound=" <<  c->getSound());
                ratioOfExceptions = c->getRatio();
                period = c->getPeriod()/1000;
                numberOfItems = c->getNumber();
                modeInhibition = c->getType();
                scenario = c->getScenario();
                player = c->getPlayer();


            }
        }

        if (playGame) {
            if (c) c->hide();
            //numberOfItems = 10 ; // TODO remove me
            Scenario s(scenario.toLocal8Bit().constData(), period*1000, numberOfItems, ratioOfExceptions, modeInhibition);
            s.load();
            s.generateItemList();

            Viewer imageViewer(s);
            imageViewer.showFullScreen();
            bool r = app.exec();

            if (commandLine) exit(0);

            if (r == 0) { // nominal case
                if (!player.isEmpty()) {
                    // store to file
                    QString filename = "Data/" + player + ".ji3u";
                    s.store(filename);

                    // go to curve diagram
                    // TODO
                }
            } // else 'q' pressed, do not store result into file
        } else if (showGraph) {
            if (c) c->hide();

            Graph x;
            bool r = app.exec();

        }
    }

}

