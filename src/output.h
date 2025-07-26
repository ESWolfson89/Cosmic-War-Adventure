#ifndef OUTPUT_H
#define OUTPUT_H

#include "map.h"
#include "graphics.h"
#include "mob.h"
#include "menu.h"
#include "input.h"
#include "region.h"
#include "globals.h"

#define NUMMESSAGELINES 10

#define NPCSHIP_PIXEL_MAXWIDTH 40
#define NPCSHIP_PIXEL_MAXHEIGHT 15

/*
    add to end of displayNPCShipGraphic:

    gfx_obj.drawRectangle(color_gray ,point((SHOWWID+37-s->getMaxNumModules())*TILEWID-2,8*TILEHGT-2),point(s->getMaxNumModules()*TILEWID+4,7*TILEHGT+4),true);

    for (int i = 0; i < s->getNumInstalledModules(); ++i)
    {
        addModuleGraphic(s,i,s->getMaxNumModules(),-2);
    }
*/

enum tab_type
{
    TABTYPE_PLAYAREA,
    TABTYPE_PLAYERSHIP
};

enum monitor_type
{
    MONITOR_TARGETINFO,
    MONITOR_CONVERSE
};

class msgbuffer
{
    public:
        msgbuffer();
        bool addMessage(std::string m, color_pair c);
        void printMessages();
        void deleteAllMessages();
        int getMessageSize();
        int getStringCharacter(int);
        color_pair getMessageColorData(int);
    private:
        std::string message_string;
        int message_cursor;
        color_pair message_color_data[NUMMESSAGELINES*GRIDWID];
};

class display
{
    public:
        display();
        void printCell(map *, point, chtype);
        void addChar(chtype,point);
        void addString(std::string,color_pair,point);
        void printMap(map *);
        void updateUpperLeft(point,point);
        void clearRange(point,point);
        void printMessages();
        void clearAndDeleteAllMessages();
        void clearMessages();
        void printMonitorWindow();
        void printWindowBorders(std::string, std::string, tab_type);
        void printShipStatsSection(MobShip *);
        void printShipGraphic(MobShip *, int);
        void addModuleGraphic(MobShip *, int,int,int);
        void updateLOSBrightness(map *, point, chtype &);
        void updateGridCharacters(map *, point, chtype &);
        void addPlayerShipGraphicDetails(int);
        void printMeter(module *, int,int,int, module_type,color_type,int);
        void printCrewMeter(module *, int,int,int);
        void printWeaponShieldEngineGraphic(module *,int,int,int);
        void delayAndUpdate(int);
        void displayMonitor(monitor_type, MobShip *);
        void displayMenu(menu *);
        void displayNPCShipInfo(MobShip *);
        void displayNPCShipGraphic(MobShip *);
        void initNPCShipPixels(MobShip *);
        void runShipDesignCADisplayRule(MobShip *);
        void setNPCShipYCenterPixels(MobShip *);
        void drawNPCShipDesign(MobShip *);
        void displayMachineBox();
        int numShipPixelsAdj(int,int,int);
        bool addMessage(std::string,color_pair);
        chtype getSymbol(map *,point);
        void save(std::ofstream &) const;
        void load(std::ifstream &);
    private:
        point upper_left;
        msgbuffer mbuffer;
        int npc_ship_pixels[NPCSHIP_PIXEL_MAXHEIGHT][NPCSHIP_PIXEL_MAXWIDTH];
        int npc_ship_pixels_temp[NPCSHIP_PIXEL_MAXHEIGHT][NPCSHIP_PIXEL_MAXWIDTH];
};

color_type getFuelMeterColor(module *m);

chtype getShipAdditiveModuloChtypeIndex(chtype,chtype);

chtype getShipMultiplicativeModuloChtypeIndex(chtype,chtype);

void msgeAdd(std::string, color_pair);

void reDisplay(bool);

void reDisplayWithoutUpdate(bool);

void printWindowBorders();

void printMobCells();

void clearAllFireCellsInRange(map*, point, int);

void clearAllFireCells(map*);

void printAndSetFireCell(map*, point, fire_t);

void addHitSprite(map*, point);

void createDamagingExplosionAnimation(point, int, int, fire_t, bool);

void printFireCircle(point, int, fire_t);

void calculatePlayerLOS();

void updateAllLastSymbols(cell *);

void changeMobTile(point, point, mob_t);

void setMobTileToNIL(map*, point);

void setMobTile(map*, point, mob_t);

bool chtypeEqual(chtype, chtype);

extern display display_obj;
extern tab_type current_tab;

#endif
