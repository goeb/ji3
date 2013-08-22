#ifndef _Viewer_hpp
#define _Viewer_hpp

#include <vector>
#include <string>
#include <QtGui>
#include "ScenarioManager.hpp"

enum State {
    STARTING,
    RUNNING,
    CLICKING,
    ENDING
};

class Image : public QLabel
{
public:
    void setImage(const char * filepath);

protected:
    virtual void resizeEvent ( QResizeEvent * event );

private:
    QImage currentImage;
    void fitImageToLabel();
    void fitTextToLabel();
    QString currentText;

};

class Viewer : public QMainWindow
{
public:
    Viewer(Scenario & s);

protected:

    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent ( QResizeEvent * event );


private:
    std::string getNextImage();
    std::string currentFile;

    std::vector<std::string> items;
    unsigned int index;
    Image * imageLabel;
    QLabel * distractor; // used only for MODE_DIVIDED_ATTENTION
    int nDistractor;
    int pendingTimer;


    void processUserClick();
    void next();
    void start();
    void finalPage();
    void dividedAttentionFinalPage();
    void end();

    int periodMs; // milliseconds
    Scenario & scenario;
    QElapsedTimer clickSpeedTimer;
    QLabel * descriptionLabel;

    enum State state;

};

#endif
