#ifndef SAVE_H
#define SAVE_H

#include <fstream>
#include "nlohmann/json.hpp"

class Save
{
public:
    static Save load(std::ifstream file); // Creates and returns a Save object corresponding to the Json file represented by the stream
    Save(int samosHealth, int samosMaxHealth, int samosMissiles, int samosMaxMissiles, int samosGrenades, int samosMaxGrenades,
         unsigned long playTime, int deaths, int damageDone, int damageReceived); // Create a new Save object with these parameters
    void save(std::ofstream file); // Converts this Save object in Json and then serialize it
    int getSamosHealth() const;
    void setSamosHealth(int newSamosHealth);

    int getSamosMaxHealth() const;
    void setSamosMaxHealth(int newSamosMaxHealth);

    int getSamosMissiles() const;
    void setSamosMissiles(int newSamosMissiles);

    int getSamosMaxMissiles() const;
    void setSamosMaxMissiles(int newSamosMaxMissiles);

    int getSamosGrenades() const;
    void setSamosGrenades(int newSamosGrenades);

    int getSamosMaxGrenades() const;
    void setSamosMaxGrenades(int newSamosMaxGrenades);

    unsigned long getPlayTime() const;
    void setPlayTime(unsigned long newPlayTime);

    int getDeaths() const;
    void setDeaths(int newDeaths);

    int getDamageDone() const;
    void setDamageDone(int newDamageDone);

    int getDamageReceived() const;
    void setDamageReceived(int newDamageReceived);

private:
    int samosHealth;
    int samosMaxHealth;
    int samosMissiles;
    int samosMaxMissiles;
    int samosGrenades;
    int samosMaxGrenades;
    // Save point
    // Items
    // Rooms discovered (minimap) for every map
    unsigned long playTime; // In seconds
    int deaths;
    int damageDone;
    int damageReceived;
    // And more ?
};

#endif // SAVE_H
