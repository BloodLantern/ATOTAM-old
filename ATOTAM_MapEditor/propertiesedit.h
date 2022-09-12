#ifndef PROPERTIESEDIT_H
#define PROPERTIESEDIT_H

#include "multitypeedit.h"

class PropertiesEdit : public MultiTypeEdit
{
public:
    PropertiesEdit(Map* map, Entity* entity, nlohmann::json propertyParam);
    PropertiesEdit(Map* map, int roomId, nlohmann::json propertyParam);
    virtual ~PropertiesEdit();

    // Edit interface
    void unmake() override;
    void make() override;

private:
    nlohmann::json property;
};

#endif // PROPERTIESEDIT_H
