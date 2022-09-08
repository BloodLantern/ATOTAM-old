#ifndef ADDEDIT_H
#define ADDEDIT_H

#include "multitypeedit.h"

class AddEdit : public MultiTypeEdit
{
public:
    AddEdit(Map*, Entity*, std::vector<Entity*>* entityList, Entity** selectedEntity);
    AddEdit(Map*, Entity*, std::vector<Entity*>* entityList);
    AddEdit(Map*, int roomId);
    ~AddEdit();

    // Edit interface
    void unmake() override;
    void make() override;

private:
    std::vector<Entity*>* entityList;
    Entity** selectedEntity = nullptr;
};

#endif // ADDEDIT_H
