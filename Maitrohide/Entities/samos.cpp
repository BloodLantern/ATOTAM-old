#include "samos.h"

Samos::Samos(double x, double y, QImage* texture, int maxHealth)
    : Living(x, y, new CollisionBox(0, 0, 100, 200), texture, Entity::EntityType::Samos, maxHealth, maxHealth, true, Entity::Direction::Right),
      isInAltForm(false)
{

}

Samos::~Samos()
{

}

bool Samos::getIsInAltForm() const
{
    return isInAltForm;
}

void Samos::setIsInAltForm(bool newIsInAltForm)
{
    isInAltForm = newIsInAltForm;
}
