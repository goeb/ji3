#include <iostream>
#include <unistd.h>

using namespace std;

#include "Logger.hpp"
#include "SoundManager.hpp"
#include "Viewer.hpp"

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


Viewer::Viewer(Scenario & s) : scenario(s)
{
    items = s.getItemSequence();
    periodMs = s.getPeriodMs();
    index = 0;
    imageLabel = new Image;
    imageLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    descriptionLabel = 0;

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
    d += "\n\n";
    d += tr("('f' pour plein écran, cliquer pour continuer)");
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
    if (currentFile == "") finalPage();
    LOG_INFO("next image: " << currentFile);

    // get sound from image file // TODO improve search of sound
    std::string soundFile = currentFile.substr(0, currentFile.size()-4); // remove ".png"
    soundFile += ".wav";
    if (scenario.getWithSound()) SoundManager::playSound(soundFile, ITEM_CHANNEL);

    imageLabel->setImage(currentFile.c_str());

    clickSpeedTimer.start();

}

void Viewer::finalPage() {
    // display instructions of scenario
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


void Viewer::end() {
    scenario.consolidateResult();
    QCoreApplication::exit(1);
}
