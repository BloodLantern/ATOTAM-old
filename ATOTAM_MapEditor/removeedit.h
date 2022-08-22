#ifndef REMOVEEDIT_H
#define REMOVEEDIT_H

#include "multitypeedit.h"

class RemoveEdit : public MultiTypeEdit
{
public:
    RemoveEdit(Map*, Entity*);
    RemoveEdit(Map*, int roomId);
    ~RemoveEdit();

    // Edit interface
    void unmake() override;
    void make() override;
};

#endif // REMOVEEDIT_H
