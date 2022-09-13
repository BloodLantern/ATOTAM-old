#ifndef PROPERTIESMODEL_H
#define PROPERTIESMODEL_H

#include "../ATOTAM/Entities/entity.h"
#include "../ATOTAM/map.h"

class PropertiesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PropertiesModel(Entity** selected, Map* currentMap, nlohmann::json editorJson, QObject *parent = nullptr);

    void updateProperties();

    // QAbstractItemModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void updateSelected(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles = QList<int>());

signals:
    void updateProperty(std::string key, std::string value);

private:
    Entity** selected = nullptr;
    nlohmann::json editorJson = nlohmann::json();
    Map* currentMap = nullptr;
};

#endif // PROPERTIESMODEL_H
