#include <iostream>
#include <unistd.h>

using namespace std;

#include "Logger.hpp"
#include "SoundManager.hpp"
#include "Viewer.hpp"
#include "version.h"

// TODO move this elsewhere
const std::string dingFile = "Data/ding.wav";

std::string Viewer::getNextImage()
{
    if (index == items.size()) return std::string("");

    index++;
    return items[index-1];
}


void Image::setImage(const char * filepath)
{
    LOG_DEBUG("setImage: " << filepath);
    QString fileName = filepath;
    currentImage = QImage(fileName);
    currentText = fileName;
    if (currentImage.isNull()) {
        // display text
        fitTextToLabel();
    } else {
        fitImageToLabel();
    }
}
void Image::fitTextToLabel() {
    setText(currentText);
    QFont f = font();
    f.setPointSize(200);
    setFont(f);
}

void Image::resizeEvent ( QResizeEvent * event )
{
    if (currentImage.isNull()) {
        // display text
        fitTextToLabel();
    } else {
        fitImageToLabel();
    }
}
void Image::fitImageToLabel() {
    const QPixmap p = QPixmap::fromImage(currentImage);
    int w = width();
    int h = height();
    if (w>800) w = 800; // we do not want too big an image
    if (h>600) h = 600;
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
}


Viewer::Viewer(Scenario & s) : scenario(s), clickDisabled(false)
{
    items = s.getItemSequence();
    periodMs = s.getPeriodMs();
    index = 0;
    imageLabel = new Image;
    imageLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    distractor = 0;

    if (scenario.getMode() == MODE_DIVIDED_ATTENTION_VISUAL) {
        distractor = new QLabel(this);
        const QImage img("Data/lutin.png");
        const QPixmap p = QPixmap::fromImage(img);
        int w = width()/4;
        int h = height()/4;
        // set a scaled pixmap to a w x h window keeping its aspect ratio
        distractor->setPixmap(p.scaled(w, h, Qt::KeepAspectRatio));
        distractor->hide();
    } else if (scenario.getMode() == MODE_DIVIDED_ATTENTION_SOUND) {
        // nothing at this stage
    }
    else distractor = 0;

    nDistractor = 0;

    descriptionLabel = 0;
    setWindowTitle(QString("ji") + VERSION);
    start();
}



void Viewer::timerEvent(QTimerEvent *event)
{
    cout << "timerEvent\n";
    killTimer(pendingTimer);
    bool ok = scenario.evaluateUserClick(currentFile, false);
    if (!ok) {
        SoundManager::playSound(dingFile, DING_CHANNEL);
        usleep(300000); // TODO get actual length of the sound
    }
    pendingTimer = startTimer(periodMs);
    next(); // goto next image
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
    cout << "keyPressEvent: " << e->key() <<  "\n";
    if (clickDisabled) return;

    const char c = e->key();
    if ( c == 'q' || c == 'Q') QCoreApplication::exit(0);
    else if (c == ' ') {
        processUserClick();
    } else if (c == 'f' || c == 'F') {
        // toggle full screen
        if (isFullScreen()) showNormal();
        else showFullScreen();
    }

}

void Viewer::keyReleaseEvent(QKeyEvent *e)
{
    cout << "keyReleaseEvent: " << e->key() <<  "\n";
}
void Viewer::mousePressEvent(QMouseEvent *event)
{
    cout << "mousePressEvent";
    if (clickDisabled) return;
    processUserClick();
}

void Viewer::resizeEvent(QResizeEvent * event)
{
    //LOG_DEBUG("Viewer::resizeEvent");
}

void Viewer::processUserClick() {
    LOG_DEBUG("processUserClick, state=" << state);
    if (state == STARTING) {
        descriptionLabel->hide();
        //delete descriptionLabel;
        pendingTimer = startTimer(periodMs);
        setCentralWidget(imageLabel);
        state = CLICKING; // useless as next() makes state = RUNNING
        next();
    } else if (state == ENDING) {
        end();
    } else if (state == RUNNING) {
        state = CLICKING;

        qint64 clickTime = clickSpeedTimer.elapsed();
        LOG_DEBUG("clickTime=" << clickTime);
        scenario.addClickTime(clickTime);

        killTimer(pendingTimer);
        bool ok = scenario.evaluateUserClick(currentFile, true);
        if (!ok) {
            SoundManager::playSound(dingFile, DING_CHANNEL);
            usleep(300000);
        }
        pendingTimer = startTimer(periodMs);
        next(); // goto next image
    } else {
        // probably ENDING or CLICKING.
        // do nothing
    }
}

void Viewer::start()
{
    // display instructions of scenario
    state = STARTING;
    QString d = QString::fromStdString(scenario.getDescription());
    d += "\n\n\n\n\n";
    d += tr("Cliquer pour continuer.");
    d += "\n\n";
    d += tr("'f' pour plein écran\n'q' pour quitter");
    //scenario.getEncoding(); TODO ?

    descriptionLabel = new QLabel(this);
    descriptionLabel->setText(d);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setMinimumSize(300, 300);
    descriptionLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QFont f = descriptionLabel->font();
    f.setPointSize(16);
    descriptionLabel->setFont(f);

    setCentralWidget(descriptionLabel);
}


