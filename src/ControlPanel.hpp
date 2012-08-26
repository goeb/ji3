#ifndef _ControlPanel_hpp
#define _ControlPanel_hpp

#include <vector>
#include <string>
#include <QtGui>

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    ControlPanel();
    inline QString getPlayer() { return player->currentText(); }
    inline QString getScenario() { return scenario->currentText(); }
    inline bool getSound() { return soundOn->isChecked()?true:false; }
    inline int getType() { return typeInhibition->isChecked()?true:false;} // TODO manage 3rd mode
    inline int getNumber() { return n75->isChecked()?75:(n100->isChecked()?100:150); }
    inline int getPeriod() { return speed10->isChecked()?1000:
                                          (speed15->isChecked()?1500:
                                              (speed20->isChecked()?2000:
                                                  (speed25->isChecked()?2500:3000))); }
    inline int getRatio() { return ratio10->isChecked()?10:(ratio20->isChecked()?20:30); }


private slots:
    void start();
    void loadUser(QString text);

protected:
    void keyReleaseEvent(QKeyEvent *e);


private:
    QLabel *playerLabel;
    QComboBox *player;
    QLabel *scenarioLabel;
    QComboBox *scenario;
    QPushButton *goButton;
    QPushButton *createButton(const QString &text, const char *member);
    QGroupBox *createSoundGroup();
    QGroupBox *createTypeGroup();
    QGroupBox *createPercentageGroup();
    QGroupBox *createSpeedGroup();
    QGroupBox *createNumberGroup();

    // radio buttons
    QRadioButton *typeInhibition;
    QRadioButton *typeAttention;
    QRadioButton *typeDividedAttention;
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




};

#endif
