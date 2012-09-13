#include <iostream>
#include <unistd.h>

using namespace std;

#include "Logger.hpp"
#include "ControlPanel.hpp"
#include "User.hpp"

#include "Graph.hpp"

ControlPanel::ControlPanel()
{
    User::loadUsers("Data");
    // player name
    playerLabel = new QLabel(tr("Nom : "));
    QFont f = playerLabel->font();
    f.setPointSize(f.pointSize()*2);
    playerLabel->setFont(f);

    player = new QComboBox;

    // increase font size
    f = player->font();
    f.setPointSize(f.pointSize()*2);
    player->setFont(f);

    player->setEditable(true);
    player->addItem("");
    connect(player, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadUser(QString)));


    // add found users
    vector<User> users = User::getUsers();
    vector<User>::iterator u;
    for (u = users.begin(); u != users.end(); u++) {
        const char* userName = u->getName().c_str();
        player->addItem(userName);
    }
    player->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // theme
    scenarioLabel = new QLabel(tr("Thème : "));
    scenario = new QComboBox;
    scenario->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QStringList scenarioFiles = Scenario::retrieveScenarios();
    for (int i = 0; i < scenarioFiles.size(); i++) {
        QString sName = scenarioFiles.at(i);
        sName = sName.replace(SCENARIO_DESCRIPTOR_SUFFIX, "");
        scenario->addItem(sName);
    }
    connect(scenario, SIGNAL(currentIndexChanged(QString)), this, SLOT(checkScenarioForceValues(QString)));


    goButton = createButton(tr("Commencer"), SLOT(start()));
    //goButton->setDefault(true);
    showGraphButton = createButton(tr("Courbe"), SLOT(showGraph()));
    quitButton = createButton(tr("Quitter"), SLOT(quit()));

    QGridLayout *grid = new QGridLayout;
    grid->setSizeConstraint(QLayout::SetNoConstraint);
    grid->addWidget(playerLabel, 0, 0);
    grid->addWidget(player, 0, 1, 1, 6);
    grid->addWidget(scenarioLabel, 1, 0);
    grid->addWidget(scenario, 1, 1);

    grid->addWidget(createSoundGroup(), 1, 2);
    grid->addWidget(createTypeGroup(), 1, 3);
    grid->addWidget(createPercentageGroup(), 1, 4);
    grid->addWidget(createSpeedGroup(), 1, 5);
    grid->addWidget(createNumberGroup(), 1, 6);


    // table
    table = new QTableWidget();
    connect(this, SIGNAL(resizeTableColumns()), table, SLOT(resizeColumnsToContents()));

    grid->addWidget(table, 2, 0, 1, 7);

    grid->addWidget(goButton, 3, 1);
    grid->addWidget(showGraphButton, 3, 2);
    grid->addWidget(quitButton, 3, 3);

    setLayout(grid);
    setWindowTitle("Jeu Inhibition 3");
    //resize(700, 300);
}

void ControlPanel::updateTable()
{
    User u;
    u.load(User::getUserfile(userName));
    updateTable(&u);
}

