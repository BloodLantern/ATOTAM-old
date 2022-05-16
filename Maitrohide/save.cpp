#include "save.h"

Save Save::load(std::ifstream file)
{
    nlohmann::json json;
    file >> json;
    return Save(json["samos"]["health"], json["samos"]["maxHealth"], json["samos"]["missiles"], json["samos"]["maxMissiles"], json["samos"]["grenades"], json["samos"]["maxGrenade"],
            json["playTime"], json["deaths"], json["damageDone"], json["damageReceived"]);
}

Save::Save(int samosHealth, int samosMaxHealth, int samosMissiles, int samosMaxMissiles, int samosGrenades, int samosMaxGrenades,
           unsigned long playTime, int deaths, int damageDone, int damageReceived)
    : samosHealth(samosHealth), samosMaxHealth(samosMaxHealth), samosMissiles(samosMissiles), samosMaxMissiles(samosMaxMissiles), samosGrenades(samosGrenades), samosMaxGrenades(samosMaxGrenades),
      playTime(playTime), deaths(deaths), damageDone(damageDone), damageReceived(damageReceived)
{

}

void Save::save()
{
    nlohmann::json json;
    json["samos"]["health"] = samosHealth;
    json["samos"]["maxHealth"] = samosMaxHealth;
    json["samos"]["missiles"] = samosMissiles;
    json["samos"]["maxMissiles"] = samosMaxMissiles;
    json["samos"]["grenades"] = samosGrenades;
    json["samos"]["maxGrenades"] = samosMaxGrenades;
    json["playTime"] = playTime;
    json["deaths"] = deaths;
    json["damageDone"] = damageDone;
    json["damageReceived"] = damageReceived;
}

int Save::getSamosHealth() const
{
    return samosHealth;
}

void Save::setSamosHealth(int newSamosHealth)
{
    samosHealth = newSamosHealth;
}

int Save::getSamosMaxHealth() const
{
    return samosMaxHealth;
}

void Save::setSamosMaxHealth(int newSamosMaxHealth)
{
    samosMaxHealth = newSamosMaxHealth;
}

int Save::getSamosMissiles() const
{
    return samosMissiles;
}

void Save::setSamosMissiles(int newSamosMissiles)
{
    samosMissiles = newSamosMissiles;
}

int Save::getSamosMaxMissiles() const
{
    return samosMaxMissiles;
}

void Save::setSamosMaxMissiles(int newSamosMaxMissiles)
{
    samosMaxMissiles = newSamosMaxMissiles;
}

int Save::getSamosGrenades() const
{
    return samosGrenades;
}

void Save::setSamosGrenades(int newSamosGrenades)
{
    samosGrenades = newSamosGrenades;
}

int Save::getSamosMaxGrenades() const
{
    return samosMaxGrenades;
}

void Save::setSamosMaxGrenades(int newSamosMaxGrenades)
{
    samosMaxGrenades = newSamosMaxGrenades;
}

unsigned long Save::getPlayTime() const
{
    return playTime;
}

void Save::setPlayTime(unsigned long newPlayTime)
{
    playTime = newPlayTime;
}

int Save::getDeaths() const
{
    return deaths;
}

void Save::setDeaths(int newDeaths)
{
    deaths = newDeaths;
}

int Save::getDamageDone() const
{
    return damageDone;
}

void Save::setDamageDone(int newDamageDone)
{
    damageDone = newDamageDone;
}

int Save::getDamageReceived() const
{
    return damageReceived;
}

void Save::setDamageReceived(int newDamageReceived)
{
    damageReceived = newDamageReceived;
}
