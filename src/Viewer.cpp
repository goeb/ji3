#include <iostream>
#include <unistd.h>

using namespace std;

#include "Logger.hpp"
#include "SoundManager.hpp"
#include "Viewer.hpp"

// TODO move this elsewhere
const std::string dingFile = "Animaux/ding.wav";

std::string Viewer::getNextImage()
{
    if (index == items.size()) return std::string("");

    index++;
    return items[index-1];
}


void Image::setImage(const char * filepath) {
    LOG_DEBUG("setImage: " << filepath);
    QString fileName = filepath;
    QImage image(fileName);
    QPixmap p = QPixmap::fromImage(image);
    setPixmap(p);
    currentImage = image;
    fitImageToLabel();
}


void Image::resizeEvent ( QResizeEvent * event ) {
    fitImageToLabel();
}
void Image::fitImageToLabel() {
    const QPixmap p = QPixmap::fromImage(currentImage);
    int w = width();
    int h = height();
    if (w>800) w = 800; // we do not want too big an image
    if (h>600) h = 600;
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
}


Viewer::Viewer(Scenario & s) : scenario(s) {
    items = s.getItemSequence();
    periodMs = s.getPeriodMs();
    index = 0;
    pendingTimer = startTimer(periodMs);
    imageLabel = new Image;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    setCentralWidget(imageLabel);
    next();
}



void Viewer::timerEvent(QTimerEvent *event) {
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
void Viewer::keyPressEvent(QKeyEvent *e) {
    cout << "keyPressEvent: " << e->key() <<  "\n";
}
void Viewer::keyReleaseEvent(QKeyEvent *e) {
    cout << "keyReleaseEvent: " << e->key() <<  "\n";
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
void Viewer::mouseReleaseEvent(QMouseEvent *event) {
    cout << "mouseReleaseEvent\n";
    processUserClick();
}


void Viewer::processUserClick() {
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
}

void Viewer::next() {
    // goto next image
    currentFile = getNextImage();
    if (currentFile == "") end();
    LOG_INFO("next image: " << currentFile);

    // get sound from image file
    std::string soundFile = currentFile.substr(0, currentFile.size()-4); // remove ".png"
    soundFile += ".wav";
    SoundManager::playSound(soundFile, ITEM_CHANNEL);

    imageLabel->setImage(currentFile.c_str());

    clickSpeedTimer.start();

}

void Viewer::end() {
    scenario.consolidateResult();
    QCoreApplication::exit(0);
}
