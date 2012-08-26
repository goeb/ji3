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
    playerLabel = new QLabel("Joueur : ");
    player = new QComboBox;
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
    scenarioLabel = new QLabel("Theme : ");
    scenario = new QComboBox;
    scenario->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scenario->addItem("Animaux/rampants.conf"); // TODO
    scenario->addItem("Animaux/volants.conf");
    scenario->addItem("Couleurs/rouge.conf");
    scenario->addItem("Instruments");
    scenario->addItem("Maison/cuisine");
    scenario->addItem("Test Normal");
    goButton = createButton(tr("Commencer"), SLOT(start()));


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
    QTableWidget *filesTable = new QTableWidget(0, 7);
    QStringList labels;
    labels << tr("Date") << tr("Theme") << tr("Son") << tr("Type") << tr("% Exceptions") << tr("Vitesse") << tr("Longueur");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(true);
    int row = 0;

    filesTable->insertRow(row);
    QTableWidgetItem *x1 = new QTableWidgetItem("01-01-2012 10:23");
    filesTable->setItem(row, 0, x1);
    QTableWidgetItem *x2 = new QTableWidgetItem("Animaux/volants");
    filesTable->setItem(row, 1, x2);

    row++;
    filesTable->insertRow(row);
    QTableWidgetItem *x3 = new QTableWidgetItem("01-01-2012 10:45");
    filesTable->setItem(row, 0, x3);
    QTableWidgetItem *x4 = new QTableWidgetItem("Animaux/volants");
    filesTable->setItem(row, 1, x4);



    grid->addWidget(filesTable, 2, 0, 1, 7);

    grid->addWidget(goButton, 3, 1);

    Graph *x = new Graph();
    grid->addWidget(x, 4, 1, 2, 3);
    setLayout(grid);
    setWindowTitle("Jeu Inhibition 3");
    //resize(700, 300);
}

void ControlPanel::loadUser(QString text)
{
    // load user inforation, and update widgets
    LOG_DEBUG("loadUser" << text.toLocal8Bit().constData());

    std::string userName = text.toLocal8Bit().constData();
    User *u = User::getUserbyName(userName);
    vector<Scenario> sList = u->getScenarioList();

    if (sList.size() > 0) {
        // last scenario gives the defaut values
        Scenario s = sList[sList.size()-1];
        int n = s.getNumberOfItems();
        if (n == 150) n150->setChecked(true);
        else if (n == 100) n100->setChecked(true);
        else n75->setChecked(true);
    }
}

void ControlPanel::start()
{
    LOG_DEBUG("start");
    QCoreApplication::exit(0); // back to main, that will actually start the game
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
    n75 = new QRadioButton(tr("Court (75 items)"));
    n100 = new QRadioButton(tr("Long (100 items)"));
    n150 = new QRadioButton(tr("Tres long (150 items)"));

    n75->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
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

    typeInhibition = new QRadioButton(tr("Inhibition"));
    typeAttention = new QRadioButton(tr("Attention"));
    typeDividedAttention = new QRadioButton(tr("Attention divisee"));

    typeInhibition->setChecked(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(typeInhibition);
    vbox->addWidget(typeAttention);
    vbox->addWidget(typeDividedAttention);
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
        if (mod & Qt::ControlModifier) QCoreApplication::exit(1); // ctrl-Q
    }
}

