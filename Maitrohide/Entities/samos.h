#ifndef SAMOS_H
#define SAMOS_H

#include "living.h"

class Samos : public Living
{
public:
    Samos(double x, double y, QImage* texture, int maxHealth);
    ~Samos();
    bool getIsInAltForm() const;
    void setIsInAltForm(bool newIsInAltForm);

private:
    bool isInAltForm;

};

#endif // SAMOS_H
