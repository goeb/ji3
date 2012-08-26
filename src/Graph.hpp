#ifndef _Graph_hpp
#define _Graph_hpp

#include <QtGui>

class Graph : public QGraphicsView
{
public:
    Graph();
    ~Graph();

private:
    QGraphicsScene *scene;

};

#endif
