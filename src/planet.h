#ifndef PLANET_H_
#define PLANET_H_

#include "mob.h"

enum RaceMajorStatus
{
    RMS_FREE,
    RMS_ENSLAVED,
    RMS_DESTROYED
};

class Planet
{
    public:
    	Planet();
        Planet(point, int, int, int);
    	void initPlanet(point, int, int, int);
    	void setOriginRace(int, int);
    	void setControlRace(int, int);
        void setLoc(point);
        void setRaceMajorStatus(RaceMajorStatus);
        void setDangerLevel(int);
        int getOriginRaceID();
        int getControlRaceID();
        int getDangerLevel();
        point getLoc();
        RaceMajorStatus getRaceMajorStatus();
        void save(std::ofstream&) const;
        void load(std::ifstream&);
    private:
        point loc;
        RaceMajorStatus raceMajorStatus;
    	int originRaceID;
    	int controlRaceID;
        int dangerLevel;
};

#endif