void *ControlPanel::updateTable(const User * u)
{
    QStringList labels;

    table->clearContents();
    table->setColumnCount(11);
    table->setRowCount(0);


    labels << tr("Date") << tr("Thème") << tr("Son") << tr("Type") << tr("% Exc.") << tr("Vitesse") << tr("Long.")  <<
              tr("Vitesse\nde click") << tr("Score") << tr("Réussite\n items\nstandards") << tr("Réussite\nexception");
    table->setHorizontalHeaderLabels(labels);
    table->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    table->verticalHeader()->hide();
    table->setShowGrid(true);
    int row = 0;

    if (!u) return table;


    std::vector<Scenario> sList = u->getScenarioList();
    std::vector<Scenario>::iterator s;
    QTableWidgetItem *wItem = 0;
    for (s=sList.begin(); s!=sList.end(); s++) {
        table->insertRow(row);
        int col = 0;

        const char * dt = s->getDatetime().c_str();
        wItem = new QTableWidgetItem(dt);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

        QString path(s->getPath().c_str());
        wItem = new QTableWidgetItem(path);
        wItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(row, col, wItem);
        col ++;

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

        //row++;
    }


    //emit resizeTableColumns();

    for (int i=0; i<table->columnCount(); i++) table->resizeColumnToContents(i);
    return table;
}
void ControlPanel::updateDefaultValues(Scenario s)
{
    // scenario name
    std::string sName = s.getPath();
    int i = scenario->findText(QString(sName.c_str()), Qt::MatchExactly);
    if (i == -1) {
        // not found in list. do not update scenario combo box.
    } else {
        scenario->setCurrentIndex(i);
    }

    // sound TODO
    if (s.getWithSound()) soundOn->setChecked(true);
    else soundOff->setChecked(true);

    // mode inhibition/attention
    if (s.getMode() == MODE_INHIBITION) modeInhibition->setChecked(true);
    else modeAttention->setChecked(true);

    // ratio of exceptions
    int r = s.getRatioOfExceptions();
    if (r == 10) ratio10->setChecked(true);
    else if (r == 20) ratio20->setChecked(true);
    else if (r == 30) ratio30->setChecked(true);
    else {
        LOG_ERROR("Invalid ratio of exception: " << r);
    }

    // speed
    int period = s.getPeriodMs();
    if (period == 1000) speed10->setChecked(true);
    else if (period == 1000) speed10->setChecked(true);
    else if (period == 1500) speed15->setChecked(true);
    else if (period == 2000) speed20->setChecked(true);
    else if (period == 2500) speed25->setChecked(true);
    else if (period == 3000) speed30->setChecked(true);
    else {
        LOG_ERROR("Invalid speed: " << period);
    }

    // number of items
    int n = s.getNumberOfItems();
    if (n == 150) n150->setChecked(true);
    else if (n == 100) n100->setChecked(true);
    else if (n == 10) n10->setChecked(true);
    else n75->setChecked(true);

}

void ControlPanel::checkScenarioForceValues(QString name)
{
    Scenario s;
    bool r = s.load(name.toLocal8Bit().constData());
    if (!r) return;

    if (s.getForcePeriodMs() == 1000) {
        speed10->setChecked(true);
        speed15->setEnabled(false);
        speed20->setEnabled(false);
        speed25->setEnabled(false);
        speed30->setEnabled(false);

    } else if (s.getForcePeriodMs() == 1500) {
        speed15->setChecked(true);
        speed10->setEnabled(false);
        speed20->setEnabled(false);
        speed25->setEnabled(false);
        speed30->setEnabled(false);

    } else if (s.getForcePeriodMs() == 2000) {
        speed20->setChecked(true);
        speed10->setEnabled(false);
        speed15->setEnabled(false);
        speed25->setEnabled(false);
        speed30->setEnabled(false);

    } else if (s.getForcePeriodMs() == 2500) {
        speed25->setChecked(true);
        speed10->setEnabled(false);
        speed15->setEnabled(false);
        speed20->setEnabled(false);
        speed30->setEnabled(false);

    } else if (s.getForcePeriodMs() == 3000) {
        speed30->setChecked(true);
        speed10->setEnabled(false);
        speed15->setEnabled(false);
        speed20->setEnabled(false);
        speed25->setEnabled(false);

    } else {
        // activate all
        speed10->setEnabled(true);
        speed15->setEnabled(true);
        speed20->setEnabled(true);
        speed25->setEnabled(true);
        speed30->setEnabled(true);
    }

    if (s.getForceRatioOfExceptions() == 10) {
        ratio10->setChecked(true);
        ratio20->setEnabled(false);
        ratio30->setEnabled(false);
    } else if (s.getForceRatioOfExceptions() == 20) {
        ratio20->setChecked(true);
        ratio10->setEnabled(false);
        ratio30->setEnabled(false);
    } else if (s.getForceRatioOfExceptions() == 30) {
        ratio30->setChecked(true);
        ratio20->setEnabled(false);
        ratio10->setEnabled(false);
    } else {
        // activate all
        ratio10->setEnabled(true);
        ratio20->setEnabled(true);
        ratio30->setEnabled(true);
    }

    if (s.getForceModeInhibition() == MODE_INHIBITION) {
        modeInhibition->setChecked(true);
        modeAttention->setEnabled(false);
    } else if (s.getForceModeInhibition() == MODE_ATTENTION) {
        modeAttention->setChecked(true);
        modeInhibition->setEnabled(false);
    } else {
        modeInhibition->setEnabled(true);
        modeAttention->setEnabled(true);
    }

    if (s.getForceNumberOfItems() == 75) {
        n10->setEnabled(false);
        n75->setChecked(true);
        n100->setEnabled(false);
        n150->setEnabled(false);
    } else if (s.getForceNumberOfItems() == 100) {
        n10->setEnabled(false);
        n75->setEnabled(false);
        n100->setChecked(true);
        n150->setEnabled(false);
    } else if (s.getForceNumberOfItems() == 150) {
        n10->setEnabled(false);
        n75->setEnabled(false);
        n100->setEnabled(false);
        n150->setChecked(true);
    } else {
        n10->setEnabled(true);
        n75->setEnabled(true);
        n100->setEnabled(true);
        n150->setEnabled(true);
    }

    if (s.getForceWithSound() == "on") {
        soundOn->setChecked(true);
        soundOff->setEnabled(false);
    } else if (s.getForceWithSound() == "off") {
        soundOn->setEnabled(false);
        soundOff->setChecked(true);
    } else {
        soundOn->setEnabled(true);
        soundOff->setEnabled(true);
    }

}


