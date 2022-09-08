#ifndef ENTITYLIST_H
#define ENTITYLIST_H

#include <QTreeView>

class EntityList : public QTreeView
{
public:
    EntityList();

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
};

#endif // ENTITYLIST_H
