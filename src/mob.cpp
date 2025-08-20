#include "mob.h"

MobShip player_ship;

MobShip::MobShip()
{
    loc = point(1,1);
    player_controlled = false;
}

MobShip::MobShip(bool pc, shipmobstat_struct sms_param, npc_ship_type nst, int hs, point p)
{
    mob_subarea_id = -1;
    setShipMob(pc,sms_param,nst,hs,p);
}

void MobShip::setShipMob(bool pc, shipmobstat_struct sms_param, npc_ship_type nst, int hs, point p)
{
    init_loc = loc = p;
    sms_data = sms_param;
    player_controlled = pc;
    hull_status = hs;
    is_activated = true;
    smgs = GOALSTATUS_COMPLETE;
    smAIp = AIPATTERN_NEUTRAL;
    destination = loc;
    NPC_turn_timer = 0.0;
    module_selection_index = 0;
    danger_level = sms_param.danger_level;
    can_move = true;
    num_credits = 0ULL;
    npc_ship_type_obj = nst;
    IDLastAttackedBy = -1;
    initShipDesign();
}

void MobShip::setMobIDLastAttackedBy(int id)
{
    IDLastAttackedBy = id;
}

int MobShip::getMobIDLastAttackedBy()
{
    return IDLastAttackedBy;
}

void MobShip::initShipDesign()
{
    int val = randInt(5, 402); // Reduce upper bound to reserve space for 2 skips
    if (val >= 331) {
        val += 2; // Skip both 268 and 331
    }
    else if (val >= 268) {
        val += 1; // Skip just 268
    }
    design_obj.CAShipGenerations = val;
    design_obj.CASecondValue = ship_design_pattern_ch[randInt(0,NUM_POSSIBLE_SHIPDESIGN_SYMBOLS-1)];
    design_obj.CAThirdValue = ship_design_pattern_ch[randInt(0, NUM_POSSIBLE_SHIPDESIGN_SYMBOLS-1)];
    design_obj.CAFourthValue = ship_design_pattern_ch[randInt(0, NUM_POSSIBLE_SHIPDESIGN_SYMBOLS-1)];
    design_obj.ctSecondColor = procgen_ship_colors[randInt(0,NUM_POSSIBLE_SHIP_COLORS-1)].fg;
    design_obj.ctThirdColor = procgen_ship_colors[randInt(0, NUM_POSSIBLE_SHIP_COLORS-1)].fg;
    design_obj.ctFourthColor = procgen_ship_colors[randInt(0, NUM_POSSIBLE_SHIP_COLORS-1)].fg;
    design_obj.shipFlameCh = shipFlames[randInt(0, static_cast<int>(shipFlames.size()) - 1)];
    design_obj.weaponFrontOffset = randInt(0, 4);
    switch(sms_data.sctype)
    {
        case(CLASSTYPE_FIGHTER):
            if (danger_level <= 13)
                design_obj.xExtension = randInt(16,18);
            else
                design_obj.xExtension = randInt(19,21);
            break;
        case(CLASSTYPE_DREADNOUGHT):
            design_obj.xExtension = randInt(24,26);
            break;
        case(CLASSTYPE_DESTROYER):
            design_obj.xExtension = randInt(30,32);
            break;
        case(CLASSTYPE_JUGGERNAUT):
            design_obj.xExtension = 40;
            break;
        default:
            design_obj.xExtension = 15;
            break;
    }
}

void MobShip::setInitLoc(point p)
{
    init_loc = p;
}

shipmobstat_struct MobShip::getStatStruct()
{
    return sms_data;
}

shipmob_goalStatus MobShip::getGoalStatus()
{
    return smgs;
}

shipmob_AIpattern MobShip::getAIPattern()
{
    return smAIp;
}

ship_design_struct MobShip::getDesignStruct()
{
    return design_obj;
}

npc_ship_type MobShip::getNPCShipType()
{
    return npc_ship_type_obj;
}

void MobShip::setGoalStatus(shipmob_goalStatus new_gs)
{
    smgs = new_gs;
}

