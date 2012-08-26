
#include "Graph.hpp"

Graph::Graph()
{
    scene = new QGraphicsScene(this);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);

    QGraphicsLineItem *line = new QGraphicsLineItem(QLine(0, 0, 100, 100));
    line->setPen(QPen(Qt::black, 5));
    scene->addItem(line);
    scene->setBackgroundBrush(Qt::green);

    //setSceneRect(scene->sceneRect());
    fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
    //qDebug() << "scene->sceneRect()=" << scene->sceneRect();
    //qDebug() << "view->sceneRect()=" << sceneRect();
    show();
}

Graph::~Graph()
{
    delete scene;
}

