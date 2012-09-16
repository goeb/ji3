
#include "Graph.hpp"



Graph::Graph()
{
    setFixedSize(QSize(800, 400));


    //show();
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);
    //qDebug() << "Graph: size=" << size() << ", updatesEnabled=" << updatesEnabled ();
    //update();

    // prepare available colors
    colors.push_back(Qt::blue);
    colors.push_back(Qt::red);

}

Graph::~Graph()
{
}

void Graph::addCurve(const std::vector<int> & points, int min, int max, const QString & label, bool reverse, Axis axis)
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
    c.axis = axis;
    curves.push_back(c);

}


void Graph::paintEvent(QPaintEvent *e)
{
    //qDebug() << "Graph::paintEvent, size=" << size();
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);


    int xMarginLeft = 90;
    int xMarginRight = 80;
    int yMarginTop = 30;
    int yMarginBottom = 60;

    // draw the curves
    std::vector<Curve>::iterator c;
    int colorIndex = 0;
    for (c = curves.begin(); c!= curves.end(); c++) {
        if (!c->points.size()) continue;

        QColor C = colors[colorIndex];

        // scaling
        int w = width();
        int h = height();
        float xScale = (float)(w - (xMarginLeft+xMarginRight)) / c->points.size();
        float yScale = (float)(h - (yMarginTop + yMarginBottom)) / (c->max - c->min);

        std::vector<int>::iterator point;
        int i = 0;
        QPoint p0;
        for (point = c->points.begin(); point != c->points.end(); point++) {
            QPoint p;
            p.setX(i*xScale + xMarginLeft);

            int y = *point;
            if (y > c->max) y = c->max;
            else if (y < c->min) y = c->min;

            if (c->reverse) p.setY(y*yScale + yMarginTop);
            else p.setY(h - y*yScale - yMarginBottom);
            if (point == c->points.begin()) {
                // nothing
            } else {
                painter.setPen(QPen(C, 5));
                QLine line(p0, p);
                //qDebug() << "Line=" << line;
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
            int xIndex = i*xScale + xMarginLeft;
            QRect position(xIndex, h - 30, xIndex + 20, h);
            painter.drawText(position, QString("%1").arg(i+1));


            i++;
            p0 = p;

        }

        // write label
        QPoint pLabel(p0.x() + 8, p0.y());
        painter.drawText(pLabel, c->label);
        colorIndex = (colorIndex + 1) % colors.size();

        // draw the axis in the margins

        Axis a = c->axis;
        QPoint p1, p2;
        if (a.side == LEFT) {
            p1 = QPoint(xMarginLeft - 15, h);
            p2 = QPoint(xMarginLeft - 15, yMarginTop/2);
        } else {
            // RIGHT
            p1 = QPoint(w-xMarginRight, h);
            p2 = QPoint(w-xMarginRight, yMarginTop/2);
        }
        painter.setPen(QPen(C, 3));
        QLine line(p1, p2);
        //qDebug() << "Axis=" << line;
        painter.drawLine(line);
        // labels are distributed equally along the axis, starting at the bottom
        for (int i = 0; i < a.labels.size(); i++) {
            int yPos;
            if ((a.labels.size()-1) == 0) yPos = h;
            else yPos = h - i*(h-yMarginTop)/(a.labels.size()-1);

            int xPos;
            if (a.side == LEFT) xPos = 5;
            else xPos = p1.x() + 5; // RIGHT

            QRect position(xPos, yPos-20, 100, 30);
            painter.drawText(position, a.labels.at(i));
            //qDebug() << "label position[" << a.labels.at(i) << "]: " << position;
        }
    }



}