void MobShip::setRoveChance(int rc)
{
    sms_data.rove_chance = rc;
}

void MobShip::setAIPattern(shipmob_AIpattern new_p)
{
    smAIp = new_p;
}

void MobShip::setTurnTimer(double d)
{
	NPC_turn_timer = d;
}

void MobShip::decrementTurnTimer()
{
	NPC_turn_timer--;
}

void MobShip::setMoveState(bool b)
{
    can_move = b;
}

void MobShip::setMaxHull(int mh)
{
    sms_data.max_hull = mh;
}

double MobShip::getTurnTimer()
{
	return NPC_turn_timer;
}

bool MobShip::inTimerRange()
{
	// area of precision (assuming you dont have speeds of 1001, etc...
	return NPC_turn_timer < 0.0001 && NPC_turn_timer > -0.0001;
}

point MobShip::at()
{
    return loc;
}

double MobShip::getSpeed()
{
    return sms_data.base_speed + getBonusSpeed();
}

double MobShip::getEvasion()
{
    return sms_data.evasion + getModifiedEvasion();
}

double MobShip::getAccuracy()
{
    return sms_data.accuracy + getModule(getModuleSelectionIndex())->getWeaponStruct().to_hit;
}

double MobShip::getBonusSpeed()
{
    double bonus_speed = 0.0;
    int eng_iter = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == MODULE_ENGINE)
        {
            bonus_speed += getModule(i)->getEngineStruct().bonus_speed;
            eng_iter++;
        }
    }
    if (eng_iter > 1)
    {
        bonus_speed += 10.0*((double)(eng_iter-1));
    }
    return bonus_speed;
}

double MobShip::getModifiedEvasion()
{
    double mod_evasion = 0.0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == MODULE_ENGINE)
        {
            mod_evasion += getModule(i)->getEngineStruct().bonus_evasion;
        }
    }
    return mod_evasion;
}

int MobShip::getFuelConsumptionRate()
{
    int rate = 1;
    int eng_iter = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == MODULE_ENGINE)
        {
            rate += getModule(i)->getEngineStruct().fuel_penalty;
            eng_iter++;
        }
    }
    if (eng_iter > 1)
    {
        rate += (eng_iter - 1);
    }
    return rate;
}

void MobShip::setLoc(point p)
{
    loc = p;
}

void MobShip::offLoc(point p)
{
    loc = addPoints(loc,p);
}

int MobShip::getDetectRadius()
{
    return sms_data.detect_radius;
}

void MobShip::addModule(Module m)
{
    if ((int)module_vec.size() < sms_data.num_max_modules)
        module_vec.push_back(m);
}

void MobShip::removeModule(int index)
{
    module_vec.erase(module_vec.begin()+index);
}

void MobShip::setHullStatus(int h)
{
    hull_status = h;
    if (hull_status > getMaxHull())
        hull_status = getMaxHull();
}

void MobShip::setActivationStatus(bool b)
{
    is_activated = b;
}

Module *MobShip::getModule(int index)
{
    return &(module_vec[index]);
}

int MobShip::getMaxNumModules()
{
    return sms_data.num_max_modules;
}

int MobShip::getNumInstalledModules()
{
    return (int)module_vec.size();
}

int MobShip::getHullStatus()
{
    return hull_status;
}

int MobShip::getMaxHull()
{
    return sms_data.max_hull;
}

void MobShip::eraseModule(int i)
{
    module_vec.erase(module_vec.begin() + i);
}

bool MobShip::crewOperable()
{
    return sms_data.crew_operable;
}

bool MobShip::isActivated() const
{
    return is_activated;
}

bool MobShip::getMoveState()
{
    return can_move;
}

int MobShip::getDangerLevel()
{
    return danger_level;
}

