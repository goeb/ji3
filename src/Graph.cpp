
#include "Graph.hpp"

Graph::Graph()
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    show();
}

Graph::~Graph()
{
    delete scene;
}

void Graph::addCurve(std::vector<int> points, int min, int max, const std::string & label)
{


    for (int i=1; i<points.size(); i++) {
        QGraphicsLineItem *line = new QGraphicsLineItem(QLine(i-1, points[i-1], i, points[i]));
        line->setPen(QPen(Qt::black, 0.05));
        scene->addItem(line);
        //scene->setBackgroundBrush(Qt::green);

        //scene->addEllipse(i, points[i], 2, 2);
    }
    // update display (scale, etc.)
    setSceneRect(scene->sceneRect());
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    qDebug() << "scene->sceneRect()=" << scene->sceneRect();
    qDebug() << "view->sceneRect()=" << sceneRect();

}


