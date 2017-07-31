#ifndef OUTPUT_H
#define OUTPUT_H

#include "map.h"
#include "graphics.h"
#include "mob.h"
#include "menu.h"

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
        void printShipStatsSection(ship_mob *);
        void printShipGraphic(ship_mob *);
        void addModuleGraphic(ship_mob *, int,int,int);
        void updateLOSBrightness(map *, point, chtype &);
        void updateGridCharacters(map *, point, chtype &);
        void addMiscShipGraphicDetails(int);
        void printMeter(module *, int,int,int, module_type,color_type,int);
        void printCrewMeter(module *, int,int,int);
        void printWeaponShieldEngineGraphic(module *,int,int,int);
        void delayAndUpdate(int);
        void displayMonitor(monitor_type, ship_mob *);
        void displayMenu(menu *);
        void displayNPCShipInfo(ship_mob *);
        void displayNPCShipGraphic(ship_mob *);
        void initNPCShipPixels(ship_mob *);
        void runShipDesignCADisplayRule(ship_mob *);
        void setNPCShipYCenterPixels(ship_mob *);
        void drawShipDesign(ship_mob *);
        int numShipPixelsAdj(int,int,int);
        bool addMessage(std::string,color_pair);
        gfx_engine *getGFXEngine();
        chtype getSymbol(map *,point);
        void save(std::ofstream &) const;
        void load(std::ifstream &);
    private:
        point upper_left;
        gfx_engine gfx_obj;
        msgbuffer mbuffer;
        int npc_ship_pixels[NPCSHIP_PIXEL_MAXHEIGHT][NPCSHIP_PIXEL_MAXWIDTH];
        int npc_ship_pixels_temp[NPCSHIP_PIXEL_MAXHEIGHT][NPCSHIP_PIXEL_MAXWIDTH];
};

color_type getFuelMeterColor(module *m);

chtype getShipAdditiveModuloChtypeIndex(chtype,chtype);

chtype getShipMultiplicativeModuloChtypeIndex(chtype,chtype);

#endif
