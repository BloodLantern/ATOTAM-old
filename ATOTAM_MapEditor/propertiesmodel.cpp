#include "propertiesmodel.h"

PropertiesModel::PropertiesModel(Entity **selected, QObject *parent)
    : QStandardItemModel{parent}
    , selected(selected)
{
    QObject::connect(this, &PropertiesModel::dataChanged,
                     this, &PropertiesModel::updateSelected);
}

void PropertiesModel::updateProperties()
{
    clear();

    if ((*selected) == nullptr || selected == nullptr) {
        QStandardItem* a = new QStandardItem;
        a->setText(QString::fromStdString("-"));
        a->setCheckable(false);
        a->setDragEnabled(false);
        a->setDropEnabled(false);
        a->setEditable(false);
        a->setSelectable(false);

        QStandardItem* b = new QStandardItem;
        b->setText(QString::fromStdString("-"));
        b->setCheckable(false);
        b->setDragEnabled(false);
        b->setDropEnabled(false);
        b->setEditable(false);
        b->setSelectable(false);

        appendRow(QList<QStandardItem*>({a, b}));
        return;
    }

    // Get entity json
    nlohmann::json json = (*selected)->getJsonRepresentation(true);

    // Append 2 columns
    insertColumn(0);
    insertColumn(1);
    // For each property, add a row and fill it
    for (auto property = json.begin(); property != json.end(); property++) {
        QStandardItem* key = new QStandardItem;
        key->setText(QString::fromStdString(property.key()));
        key->setCheckable(false);
        key->setDragEnabled(false);
        key->setDropEnabled(false);
        key->setEditable(false);

        QStandardItem* value = new QStandardItem;
        if (property.value().is_string())
            value->setText(QString::fromStdString(property.value()));
        else if (property.value().is_number())
            value->setText(QString::number(property.value().get<int>()));
        value->setCheckable(false);
        value->setDragEnabled(false);
        value->setDropEnabled(true);
        value->setEditable(true);

        appendRow(QList<QStandardItem*>({key, value}));
    }
}

QVariant PropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("Property");
            case 1:
                return QString("Value");
            }
        }
    return QVariant();
}

void PropertiesModel::updateSelected(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int>&)
{
    if (topLeft != bottomRight)
        return;

    // Get the values
    QStandardItem* item = itemFromIndex(topLeft);
    std::string key = this->item(topLeft.row(), 0)->text().toStdString();
    std::string value = item->text().toStdString();

    emit updateProperty(key, value);
}
