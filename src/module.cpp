#include "module.h"

module::module()
{

}

module::module(module_type m, int q, int mq)
{
    setModule(m,q,mq);
}

void module::setModule(module_type m, int q, int mq)
{
    mt = m;
    fill_quantity = q;
    max_fill_quantity = mq;
    is_item_container = false;
    weapon_stats = allbasicweapon_stats[0];
    shield_stats = allbasicshield_stats[0];
    engine_stats = allbasicengine_stats[0];
}

void module::setWeaponStruct(weapon_struct w)
{
    weapon_stats = w;
}

void module::setShieldStruct(shield_struct s)
{
    shield_stats = s;
}

void module::setEngineStruct(shipengine_struct e)
{
    engine_stats = e;
}

weapon_struct module::getWeaponStruct()
{
    return weapon_stats;
}

shield_struct module::getShieldStruct()
{
    return shield_stats;
}

shipengine_struct module::getEngineStruct()
{
    return engine_stats;
}

module_type module::getModuleType()
{
    return mt;
}

void module::offFillQuantity(int offset)
{
    fill_quantity += offset;
    checkFillQuantity();
}

void module::setFillQuantity(int amount)
{
    fill_quantity = amount;
    checkFillQuantity();
}

void module::checkFillQuantity()
{
    if (fill_quantity < 0)
        fill_quantity = 0;
    if (fill_quantity > max_fill_quantity)
        fill_quantity = max_fill_quantity;
}

int module::getFillQuantity()
{
    return fill_quantity;
}

int module::getMaxFillQuantity()
{
    return max_fill_quantity;
}

int module::getBaseCost()
{
    int base_cost = 1ULL;

    switch(mt)
    {
        case(MODULE_WEAPON):
        {
            base_cost = weapon_stats.base_cost;
            break;
        }
        case(MODULE_SHIELD):
        {
            base_cost = shield_stats.base_cost;
            break;
        }
        case(MODULE_ENGINE):
        {
            base_cost = engine_stats.base_cost;
            break;
        }
        default:
            break;
    }

    return base_cost;
}

void module::save(std::ofstream& os) const
{
    os.write(reinterpret_cast<const char*>(&mt), sizeof(module_type));
    os.write(reinterpret_cast<const char*>(&fill_quantity), sizeof(int));
    os.write(reinterpret_cast<const char*>(&max_fill_quantity), sizeof(int));
    os.write(reinterpret_cast<const char*>(&is_item_container), sizeof(bool));

    saveWeaponStruct(os, weapon_stats);
    saveShieldStruct(os, shield_stats);
    saveEngineStruct(os, engine_stats);

    stringSave(os, module_name);
    stringSave(os, name_modifier);
}

void module::load(std::ifstream& is)
{
    is.read(reinterpret_cast<char*>(&mt), sizeof(module_type));
    is.read(reinterpret_cast<char*>(&fill_quantity), sizeof(int));
    is.read(reinterpret_cast<char*>(&max_fill_quantity), sizeof(int));
    is.read(reinterpret_cast<char*>(&is_item_container), sizeof(bool));

    loadWeaponStruct(is, weapon_stats);
    loadShieldStruct(is, shield_stats);
    loadEngineStruct(is, engine_stats);

    stringLoad(is, module_name);
    stringLoad(is, name_modifier);
}

void saveWeaponStruct(std::ofstream& os, const weapon_struct& w)
{
    os.write(reinterpret_cast<const char*>(&w.wt), sizeof(weapon_t));
    saveDiceRoll(os, w.hull_damage);
    saveDiceRoll(os, w.crew_damage);
    os.write(reinterpret_cast<const char*>(&w.to_hit), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.shield_damage_count), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.stealth_chance), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.travel_through_chance), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.anti_personnel_crit_perc), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.anti_personnel_crit_multiplier), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.anti_hull_crit_perc), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.anti_hull_crit_multiplier), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.travel_range), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.blast_radius), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.num_shots), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.consumption_rate), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.regen_rate), sizeof(int));
    os.write(reinterpret_cast<const char*>(&w.is_firet_line), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&w.ignores_shields), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&w.eightDirectionRestricted), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&w.ftile), sizeof(fire_t));
    chtypeSave(os, w.disp_chtype);
    stringSave(os, w.name_modifier);
    os.write(reinterpret_cast<const char*>(&w.base_cost), sizeof(int));
}

void loadWeaponStruct(std::ifstream& is, weapon_struct& w)
{
    is.read(reinterpret_cast<char*>(&w.wt), sizeof(weapon_t));
    loadDiceRoll(is, w.hull_damage);
    loadDiceRoll(is, w.crew_damage);
    is.read(reinterpret_cast<char*>(&w.to_hit), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.shield_damage_count), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.stealth_chance), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.travel_through_chance), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.anti_personnel_crit_perc), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.anti_personnel_crit_multiplier), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.anti_hull_crit_perc), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.anti_hull_crit_multiplier), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.travel_range), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.blast_radius), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.num_shots), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.consumption_rate), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.regen_rate), sizeof(int));
    is.read(reinterpret_cast<char*>(&w.is_firet_line), sizeof(bool));
    is.read(reinterpret_cast<char*>(&w.ignores_shields), sizeof(bool));
    is.read(reinterpret_cast<char*>(&w.eightDirectionRestricted), sizeof(bool));
    is.read(reinterpret_cast<char*>(&w.ftile), sizeof(fire_t));
    chtypeLoad(is, w.disp_chtype);
    stringLoad(is, w.name_modifier);
    is.read(reinterpret_cast<char*>(&w.base_cost), sizeof(int));
}

void saveShieldStruct(std::ofstream& os, const shield_struct& s)
{
    os.write(reinterpret_cast<const char*>(&s.base_num_layers), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.regen_rate), sizeof(int));
    chtypeSave(os, s.disp_chtype);
    stringSave(os, s.name_modifier);
    os.write(reinterpret_cast<const char*>(&s.base_cost), sizeof(int));
}

void loadShieldStruct(std::ifstream& is, shield_struct& s)
{
    is.read(reinterpret_cast<char*>(&s.base_num_layers), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.regen_rate), sizeof(int));
    chtypeLoad(is, s.disp_chtype);
    stringLoad(is, s.name_modifier);
    is.read(reinterpret_cast<char*>(&s.base_cost), sizeof(int));
}

void saveEngineStruct(std::ofstream& os, const shipengine_struct& e)
{
    os.write(reinterpret_cast<const char*>(&e.bonus_speed), sizeof(int));
    os.write(reinterpret_cast<const char*>(&e.bonus_evasion), sizeof(int));
    os.write(reinterpret_cast<const char*>(&e.fuel_penalty), sizeof(int));
    chtypeSave(os, e.disp_chtype);
    stringSave(os, e.name_modifier);
    os.write(reinterpret_cast<const char*>(&e.base_cost), sizeof(int));
}

void loadEngineStruct(std::ifstream& is, shipengine_struct& e)
{
    is.read(reinterpret_cast<char*>(&e.bonus_speed), sizeof(int));
    is.read(reinterpret_cast<char*>(&e.bonus_evasion), sizeof(int));
    is.read(reinterpret_cast<char*>(&e.fuel_penalty), sizeof(int));
    chtypeLoad(is, e.disp_chtype);
    stringLoad(is, e.name_modifier);
    is.read(reinterpret_cast<char*>(&e.base_cost), sizeof(int));
}

int getWeaponModuleConsumptionPerTurn(module *m)
{
    return m->getWeaponStruct().consumption_rate * m->getWeaponStruct().num_shots;
}