void ControlPanel::loadUser(QString text)
{
    // load user inforation, and update widgets
    LOG_DEBUG("loadUser" << text.toLocal8Bit().constData());

    userName = text.toLocal8Bit().constData();
    User *u = User::getUserbyName(userName);

    if (u) {
        vector<Scenario> sList = u->getScenarioList();

        if (sList.size() > 0) {
            // last scenario gives the defaut values
            updateDefaultValues(sList[sList.size()-1]);
        }

        // update graph

    }
    // update table of all record for that user
    updateTable(u);

}

void ControlPanel::start()
{
    LOG_DEBUG("start");
    QCoreApplication::exit(1); // back to main, that will actually start the game
}

void ControlPanel::quit()
{
    LOG_DEBUG("quit");
    exit(0);
}



void ControlPanel::showGraph()
{
    LOG_DEBUG("showGraph");
    QCoreApplication::exit(2);
}

QGroupBox *ControlPanel::createSoundGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("Son"));
    soundOn = new QRadioButton(tr("Avec"));
    soundOff = new QRadioButton(tr("Sans"));

    soundOn->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(soundOn);
    vbox->addWidget(soundOff);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}


QGroupBox *ControlPanel::createPercentageGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("% Exceptions"));
    ratio10 = new QRadioButton(tr("10 %"));
    ratio20 = new QRadioButton(tr("20 %"));
    ratio30 = new QRadioButton(tr("30 %"));

    ratio10->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(ratio10);
    vbox->addWidget(ratio20);
    vbox->addWidget(ratio30);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}
QGroupBox *ControlPanel::createSpeedGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("Vitesse"));
    speed10 = new QRadioButton(tr("1 s"));
    speed15 = new QRadioButton(tr("1,5 s"));
    speed20 = new QRadioButton(tr("2 s"));
    speed25 = new QRadioButton(tr("2,5 s"));
    speed30 = new QRadioButton(tr("3 s"));

    speed10->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(speed10);
    vbox->addWidget(speed15);
    vbox->addWidget(speed20);
    vbox->addWidget(speed25);
    vbox->addWidget(speed30);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}

QGroupBox *ControlPanel::createNumberGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("Longueur"));
    n10 = new QRadioButton(tr("Très court (10 items)"));
    n75 = new QRadioButton(tr("Court (75 items)"));
    n100 = new QRadioButton(tr("Long (100 items)"));
    n150 = new QRadioButton(tr("Tres long (150 items)"));

    n75->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(n10);
    vbox->addWidget(n75);
    vbox->addWidget(n100);
    vbox->addWidget(n150);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}


QGroupBox *ControlPanel::createTypeGroup()
{
    QGroupBox *groupBox = new QGroupBox(tr("Type"));

    modeInhibition = new QRadioButton(tr("Inhibition"));
    modeAttention = new QRadioButton(tr("Attention"));
    modeDividedAttention = new QRadioButton(tr("Attention divisée"));
    modeDividedAttention->setEnabled(0);

    modeInhibition->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(modeInhibition);
    vbox->addWidget(modeAttention);
    vbox->addWidget(modeDividedAttention);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;
}

QPushButton *ControlPanel::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void ControlPanel::keyReleaseEvent(QKeyEvent *e) {
    const char c = e->key();
    Qt::KeyboardModifiers mod = e->modifiers();
    if ( c == 'q' || c == 'Q') {
        if (mod & Qt::ControlModifier) QCoreApplication::exit(0); // ctrl-Q
    }
}

