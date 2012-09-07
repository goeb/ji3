#ifndef _Graph_hpp
#define _Graph_hpp

#include <vector>
#include <QtGui>

typedef struct {
    std::vector<int> points;
    int min;
    int max;
    std::string label;
} Curve;


class Graph : public QFrame
{
    Q_OBJECT

public:
    Graph();
    ~Graph();

    void addCurve(const std::vector<int> &points, int min, int max, const std::string & label);

protected:
    void paintEvent(QPaintEvent *);

private:
    std::vector<Curve> curves;
    std::vector<QColor> colors;

};

#endif
