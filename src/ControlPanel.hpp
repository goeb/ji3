#ifndef _ControlPanel_hpp
#define _ControlPanel_hpp

#include <vector>
#include <string>
#include <QtGui>

#include "ScenarioManager.hpp"
#include "User.hpp"

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel();
    inline QString getPlayer() { return player->currentText(); }
    inline QString getScenario() { return scenario->currentText(); }
    inline bool getSound() { return soundOn->isChecked()?true:false; }
    inline TestMode getType() {
        if (modeInhibition->isChecked()) return MODE_INHIBITION;
        else if (modeAttention->isChecked()) return MODE_ATTENTION;
        else return MODE_DIVIDED_ATTENTION;
    }
    inline int getNumber() { return n10->isChecked()?10:
                                        (n75->isChecked()?75:n100->isChecked()?100:150); } // TODO 10->75
    inline int getPeriod() { return speed10->isChecked()?1000:
                                          (speed15->isChecked()?1500:
                                              (speed20->isChecked()?2000:
                                                  (speed25->isChecked()?2500:3000))); }
    inline int getRatio() { return ratio10->isChecked()?10:(ratio20->isChecked()?20:30); }

    void updateTable();

private slots:
    void start();
    void quit();
    void showGraph();

    void loadUser(QString text);
    void checkScenarioForceValues(QString name);


signals:
        void resizeTableColumns();
protected:
    void keyReleaseEvent(QKeyEvent *e);


private:
    QLabel *playerLabel;
    QComboBox *player;
    QLabel *scenarioLabel;
    QComboBox *scenario;
    QPushButton *goButton;
    QPushButton *showGraphButton;
    QPushButton *quitButton;
    QPushButton *createButton(const QString &text, const char *member);
    QGroupBox *createSoundGroup();
    QGroupBox *createTypeGroup();
    QGroupBox *createPercentageGroup();
    QGroupBox *createSpeedGroup();
    QGroupBox *createNumberGroup();

    // radio buttons
    QRadioButton *modeInhibition;
    QRadioButton *modeAttention;
    QRadioButton *modeDividedAttention;
    QRadioButton *n10;
    QRadioButton *n75;
    QRadioButton *n100;
    QRadioButton *n150;
    QRadioButton *speed10;
    QRadioButton *speed15;
    QRadioButton *speed20;
    QRadioButton *speed25;
    QRadioButton *speed30;
    QRadioButton *ratio10;
    QRadioButton *ratio20;
    QRadioButton *ratio30;
    QRadioButton *soundOn;
    QRadioButton *soundOff;

    void updateDefaultValues(Scenario s);
    QTableWidget *table;
    void updateTable(const User * u);
    std::string userName ;




};

#endif