void MobShip::incTotalFillAmount(int amount,module_type mt)
{
    int increment_remaining = amount;
    int increment_unit;
    int module_quant = 0;
    int module_cap = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == mt)
        {
            module_quant = getModule(i)->getFillQuantity();
            module_cap = getModule(i)->getMaxFillQuantity();
            increment_unit = (int)std::min(module_cap - module_quant,increment_remaining);
            getModule(i)->offFillQuantity(increment_unit);
            increment_remaining -= increment_unit;
        }
        if (increment_remaining <= 0)
        {
            return;
        }
    }
}

void MobShip::decTotalFillAmount(int amount,module_type mt)
{
    int decrement_remaining = amount;
    int decrement_unit;
    int module_quant = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == mt)
        {
            module_quant = getModule(i)->getFillQuantity();
            decrement_unit = (int)std::min(module_quant,decrement_remaining);
            getModule(i)->offFillQuantity(-1*decrement_unit);
            decrement_remaining -= decrement_unit;
        }
        if (decrement_remaining <= 0)
        {
            return;
        }
    }
}

int MobShip::getTotalMTFillRemaining(module_type mt)
{
    int amount_remaining = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == mt)
            amount_remaining += getModule(i)->getFillQuantity();
    }
    return amount_remaining;
}

int MobShip::getTotalMTFillCapacity(module_type mt)
{
    int amount_capacity = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() == mt)
            amount_capacity += getModule(i)->getMaxFillQuantity();
    }
    return amount_capacity;
}

int MobShip::getTotalFillPercentageOfType(module_type mt)
{
    return (int)(((double)getTotalMTFillRemaining(MODULE_SHIELD)/(double)getTotalMTFillCapacity(MODULE_SHIELD))*100.0);
}

int MobShip::getNumInstalledModulesOfType(module_type mt)
{
    int num = 0;
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (mt == getModule(i)->getModuleType())
            num++;
    }
    return num;
}

int MobShip::getCrashFactor()
{
    return sms_data.crash_factor;
}

int MobShip::getModuleSelectionIndex()
{
    return module_selection_index;
}

void MobShip::setModuleSelectionIndex(int msi)
{
    module_selection_index = msi;
    checkSelectedModuleBoundary();
}

void MobShip::offModuleSelectionIndex(int o)
{
    module_selection_index += o;
    checkSelectedModuleBoundary();
}

void MobShip::checkSelectedModuleBoundary()
{
    if (module_selection_index >= getNumInstalledModules())
        module_selection_index = 0;
    if (module_selection_index < 0)
        module_selection_index = getNumInstalledModules() - 1;
}

bool MobShip::isCurrentPlayerShip()
{
    return player_controlled;
}

void MobShip::cleanupEverything()
{
    std::vector<Module>().swap(module_vec);
}

void MobShip::setDestination(point p)
{
    destination = p;
}

point MobShip::getInitLoc()
{
    return init_loc;
}

point MobShip::getDestination()
{
    return destination;
}

std::string MobShip::getShipName()
{
    return sms_data.ship_name;
}

mob_t MobShip::getMobType()
{
    return sms_data.mtype;
}

chtype MobShip::getShipSymbol()
{
    return sms_data.ship_symbol;
}

int MobShip::getMobSubAreaID()
{
    return mob_subarea_id;
}

int MobShip::getMobSubAreaGroupID() const
{
    return mob_subarea_group_id;
}

int MobShip::getMobSubAreaAttackID()
{
    return mob_subarea_attack_id;
}

void MobShip::setShipName(std::string sn)
{
    sms_data.ship_name = sn;
}

void MobShip::initMobSubAreaID(int id)
{
    mob_subarea_id = id;
}

void MobShip::setMobSubAreaAttackID(int aid)
{
    mob_subarea_attack_id = aid;
}

void MobShip::setMobSubAreaGroupID(int gid)
{
    mob_subarea_group_id = gid;
}

void MobShip::useFuelEvent(int cons_rate)
{
    for (int i = 0; i < getNumInstalledModules(); ++i)
    {
        if (getModule(i)->getModuleType() != MODULE_FUEL)
            continue;
        if (getModule(i)->getFillQuantity() <= 0)
            continue;
        getModule(i)->offFillQuantity(-1*cons_rate);
        return;
    }
}

