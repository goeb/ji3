
#include "Graph.hpp"



Graph::Graph()
{
    resize(800, 600);

    //show();
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);
    qDebug() << "Graph: size=" << size() << ", updatesEnabled=" << updatesEnabled ();
    //update();

    // prepare available colors
    colors.push_back(Qt::blue);
    colors.push_back(Qt::red);

}

Graph::~Graph()
{
}

void Graph::addCurve(const std::vector<int> & points, int min, int max, const std::string & label)
{
    Curve c;
    c.points = points;

    if (min == max) {
        // compute min and max from given points
        c.min = c.max = 0;
        std::vector<int>::const_iterator p;
        for (p = points.begin(); p != points.end(); p++) {
            if (*p > c.max) c.max = *p;
            if (*p < c.min) c.min = *p;
        }
    } else {
        c.min = min;
        c.max = max;
    }
    c.label = label;
    curves.push_back(c);

}


void Graph::paintEvent(QPaintEvent *e)
{
    qDebug() << "Graph::paintEvent";
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);


    int xMargin = 10;
    int yMargin = 10;

    // draw the curves
    std::vector<Curve>::iterator c;
    int colorIndex = 0;
    for (c = curves.begin(); c!= curves.end(); c++) {
        if (!c->points.size()) continue;

        QColor C = colors[colorIndex];

        // scaling
        int w = width();
        int h = height();
        float xScale = (w - 2 * xMargin) / c->points.size();
        float yScale = (h - 2 * yMargin) / (c->max - c->min);

        std::vector<int>::iterator point;
        int i = 0;
        QPoint p0;
        for (point = c->points.begin(); point != c->points.end(); point++) {
            QPoint p;
            p.setX(i*xScale + xMargin);
            p.setY(h - (*point)*yScale - xMargin);
            if (point == c->points.begin()) {
                // nothing
            } else {
                painter.setPen(QPen(C, 5));
                QLine line(p0, p);
                qDebug() << "Line=" << line;
                painter.drawLine(line);
            }

            QPen pen(C, 2);
            painter.setPen(pen);
            QBrush brush(C);
            painter.setBrush(brush);
            painter.drawEllipse(QRect(p.x()-4, p.y()-4, 8, 8));
            i++;
            p0 = p;
        }

        // write label
        painter.drawText(p0, c->label.c_str());
        colorIndex = (colorIndex + 1) % colors.size();
    }
}


