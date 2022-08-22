#ifndef REMOVEEDIT_H
#define REMOVEEDIT_H

#include "multitypeedit.h"

class RemoveEdit : public MultiTypeEdit
{
public:
    RemoveEdit(Map*, Entity*, std::vector<Entity*>* entityList, Entity** selectedEntity);
    RemoveEdit(Map*, int roomId);
    ~RemoveEdit();

    // Edit interface
    void unmake() override;
    void make() override;

private:
    std::vector<Entity*>* entityList;
    Entity** selectedEntity;
};

#endif // REMOVEEDIT_H
