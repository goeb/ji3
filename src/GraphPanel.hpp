#ifndef _GraphPanel_hpp
#define _GraphPanel_hpp

#include <vector>
#include <string>
#include <QtGui>

#include "ScenarioManager.hpp"
#include "Graph.hpp"

class GraphPanel : public QWidget
{
    Q_OBJECT

public:
    GraphPanel();
    ~GraphPanel();

private slots:
    void close();

protected:
    void keyReleaseEvent(QKeyEvent *e);

private:
    Graph graph;
    QTableWidget *table;
    QPushButton *closeButton;
    QPushButton *createButton(const QString &text, const char *member);
    QGridLayout *grid;

};

#endif
