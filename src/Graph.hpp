#ifndef _Graph_hpp
#define _Graph_hpp

#include <vector>
#include <QtGui>

enum AxisSide { LEFT, RIGHT };
typedef struct {
    AxisSide side;
    QStringList labels; // labels are equally distributed along the axis
} Axis;

typedef struct {
    std::vector<int> points;
    int min;
    int max;
    QString label;
    bool reverse;
    Axis axis;
} Curve;



class Graph : public QFrame
{
    Q_OBJECT

public:
    Graph();
    ~Graph();

    void addCurve(const std::vector<int> & points, int min, int max, const QString & label, bool reverse, Axis axis);


protected:
    void paintEvent(QPaintEvent *);

private:
    std::vector<Curve> curves;
    std::vector<QColor> colors;

};

#endif
