#ifndef _Graph_hpp
#define _Graph_hpp

#include <vector>
#include <QtGui>

class Graph : public QGraphicsView
{
public:
    Graph();
    ~Graph();

    void addCurve(std::vector<int> points, int min, int max, const std::string & label);

private:
    QGraphicsScene *scene;

};

#endif
