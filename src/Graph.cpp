
#include "Graph.hpp"



Graph::Graph()
{
    setFixedSize(QSize(600, 400));


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

void Graph::addCurve(const std::vector<int> & points, int min, int max, const std::string & label, bool reverse)
{
    Curve c;
    c.points = points;

    if (min == max) {
        // compute min and max from given points
        c.min = c.max = 0;
        std::vector<int>::const_iterator p;
        for (p = points.begin(); p != points.end(); p++) {
            int value = *p;
            if (value > c.max) c.max = value;
            if (value < c.min) c.min = *p;
        }
    } else {
        c.min = min;
        c.max = max;
    }
    c.label = label;
    c.reverse = reverse;
    curves.push_back(c);

}


void Graph::paintEvent(QPaintEvent *e)
{
    qDebug() << "Graph::paintEvent, size=" << size();
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);


    int xMargin = 30;
    int yMargin = 30;

    // draw the curves
    std::vector<Curve>::iterator c;
    int colorIndex = 0;
    for (c = curves.begin(); c!= curves.end(); c++) {
        if (!c->points.size()) continue;

        QColor C = colors[colorIndex];

        // scaling
        int w = width();
        int h = height();
        float xScale = (float)(w - 2 * xMargin) / c->points.size();
        float yScale = (float)(h - 3 * yMargin) / (c->max - c->min);

        std::vector<int>::iterator point;
        int i = 0;
        QPoint p0;
        for (point = c->points.begin(); point != c->points.end(); point++) {
            QPoint p;
            p.setX(i*xScale + xMargin);
            if (c->reverse) p.setY((*point)*yScale + yMargin);
            else p.setY(h - (*point)*yScale - 2*yMargin);
            if (point == c->points.begin()) {
                // nothing
            } else {
                painter.setPen(QPen(C, 5));
                QLine line(p0, p);
                qDebug() << "Line=" << line;
                painter.drawLine(line);
            }

            // draw a circle
            QPen pen(C, 2);
            painter.setPen(pen);
            QBrush brush(C);
            painter.setBrush(brush);
            painter.drawEllipse(QRect(p.x()-4, p.y()-4, 8, 8));

            // draw x index (1, 2, 3 ,...)
            painter.setPen(QPen(Qt::black));
            int xIndex = i*xScale + xMargin;
            QRect position(xIndex, h - 30, xIndex + 20, h);
            painter.drawText(position, QString("%1").arg(i+1));


            i++;
            p0 = p;
        }
        //painter.drawLine(QLine(0, 0, w, 0));

        // write label
        QPoint pLabel(p0.x() + 8, p0.y());
        painter.drawText(pLabel, c->label.c_str());
        colorIndex = (colorIndex + 1) % colors.size();
    }
}