bool MobShip::isLowOnFuel()
{
    if (hasNoFuel())
        return false;

    if (((double)getTotalMTFillRemaining(MODULE_FUEL)/(double)getTotalMTFillCapacity(MODULE_FUEL)) < 0.1)
        return true;

    return false;
}

bool MobShip::hasNoFuel()
{
    return (getTotalMTFillRemaining(MODULE_FUEL) == 0.0 || getTotalMTFillCapacity(MODULE_FUEL) == 0.0);
}

uint_64 MobShip::getNumCredits()
{
    return num_credits;
}

void MobShip::setNumCredits(uint_64 new_cred)
{
    num_credits = new_cred;
}

void MobShip::setNumMaxModules(int nmm)
{
    sms_data.num_max_modules = nmm;
}

void MobShip::save(std::ofstream& os) const
{
    os.write(reinterpret_cast<const char*>(&player_controlled), sizeof(bool));
    saveShipMobStatStruct(os, sms_data); // Manual serialization
    os.write(reinterpret_cast<const char*>(&npc_ship_type_obj), sizeof(npc_ship_type));
    os.write(reinterpret_cast<const char*>(&is_activated), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&can_move), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&smgs), sizeof(shipmob_goalStatus));
    os.write(reinterpret_cast<const char*>(&smAIp), sizeof(shipmob_AIpattern));
    os.write(reinterpret_cast<const char*>(&NPC_turn_timer), sizeof(double));
    os.write(reinterpret_cast<const char*>(&mob_subarea_id), sizeof(int));
    os.write(reinterpret_cast<const char*>(&hull_status), sizeof(int));
    os.write(reinterpret_cast<const char*>(&danger_level), sizeof(int));
    os.write(reinterpret_cast<const char*>(&module_selection_index), sizeof(int));
    os.write(reinterpret_cast<const char*>(&mob_subarea_group_id), sizeof(int));
    os.write(reinterpret_cast<const char*>(&mob_subarea_attack_id), sizeof(int));
    os.write(reinterpret_cast<const char*>(&IDLastAttackedBy), sizeof(int));
    os.write(reinterpret_cast<const char*>(&design_obj), sizeof(ship_design_struct));

    destination.save(os);
    loc.save(os);
    init_loc.save(os);

    os.write(reinterpret_cast<const char*>(&num_credits), sizeof(uint_64));

    int module_count = static_cast<int>(module_vec.size());
    os.write(reinterpret_cast<const char*>(&module_count), sizeof(int));
    for (const Module& m : module_vec)
        m.save(os);
}

void MobShip::load(std::ifstream& is)
{
    is.read(reinterpret_cast<char*>(&player_controlled), sizeof(bool));
    loadShipMobStatStruct(is, sms_data);
    is.read(reinterpret_cast<char*>(&npc_ship_type_obj), sizeof(npc_ship_type));
    is.read(reinterpret_cast<char*>(&is_activated), sizeof(bool));
    is.read(reinterpret_cast<char*>(&can_move), sizeof(bool));
    is.read(reinterpret_cast<char*>(&smgs), sizeof(shipmob_goalStatus));
    is.read(reinterpret_cast<char*>(&smAIp), sizeof(shipmob_AIpattern));
    is.read(reinterpret_cast<char*>(&NPC_turn_timer), sizeof(double));
    is.read(reinterpret_cast<char*>(&mob_subarea_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&hull_status), sizeof(int));
    is.read(reinterpret_cast<char*>(&danger_level), sizeof(int));
    is.read(reinterpret_cast<char*>(&module_selection_index), sizeof(int));
    is.read(reinterpret_cast<char*>(&mob_subarea_group_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&mob_subarea_attack_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&IDLastAttackedBy), sizeof(int));
    is.read(reinterpret_cast<char*>(&design_obj), sizeof(ship_design_struct));

    destination.load(is);
    loc.load(is);
    init_loc.load(is);

    is.read(reinterpret_cast<char*>(&num_credits), sizeof(uint_64));

    int module_count = 0;
    is.read(reinterpret_cast<char*>(&module_count), sizeof(int));
    module_vec.resize(module_count);
    for (int i = 0; i < module_count; ++i)
        module_vec[i].load(is);
}

