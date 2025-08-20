#include "homeworld.h"

HomeWorld::HomeWorld()
{

}

HomeWorld::HomeWorld(point p, int oID, int cID, int dl)
{
	initHomeWorld(p, oID, cID, dl);
}

void HomeWorld::initHomeWorld(point p, int oID, int cID, int dl)
{
	setLoc(p);
	setOriginRace(oID, dl);
	setControlRace(cID, dl);
	setDangerLevel(dl);
	raceMajorStatus = RMS_FREE;
}

void HomeWorld::setDangerLevel(int dl)
{
	dangerLevel = dl;
}

void HomeWorld::setOriginRace(int oID, int dl)
{
	originRaceID = oID;
	setDangerLevel(dl);
}

void HomeWorld::setControlRace(int cID, int dl)
{
	controlRaceID = cID;
	setDangerLevel(dl);
}

void HomeWorld::setLoc(point p)
{
	loc = p;
}

void HomeWorld::setRaceMajorStatus(RaceMajorStatus rms)
{
	raceMajorStatus = rms;
}

int HomeWorld::getOriginRaceID()
{
	return originRaceID;
}

int HomeWorld::getControlRaceID()
{
	return controlRaceID;
}

int HomeWorld::getDangerLevel()
{
	return dangerLevel;
}

point HomeWorld::getLoc()
{
	return loc;
}

RaceMajorStatus HomeWorld::getRaceMajorStatus()
{
	return raceMajorStatus;
}

void HomeWorld::save(std::ofstream& os) const
{
	loc.save(os);
	os.write(reinterpret_cast<const char*>(&originRaceID), sizeof(int));
	os.write(reinterpret_cast<const char*>(&controlRaceID), sizeof(int));
	os.write(reinterpret_cast<const char*>(&raceMajorStatus), sizeof(RaceMajorStatus));
	os.write(reinterpret_cast<const char*>(&dangerLevel), sizeof(int));
}

void HomeWorld::load(std::ifstream& is)
{
	loc.load(is);
	is.read(reinterpret_cast<char*>(&originRaceID), sizeof(int));
	is.read(reinterpret_cast<char*>(&controlRaceID), sizeof(int));
	is.read(reinterpret_cast<char*>(&raceMajorStatus), sizeof(RaceMajorStatus));
	is.read(reinterpret_cast<char*>(&dangerLevel), sizeof(int));
}