void Viewer::next()
{
    state = RUNNING;
    // goto next image
    currentFile = getNextImage();
    if (currentFile == "") {
        // end of the sequence
        // display the final page
        if (scenario.getMode() == MODE_DIVIDED_ATTENTION_VISUAL ||
                scenario.getMode() == MODE_DIVIDED_ATTENTION_SOUND) return dividedAttentionFinalPage();
        else return finalPage();
    }
    LOG_INFO("next image: " << currentFile);

    // get sound from image file // TODO improve search of sound
    std::string soundFile = currentFile.substr(0, currentFile.size()-4); // remove ".png"
    soundFile += ".wav";
    if (scenario.getWithSound()) SoundManager::playSound(soundFile, ITEM_CHANNEL);

    imageLabel->setImage(currentFile.c_str());

    if (scenario.getMode() == MODE_DIVIDED_ATTENTION_SOUND ||
            scenario.getMode() == MODE_DIVIDED_ATTENTION_VISUAL) {

        if (scenario.distractors[index]) {
            // the distractor must be showed or played

            if (scenario.getMode() == MODE_DIVIDED_ATTENTION_SOUND) {
                SoundManager::playSound("Data/coucou.wav", ITEM_CHANNEL);

            } else if (scenario.getMode() == MODE_DIVIDED_ATTENTION_VISUAL) {
                // display the distractor
                // chose a random position
                static int previousAlignment = 0;
                int alignment;
                Qt::Alignment align;
                // make sure 2 consecutive distractors do not show up at the same place
                while ( (alignment = (rand() % 8)) == previousAlignment) {}
                previousAlignment = alignment;
                switch (alignment) {
                case 0: align = Qt::AlignTop | Qt::AlignLeft; break;
                case 1: align = Qt::AlignTop | Qt::AlignHCenter; break;
                case 2: align = Qt::AlignTop | Qt::AlignRight; break;
                case 3: align = Qt::AlignVCenter | Qt::AlignLeft; break;
                case 4: align = Qt::AlignVCenter | Qt::AlignRight; break;
                case 5: align = Qt::AlignBottom | Qt::AlignLeft; break;
                case 6: align = Qt::AlignBottom | Qt::AlignHCenter; break;
                case 7: align = Qt::AlignBottom | Qt::AlignRight; break;
                }
                distractor->setAlignment(align);
                distractor->setGeometry(0, 0, width(), height());
                distractor->show();
                distractor->raise();
            }
        } else if (distractor) distractor->hide();
    } else if (distractor) distractor->hide();

    clickSpeedTimer.start();

}

void Viewer::finalPage() {
    // indicate that the scenario is finished
    state = ENDING;
    killTimer(pendingTimer);
    QString d = tr("Fin.\nCliquer pour continuer.");
    //scenario.getEncoding(); TODO ?

    descriptionLabel = new QLabel(this);
    descriptionLabel->setText(d);
    descriptionLabel->setFrameStyle(QFrame::Box);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setMinimumSize(300, 300);
    descriptionLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QFont f = descriptionLabel->font();
    f.setPointSize(16);
    descriptionLabel->setFont(f);

    setCentralWidget(descriptionLabel);

}


void Viewer::dividedAttentionFinalPage() {
    // collect user input about number of distractors seen
    state = ENDING;
    killTimer(pendingTimer);
    QString d = tr("Fin.\nIndiquer combien de distracteurs ont été vus.");
    //scenario.getEncoding(); TODO ?
    if (distractor) distractor->hide(); // case of the visual divided attention, not sonor

    clickDisabled = true; // disable click, force validation by button

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    descriptionLabel = new QLabel(this);
    descriptionLabel->setText(d);
    //descriptionLabel->setFrameStyle(QFrame::Box);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setFixedHeight(200);
    descriptionLabel->setFixedWidth(300);
    descriptionLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    vbox->addWidget(descriptionLabel);

    QFont f = descriptionLabel->font();
    f.setPointSize(18);
    descriptionLabel->setFont(f);

    nDistractorsInput = new QLineEdit();
    nDistractorsInput->setFont(f);
    nDistractorsInput->setMaximumWidth(100);
    nDistractorsInput->setFixedWidth(120);
    QRegExp expr("[0-9]+");
    QRegExpValidator *v = new QRegExpValidator(expr, 0);
    nDistractorsInput->setValidator(v);
    vbox->addWidget(nDistractorsInput, 0, Qt::AlignCenter);

    QPushButton *validateButton = new QPushButton(tr("Valider"));
    validateButton->setFont(f);
    validateButton->setFixedWidth(300);

    vbox->addWidget(validateButton);
    //setLayout(vbox);
    QWidget *widget = new QWidget();
    widget->setLayout(vbox);
    setCentralWidget(widget);
    nDistractorsInput->setFocus(Qt::OtherFocusReason);
    connect(validateButton, SIGNAL(clicked()), this, SLOT(end()));

    //setCentralWidget(vbox);
}

void Viewer::end() {
    if (scenario.getMode() == MODE_DIVIDED_ATTENTION_VISUAL ||
            scenario.getMode() == MODE_DIVIDED_ATTENTION_SOUND) {
        QString nStr = nDistractorsInput->text();
        int n = nStr.toInt();
        LOG_DEBUG("n distractors given by player: " << n);
        scenario.nDistractorsResponse = n-scenario.distractors.size();
    }


    scenario.consolidateResult();
    QCoreApplication::exit(1);
}
