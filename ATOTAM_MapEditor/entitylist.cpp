#include "entitylist.h"

#include <QDrag>
#include <QMimeData>

EntityList::EntityList()
{

}

void EntityList::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (selectionModel()->hasSelection()) {
            QDrag *drag = new QDrag(this);
            QMimeData *data = new QMimeData;
            QStandardItem* selected = dynamic_cast<QStandardItemModel*>(model())->itemFromIndex(selectionModel()->selectedIndexes()[0]);
            data->setText(selected->parent()->text() + ":" + selected->text());
            drag->setMimeData(data);
            drag->exec();
        }
    }
}


void EntityList::dragEnterEvent(QDragEnterEvent *event)
{
    event->setAccepted(false);
}
