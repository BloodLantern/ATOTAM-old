#ifndef AREA_H
#define AREA_H

#include "entity.h"

class Area : public Entity
{
public:
    Area(double x, double y, CollisionBox* box, QImage* texture, std::string name);
    Area(double x, double y, std::string name);
    Area(const Area&);
    ~Area();

    virtual nlohmann::json getJsonRepresentation(bool defaultValues);
    virtual void setJsonValues(nlohmann::json);

    const std::string &getAreaType() const;
    void setAreaType(const std::string &newAreaType);

private:
    std::string areaType = "";
};

#endif // AREA_H