void saveShipMobStatStruct(std::ofstream& os, const shipmobstat_struct& s)
{
    os.write(reinterpret_cast<const char*>(&s.mtype), sizeof(mob_t));
    os.write(reinterpret_cast<const char*>(&s.danger_level), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.accuracy), sizeof(double));
    os.write(reinterpret_cast<const char*>(&s.evasion), sizeof(double));
    os.write(reinterpret_cast<const char*>(&s.base_speed), sizeof(double));
    os.write(reinterpret_cast<const char*>(&s.max_hull), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.detect_radius), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.num_max_modules), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.crash_factor), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.num_crew), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.num_fuel), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.frequency), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.shoot_frequency), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.destruction_radius), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.weapon_change_chance), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.rove_chance), sizeof(int));
    os.write(reinterpret_cast<const char*>(&s.min_attack_pfind_distance), sizeof(int));

    os.write(reinterpret_cast<const char*>(s.num_weapons_of_type), sizeof(s.num_weapons_of_type));
    os.write(reinterpret_cast<const char*>(s.num_engines_of_type), sizeof(s.num_engines_of_type));
    os.write(reinterpret_cast<const char*>(s.num_shields_of_type), sizeof(s.num_shields_of_type));

    os.write(reinterpret_cast<const char*>(&s.crew_operable), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&s.sctype), sizeof(shipmob_classtype));
    os.write(reinterpret_cast<const char*>(&s.rtype), sizeof(race_type));

    chtypeSave(os, s.ship_symbol);
    stringSave(os, s.ship_name);
}

void loadShipMobStatStruct(std::ifstream& is, shipmobstat_struct& s)
{
    is.read(reinterpret_cast<char*>(&s.mtype), sizeof(mob_t));
    is.read(reinterpret_cast<char*>(&s.danger_level), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.accuracy), sizeof(double));
    is.read(reinterpret_cast<char*>(&s.evasion), sizeof(double));
    is.read(reinterpret_cast<char*>(&s.base_speed), sizeof(double));
    is.read(reinterpret_cast<char*>(&s.max_hull), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.detect_radius), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.num_max_modules), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.crash_factor), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.num_crew), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.num_fuel), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.frequency), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.shoot_frequency), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.destruction_radius), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.weapon_change_chance), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.rove_chance), sizeof(int));
    is.read(reinterpret_cast<char*>(&s.min_attack_pfind_distance), sizeof(int));

    is.read(reinterpret_cast<char*>(s.num_weapons_of_type), sizeof(s.num_weapons_of_type));
    is.read(reinterpret_cast<char*>(s.num_engines_of_type), sizeof(s.num_engines_of_type));
    is.read(reinterpret_cast<char*>(s.num_shields_of_type), sizeof(s.num_shields_of_type));

    is.read(reinterpret_cast<char*>(&s.crew_operable), sizeof(bool));
    is.read(reinterpret_cast<char*>(&s.sctype), sizeof(shipmob_classtype));
    is.read(reinterpret_cast<char*>(&s.rtype), sizeof(race_type));

    chtypeLoad(is, s.ship_symbol);
    stringLoad(is, s.ship_name);
}

MobShip* getPlayerShip()
{
    return &player_ship;
}

bool hasWeaponOfType(MobShip* ship, weapon_t t)
{
    const int numModules = ship->getNumInstalledModules();
    for (int i = 0; i < numModules; ++i)
    {
        Module* mod = ship->getModule(i);
        if (mod->getModuleType() == MODULE_WEAPON &&
            mod->getWeaponStruct().wt == t)
        {
            return true;
        }
    }
    return false;
}

Module* getCurrentMobSelectedModule(MobShip* mb)
{
    return mb->getModule(mb->getModuleSelectionIndex());
}