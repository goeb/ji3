#include "config.h"
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
#include "GraphPanel.hpp"

void usage()
{
    std::cout << PACKAGE_STRING "\n"
                 "Usage:\n"
                 "  ji3 [OPTIONS] SCENARIO\n"
                 "\n"
                 "OPTIONS:\n"
                 "  -p PERIOD       Period in second between 2 items.\n"
                 "  -n N            Number of items showed (including exceptions).\n"
                 "  -x X            Ratio of exceptions showed (percent of N).\n"
                 "  -i              Mode Inhibition (player MUST NOT click on exceptions).\n"
                 "  -a              Mode Attention (player MUST click on exceptions).\n"
                 "  -ads            Mode Divided Attention with sound.\n"
                 "  -adv            Mode Divided Attention with visual.\n"
                 "                -i and -a are exclusive.\n"
                 "  --codec codec   Specify codec (UTF-8, latin1, etc.)\n"
                 "  -h              This help.\n"
                 "\n";
    exit(1);

}

int main(int argc, char **argv)
{
    Logger::init(".");
    SoundManager::init();



    LOG_INFO("Starting...");
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QString scenario;

    // set default values
    float period = 1; // seconds
    int numberOfItems = 10;
    int ratioOfExceptions = 10; // percent
    TestMode modeInhibition = MODE_ATTENTION;
    bool commandLine = false;
    const char * codec = 0;

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
        } else if (arg == "--codec") {
            i++; if (i >= n) usage();
            codec = args.at(i).toAscii().constData();
        } else if (arg == "-x") {
            i++; if (i >= n) usage();
            ratioOfExceptions = args.at(i).toInt();
        } else if (arg == "-a") {
            modeInhibition = MODE_ATTENTION;
        } else if (arg == "-i") {
            modeInhibition = MODE_INHIBITION;
        } else if (arg == "-ads") {
            modeInhibition = MODE_DIVIDED_ATTENTION_SOUND;
        } else if (arg == "-adv") {
            modeInhibition = MODE_DIVIDED_ATTENTION_VISUAL;
        } else if (arg == "-h") {
            usage();
        } else {
            // scenario
            scenario = args.at(i);
            commandLine = true;
        }
        i++;
    }

    if (codec) {
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName(codec));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName(codec));
    }

    ControlPanel *c = 0;
    QString player = "";
    bool playGame = false;
    bool showGraph = false;
    bool withSound = true;
    while (1) {
        showGraph = false;
        playGame = false;

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

            c->updateTable();
            c->show();
            int r = app.exec();
            LOG_DEBUG("after app.exec(), r=" << r);

            if (0 == r) { // ctrl-Q, or closed window
                // really quit
                exit(0);
            } else {
                if (2 == r) {
                    // show graph
                    showGraph = true;

                } else {
                    // play game
                    playGame = true;
                }

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
                withSound = c->getSound();
            }
        }

        Scenario s(scenario.toLocal8Bit().constData(), period*1000, numberOfItems, ratioOfExceptions, modeInhibition, withSound);
        QString filename = User::getUserfile(player);

        if (playGame) {
            if (c) c->hide();

            s.load();
            s.generateItemList();
            s.generateDistractors();

            Viewer imageViewer(s);
            //imageViewer.showFullScreen();
            imageViewer.show();
            imageViewer.resize(800,600);
            int r = app.exec();

            LOG_DEBUG("Return from Viewer: r=" << r);

            if (commandLine) exit(0);

            if (r == 0) { // closed window, of 'q' pressed
                // else 'q' pressed, do not store result into file
            } else {
                // nominal case
                if (!player.isEmpty()) {
                    // store to file
                    s.store(filename);

                    // go to curve diagram
                    showGraph = true;
                }
            }

        }

        if (showGraph) {
            if (c) c->hide();

            // from user name, get list of scenarios
            // from these scenarii, only keep those matching the last one
            // (so that the curve compares things comparable)
            GraphPanel x(filename, s);
            x.show();
            int r = app.exec();
        }
    }

}

