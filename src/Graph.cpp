
#include "Graph.hpp"

Graph::Graph()
{
    scene = new QGraphicsScene(this);
    //scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);



    std::vector<int> points;
    points.push_back(3);
    points.push_back(5);
    points.push_back(8);
    points.push_back(7);
    points.push_back(10);
    addCurve(points, 0, 10, "toto");

    setSceneRect(scene->sceneRect());
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    qDebug() << "scene->sceneRect()=" << scene->sceneRect();
    qDebug() << "view->sceneRect()=" << sceneRect();

    //scale(0.5, 0.5);
    //scale(0.5, 0.5);
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
}


