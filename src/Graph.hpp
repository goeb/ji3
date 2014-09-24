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

enum GraphType { GR_CURVE, GR_HISTOGRAM };

class Graph : public QFrame
{
    Q_OBJECT

public:
    Graph(GraphType type = GR_CURVE, int colorIndexStart = 0);
    ~Graph();

    void addCurve(const std::vector<int> & points, int min, int max, const QString & label, bool reverse, Axis axis);


protected:
    void paintEvent(QPaintEvent *);

private:
    QPoint getPixel(int x, int xScale, int y, int yScale, int ymin, int ymax);
    void paintHistogram(QPaintEvent *);
    void paintCurve(QPaintEvent *);

    std::vector<Curve> curves;
    std::vector<QColor> colors;
    GraphType graphType;
    int colorIndexStart;
    int xMarginLeft;
    int xMarginRight;
    int yMarginTop;
    int yMarginBottom;

};

#endif
