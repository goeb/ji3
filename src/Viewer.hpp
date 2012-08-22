#ifndef _Viewer_hpp
#define _Viewer_hpp

#include <vector>
#include <string>
#include <QtGui>
#include "ScenarioManager.hpp"

class Image : public QLabel
{
public:
    void setImage(const char * filepath);

protected:
    virtual void resizeEvent ( QResizeEvent * event );

private:
    QImage currentImage;
    void fitImageToLabel();
};

class Viewer : public QMainWindow
{
public:
    Viewer(Scenario & s);

protected:

    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    std::string getNextImage();
    std::string currentFile;

    std::vector<std::string> items;
    unsigned int index;
    Image * imageLabel;
    int pendingTimer;

    void processUserClick();
    void next();
    void end();

    int periodMs; // milliseconds
    Scenario & scenario;
    QElapsedTimer clickSpeedTimer;

};

#endif
