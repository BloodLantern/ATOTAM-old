#include "addedit.h"
#include "removeedit.h"

AddEdit::AddEdit(Map *map, Entity *entity, std::vector<Entity *>* entityList, Entity **selectedEntity)
    : MultiTypeEdit(map, entity)
    , entityList(entityList)
    , selectedEntity(selectedEntity)
{
    *selectedEntity = entity;
}

AddEdit::AddEdit(Map *map, Entity *entity, std::vector<Entity *> *entityList)
    : MultiTypeEdit(map, entity)
    , entityList(entityList)
{

}

AddEdit::AddEdit(Map *map, int roomId)
    : MultiTypeEdit(map, roomId)
{

}

AddEdit::~AddEdit()
{
    if (!made && entity && !roomEdit) {
        delete entity;
        entity = nullptr;
        if (selectedEntity != nullptr)
            *selectedEntity = nullptr;
    }
}

void AddEdit::unmake()
{
    if (roomEdit) {
    } else {
        RemoveEdit edit(map, entity, entityList, selectedEntity);
        edit.make();
        edit.setMade(false);
    }
    made = false;
}

void AddEdit::make()
{
    if (roomEdit) {
    } else {
        // Get map Json pointer
        nlohmann::json* mapJson = map->getJson();
        // json_pointer to the entity list
        nlohmann::json::json_pointer ptr("/rooms/" + std::to_string(entity->getRoomId()) + "/content/" + entity->getEntType());
        // json_pointer to the room position
        nlohmann::json::json_pointer roomPtr("/rooms/" + std::to_string(entity->getRoomId()) + "/position");
        nlohmann::json entJson;
        entJson = entity->getJsonRepresentation();
        entJson["x"] = entJson["x"].get<double>() - mapJson->at(roomPtr)[0].get<int>();
        entJson["y"] = entJson["y"].get<double>() - mapJson->at(roomPtr)[1].get<int>();
        // Create the entity type category if null
        if (mapJson->at(nlohmann::json::json_pointer("/rooms/" + std::to_string(entity->getRoomId()) + "/content"))[entity->getEntType()].is_null())
            mapJson->at(ptr) = "{}"_json;
        ptr = nlohmann::json::json_pointer("/rooms/" + std::to_string(entity->getRoomId()) + "/content/" + entity->getEntType() + "/" + entity->getFullName());
        // Create the entity name array if null
        if (mapJson->at(nlohmann::json::json_pointer("/rooms/" + std::to_string(entity->getRoomId()) + "/content"))[entity->getEntType()][entity->getFullName()].is_null())
            mapJson->at(ptr) = "[]"_json;
        // Add it
        mapJson->at(ptr).push_back(entJson);
        entityList->push_back(entity);
        if (selectedEntity)
            *selectedEntity = entity;
    }
    made = true;
}
