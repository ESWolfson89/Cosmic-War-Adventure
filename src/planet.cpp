#include "planet.h"

Planet::Planet()
{

}

Planet::Planet(point p, int oID, int cID, int dl)
{
	initPlanet(p, oID, cID, dl);
}

void Planet::initPlanet(point p, int oID, int cID, int dl)
{
	setLoc(p);
	setOriginRace(oID, dl);
	setControlRace(cID, dl);
	setDangerLevel(dl);
	raceMajorStatus = RMS_FREE;
}

void Planet::setDangerLevel(int dl)
{
	dangerLevel = dl;
}

void Planet::setOriginRace(int oID, int dl)
{
	originRaceID = oID;
	setDangerLevel(dl);
}

void Planet::setControlRace(int cID, int dl)
{
	controlRaceID = cID;
	setDangerLevel(dl);
}

void Planet::setLoc(point p)
{
	loc = p;
}

void Planet::setRaceMajorStatus(RaceMajorStatus rms)
{
	raceMajorStatus = rms;
}

int Planet::getOriginRaceID()
{
	return originRaceID;
}

int Planet::getControlRaceID()
{
	return controlRaceID;
}

int Planet::getDangerLevel()
{
	return dangerLevel;
}

point Planet::getLoc()
{
	return loc;
}

RaceMajorStatus Planet::getRaceMajorStatus()
{
	return raceMajorStatus;
}

void Planet::save(std::ofstream& os) const
{
	loc.save(os);
	os.write(reinterpret_cast<const char*>(&originRaceID), sizeof(int));
	os.write(reinterpret_cast<const char*>(&controlRaceID), sizeof(int));
	os.write(reinterpret_cast<const char*>(&raceMajorStatus), sizeof(RaceMajorStatus));
	os.write(reinterpret_cast<const char*>(&dangerLevel), sizeof(int));
}

void Planet::load(std::ifstream& is)
{
	loc.load(is);
	is.read(reinterpret_cast<char*>(&originRaceID), sizeof(int));
	is.read(reinterpret_cast<char*>(&controlRaceID), sizeof(int));
	is.read(reinterpret_cast<char*>(&raceMajorStatus), sizeof(RaceMajorStatus));
	is.read(reinterpret_cast<char*>(&dangerLevel), sizeof(int));
}