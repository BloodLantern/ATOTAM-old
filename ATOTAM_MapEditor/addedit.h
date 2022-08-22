#ifndef ADDEDIT_H
#define ADDEDIT_H

#include "multitypeedit.h"

class AddEdit : public MultiTypeEdit
{
public:
    AddEdit(Map*, Entity*);
    AddEdit(Map*, int roomId);

    // Edit interface
    void unmake() override;
    void make() override;
};

#endif // ADDEDIT_H
