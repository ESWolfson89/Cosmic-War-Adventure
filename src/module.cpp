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
    os.write((const char *)&mt,sizeof(module_type));
    os.write((const char *)&fill_quantity,sizeof(int));
    os.write((const char *)&max_fill_quantity,sizeof(int));
    os.write((const char *)&is_item_container,sizeof(bool));
}

void module::load(std::ifstream& is)
{
    is.read((char *)&mt,sizeof(module_type));
    is.read((char *)&fill_quantity,sizeof(int));
    is.read((char *)&max_fill_quantity,sizeof(int));
    is.read((char *)&is_item_container,sizeof(bool));
}

int getWeaponModuleConsumptionPerTurn(module *m)
{
    return m->getWeaponStruct().consumption_rate * m->getWeaponStruct().num_shots;
}
