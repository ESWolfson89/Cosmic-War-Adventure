#include "output.h"

display::display()
{
    upper_left.set(0,0);
}

void display::addChar(chtype ct, point p)
{
    gfx_obj.addBitmapCharacter(ct,p);
}

void display::addString(std::string s, color_pair col_p, point p)
{
    gfx_obj.addBitmapString(s,col_p,p);
}

// create black rectangle on screen (in TILEWID * TILEHGT units)
void display::clearRange(point p1, point p2)
{
    for (int x = p1.x(); x <= p2.x(); ++x)
    {
        for (int y = p1.y(); y <= p2.y(); ++y)
        {
            addChar(blanktile_ch,point(x,y));
        }
    }
}

void display::printMap(map *m)
{
    point p;
    point mp = m->getSize();

    if (mp.x() >= SHOWWID)
        mp.setx(SHOWWID);
    if (mp.y() >= SHOWHGT)
        mp.sety(SHOWHGT);

    for (int x = upper_left.x(); x < mp.x() + upper_left.x(); ++x)
    {
        for (int y = upper_left.y(); y < mp.y() + upper_left.y(); ++y)
        {
            p.set(x,y);

            printCell(m,p,getSymbol(m,p));
        }
    }
}

void display::updateUpperLeft(point loc, point map_size)
{
    // scroll x
    // if near border, dock birds eye view
    if (map_size.x() < SHOWWID || loc.x() < HALFWID)
        upper_left.setx(0);
    else if (loc.x() > map_size.x() - HALFWID - 1)
        upper_left.setx(map_size.x() - SHOWWID);
    else // if not near border, scroll
        upper_left.setx(loc.x() - HALFWID + 1);

    // scroll y
    // if near border, dock birds eye view
    if (map_size.y() < SHOWHGT || loc.y() < HALFHGT)
        upper_left.sety(0);
    else if (loc.y() > map_size.y() - HALFHGT - 1)
        upper_left.sety(map_size.y() - SHOWHGT);
    else // otherwise, scroll
        upper_left.sety(loc.y() - HALFHGT + 1);
}

void display::printCell(map *m, point p, chtype ct)
{
    // <<<<<<<<<<<<<<<<map size must be a factor of 2 for this to work properly>>>>>>>>>>>>>>>>>
    point offset = point(std::max(0,(SHOWWID - m->getSize().x())/2),std::max(0,(SHOWHGT - m->getSize().y())/2));
    point np = point(p.x() - upper_left.x() + offset.x() + 1,p.y() - upper_left.y() + offset.y() + 1);
    // make sure we don't print out of playing field
    bool print_condition = inRange(np,point(1,1),point(SHOWWID,SHOWHGT));

    // print only those cells in range of "visible" (SHOW...) map
    if (print_condition)
    {
        // check field of view display parameters
        updateGridCharacters(m,p,ct);
        updateLOSBrightness(m,p,ct);
        addChar(ct,np);
    }
}

void display::updateGridCharacters(map *m, point p, chtype &ct)
{
    if (!m->getm(p))
    {
        ct = blank_ch;
    }
}

// # # #
// # #
// #

void display::updateLOSBrightness(map *m, point p, chtype &ct)
{
    if (!m->getv(p))
    {
        ct.color.fg = getDimmedColor(ct.color.fg,2,0);
        ct.color.bg = getDimmedColor(ct.color.bg,2,0);
    }
}

chtype display::getSymbol(map *m, point p)
{
    // print in priority depicted in cell.h
    if (m->getFire(p) != NIL_f)
        return fire_symbol[(int)m->getFire(p)];

    // ...Do NOT print NPCs if not in field of view
    else if (m->getMob(p) != NIL_m && m->getv(p))
        return mob_symbol[(int)m->getMob(p)];

    else if (m->getItem(p) != NIL_i)
        return item_symbol[(int)m->getItem(p)];

    return backdrop_symbol[(int)m->getBackdrop(p)];
}

gfx_engine *display::getGFXEngine()
{
    return &gfx_obj;
}

void display::printMessages()
{
    chtype ch;
    for (int i = 0; i < mbuffer.getMessageSize(); ++i)
    {
       ch.ascii = mbuffer.getStringCharacter(i);
       ch.color = mbuffer.getMessageColorData(i);
       gfx_obj.addBitmapCharacter(ch,point(i % GRIDWID, (GRIDHGT - NUMMESSAGELINES) + (int)(i / GRIDWID)));
    }
}

void display::clearAndDeleteAllMessages()
{
	clearMessages();
	mbuffer.deleteAllMessages();
}

void display::clearMessages()
{
	for (int i = 0; i < NUMMESSAGELINES; ++i)
    {
        gfx_obj.addBitmapString("                                                                                ",cp_blackonblack,point(0,GRIDHGT-NUMMESSAGELINES+i));
    }
}

bool display::addMessage(std::string m, color_pair col)
{
    return mbuffer.addMessage(m, col);
}

void display::displayMonitor(monitor_type mt, ship_mob *mb)
{
    switch(mt)
    {
        case(MONITOR_TARGETINFO):
             displayNPCShipInfo(mb);
             break;
        default:
            break;
    }
}

void display::displayMenu(menu *mu)
{
    point loc = mu->getLoc();
    point area = mu->getArea();

    gfx_obj.drawRectangle(color_black,point(TILEWID*loc.x(),TILEHGT*loc.y()),point(area.x()*TILEWID,area.y()*TILEHGT),true);
    gfx_obj.drawRectangle(color_white,point(TILEWID*loc.x()-1,TILEHGT*loc.y()-1),point(area.x()*TILEWID+2,area.y()*TILEHGT+2),false);

    if (mu->getNumMenuMainTextStrings() > 0)
        addString(mu->getMenuMainText(0),cp_whiteonblack,point(((loc.x()+area.x())/2)-((int)mu->getMenuMainText(0).size()/2)+1,loc.y()));

    for (int i = 0; i < mu->getNumMenuItems(); ++i)
    {
        addChar(mu->getMenuItem(i).menu_symbol,point(loc.x()+1,loc.y()+i+2));
        addString(mu->getMenuItem(i).description,(i == mu->getSelectionIndex() ? cp_blackonwhite : cp_whiteonblack),point(loc.x() + 3,loc.y() + i + 2));
    }

    gfx_obj.updateScreen();
}

/*
SHIP DESIGN GRAPHIC CODE STARTS HERE
*/

void display::displayNPCShipGraphic(ship_mob *s)
{
    initNPCShipPixels(s);
    runShipDesignCADisplayRule(s);
    setNPCShipYCenterPixels(s);
    drawShipDesign(s);
}

void display::initNPCShipPixels(ship_mob *s)
{
    for (int i = 0; i < NPCSHIP_PIXEL_MAXHEIGHT; ++i)
    for (int j = 0; j < NPCSHIP_PIXEL_MAXWIDTH; ++j)
    {
        if (j > NPCSHIP_PIXEL_MAXWIDTH - s->getDesignStruct().x_extension)
        {
            npc_ship_pixels[i][j] = 1;
            npc_ship_pixels_temp[i][j] = 1;
        }
        else
        {
            npc_ship_pixels[i][j] = 0;
            npc_ship_pixels_temp[i][j] = 0;
        }
    }
}

void display::runShipDesignCADisplayRule(ship_mob *s)
{
    int num_adj = 0;
    point adj;
    for (int n = 0; n < s->getDesignStruct().ca_ship_gens; ++n)
    {
        for (int i = 1; i < NPCSHIP_PIXEL_MAXHEIGHT - 1; ++i)
        for (int j = NPCSHIP_PIXEL_MAXWIDTH - s->getDesignStruct().x_extension + 1; j < NPCSHIP_PIXEL_MAXWIDTH - 1; ++j)
        {
            num_adj = 0;
            for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
            {
                adj.set(dx+j,dy+i);
                if (dy != 0 || dx != 0)
                if (npc_ship_pixels[adj.y()][adj.x()] > 0)
                    num_adj += npc_ship_pixels[adj.y()][adj.x()];
            }

            num_adj = num_adj % 9;

            if ((num_adj == 2 || num_adj == 3) && npc_ship_pixels[i][j] == 1)
                npc_ship_pixels_temp[i][j] = 1;
            else if (num_adj == 3 && npc_ship_pixels[i][j] == 0)
                npc_ship_pixels_temp[i][j] = 1;
            else if (num_adj == 4)
                npc_ship_pixels_temp[i][j] = 2;
            else if (num_adj == 5)
                npc_ship_pixels_temp[i][j] = 3;
            else
                npc_ship_pixels_temp[i][j] = 0;
        }

        for (int i = 1; i < NPCSHIP_PIXEL_MAXHEIGHT - 1; ++i)
        for (int j = 1; j < NPCSHIP_PIXEL_MAXWIDTH - 1; ++j)
        {
            npc_ship_pixels[i][j] = npc_ship_pixels_temp[i][j];
        }
    }
}

void display::setNPCShipYCenterPixels(ship_mob *s)
{
    for (int i = 1; i < NPCSHIP_PIXEL_MAXHEIGHT - 1; ++i)
    for (int j = 1; j < NPCSHIP_PIXEL_MAXWIDTH - 1; ++j)
    {
        if (i == 7 && j >= NPCSHIP_PIXEL_MAXWIDTH - s->getDesignStruct().x_extension + 1)
           npc_ship_pixels[i][j] = 1;
    }
}

void display::drawShipDesign(ship_mob *s)
{
    chtype primary_design_ch, secondary_design_ch, front_tile_ch;

    secondary_design_ch.ascii = s->getDesignStruct().ca_start_val;
    secondary_design_ch.color.fg = s->getDesignStruct().ca_secondary_color;
    secondary_design_ch.color.bg = getDimmedColor(s->getShipSymbol().color.fg,3,0);

    primary_design_ch.ascii = 219;
    primary_design_ch.color = s->getShipSymbol().color;

    front_tile_ch.ascii = gray_rightarrow.ascii;
    front_tile_ch.color = s->getShipSymbol().color;

    point loc;

    // Add bulk of ship design to monitor
    for (int i = 0; i < NPCSHIP_PIXEL_MAXHEIGHT - 1; ++i)
    for (int j = 1; j < NPCSHIP_PIXEL_MAXWIDTH; ++j)
    {
        loc.set(SHOWWID+2+j,4+i);

        if (i == 0 || j == 0 || i == NPCSHIP_PIXEL_MAXHEIGHT - 1 || j == NPCSHIP_PIXEL_MAXWIDTH - 1)
            addChar(blank_ch,loc);
        else if (npc_ship_pixels[i][j] == 1 || npc_ship_pixels[i][j] == 2)
            addChar(primary_design_ch,loc);
        else if (npc_ship_pixels[i][j] == 3)
            addChar(secondary_design_ch,loc);
        else if (npc_ship_pixels[i][j-1] > 0 && npc_ship_pixels[i][j+1] > 0 && npc_ship_pixels[i][j] == 0 && j > 1 && j < NPCSHIP_PIXEL_MAXWIDTH - 2)
            addChar(gray_horizontal_pipe,loc);
        else if (npc_ship_pixels[i-1][j] > 0 && npc_ship_pixels[i+1][j] > 0 && npc_ship_pixels[i][j] == 0 && i > 1 && i < NPCSHIP_PIXEL_MAXHEIGHT - 2)
            addChar(gray_vertical_pipe,loc);
        else if (i > 1 && j > 1 && i < NPCSHIP_PIXEL_MAXHEIGHT - 2 && j < NPCSHIP_PIXEL_MAXWIDTH - 2 && numShipPixelsAdj(i,j,0) < 8 && npc_ship_pixels[i][j] == 0)
            addChar(secondary_design_ch,loc);
    }

    // ...add exhausts, flame trails
    for (int i = 1; i < NPCSHIP_PIXEL_MAXHEIGHT - 1; ++i)
    for (int j = 1; j < NPCSHIP_PIXEL_MAXWIDTH - 5; ++j)
    {
        loc.set(SHOWWID+2+j,4+i);

        if (npc_ship_pixels[i][j+2] > 0)
        {
            addChar(gray_horizontal_pipe,addPoints(loc,point(1,0)));
            addChar(lightblue_flame,loc);
            break;
        }
    }

    // ...add front ship |> tiles
    for (int i = 1; i < NPCSHIP_PIXEL_MAXHEIGHT - 1; ++i)
    for (int j = NPCSHIP_PIXEL_MAXWIDTH - 1; j >= 2; --j)
    {
        loc.set(SHOWWID+2+j,4+i);

        if (npc_ship_pixels[i][j-1] > 0)
        {
            addChar(front_tile_ch,loc);
            break;
        }
    }
}

int display::numShipPixelsAdj(int i, int j, int val)
{
    int num_adj = 0;

    for (int dy = -1; dy <= 1; ++dy)
    for (int dx = -1; dx <= 1; ++dx)
    {
        if (dx != 0 || dy != 0)
        if (npc_ship_pixels[i+dy][j+dx] == val)
            num_adj++;
    }

    return num_adj;
}

/*
SHIP DESIGN GRAPHIC CODE ENDS HERE
*/

void display::displayNPCShipInfo(ship_mob *s)
{
    gfx_obj.drawRectangle(color_darkgray,point((SHOWWID+2)*TILEWID,0),point(SCREENWID-((SHOWWID+2)*TILEWID)-TILEWID+1,TILEHGT + (SHOWHGT*TILEHGT)/2),true);
    printMonitorWindow();

    std::string ship_total_name = s->getShipName();
    std::string hull_string = "Hull: " + int2String(s->getHullStatus()) + "/" + int2String(s->getMaxHull());
    std::string crew_string = "Crew: ";
    std::string evasion_string = "Evasion: " + double2String(s->getEvasion());
    std::string accuracy_string = "Accuracy: " + double2String(s->getAccuracy());
    std::string speed_string = "Speed: " + double2String(s->getSpeed());
    std::string shields_string = "Shields: " + int2String(s->getTotalMTFillRemaining(MODULE_SHIELD)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_SHIELD));

    if (s->crewOperable())
        crew_string += int2String(s->getTotalMTFillRemaining(MODULE_CREW)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_CREW));
    else
        crew_string += "AUTOMATED";

    addString(ship_total_name,cp_whiteonblack,point(SHOWWID+(GRIDWID-SHOWWID)/2 - ship_total_name.size()/2,0));
    gfx_obj.drawRectangle(color_white,point((SHOWWID+2)*TILEWID,TILEHGT-1),point(SCREENWID-((SHOWWID+2)*TILEWID)-TILEWID+1,(SHOWHGT*TILEHGT)/2),false);
    addString(hull_string+"                      ",cp_grayonblack,point(SHOWWID+3,1));
    addString(crew_string+"                      ",cp_lightgrayonblack,point(SHOWWID+3,2));
    addString(shields_string+"                      ",cp_lightblueonblack,point(SHOWWID+3,3));
    addString(speed_string+"       ",cp_purpleonblack,point(SHOWWID+23,1));
    addString(accuracy_string+"       ",cp_redonblack,point(SHOWWID+23,2));
    addString(evasion_string+"       ",cp_darkredonblack,point(SHOWWID+23,3));

    displayNPCShipGraphic(s);
    return;
}

void display::printWindowBorders(std::string current_map_name, std::string player_ship_name, tab_type tt)
{
    gfx_obj.drawRectangle(color_darkgray,point(0,0),point(SCREENWID,(SHOWHGT+2)*TILEHGT-1),true);
    // display starmap window
    gfx_obj.addBitmapString(current_map_name,(tt == TABTYPE_PLAYAREA ? cp_blackonwhite : cp_whiteonblack),point(SHOWWID/2-(int)current_map_name.size()/2,0));
    gfx_obj.drawRectangle(color_black,point(TILEWID,TILEHGT),point(SHOWWID*TILEWID,SHOWHGT*TILEHGT),true);
    gfx_obj.drawRectangle(color_white,point(TILEWID-1,TILEHGT-1),point(SHOWWID*TILEWID+2,SHOWHGT*TILEHGT+2),false);
    // display animation/misc window
    printMonitorWindow();
    // display ship window/ verious stats/ inventory etc...
    gfx_obj.addBitmapString(player_ship_name,(tt == TABTYPE_PLAYERSHIP ? cp_blackonwhite : cp_whiteonblack),point(SHOWWID+(GRIDWID-SHOWWID-10)/2+2,SHOWHGT/2+1));
    gfx_obj.drawRectangle(color_black,point((SHOWWID+2)*TILEWID+1,(SHOWHGT*TILEHGT)/2+TILEHGT*2),point(SCREENWID-((SHOWWID+2)*TILEWID)-TILEWID-1,(SHOWHGT*TILEHGT)/2-TILEHGT),true);
    gfx_obj.drawRectangle(color_white,point((SHOWWID+2)*TILEWID,(SHOWHGT*TILEHGT)/2+(TILEHGT*2)-1),point(SCREENWID-((SHOWWID+2)*TILEWID)-TILEWID+1,(SHOWHGT*TILEHGT)/2-TILEHGT+2),false);
}

void display::printMonitorWindow()
{
    gfx_obj.addBitmapString("TARGET",cp_whiteonblack,point(SHOWWID+(GRIDWID-SHOWWID-3)/2,0));
    gfx_obj.drawRectangle(color_black,point((SHOWWID+2)*TILEWID+1,TILEHGT),point(SCREENWID-((SHOWWID+2)*TILEWID)-TILEWID-1,(SHOWHGT*TILEHGT)/2-2),true);
    gfx_obj.drawRectangle(color_white,point((SHOWWID+2)*TILEWID,TILEHGT-1),point(SCREENWID-((SHOWWID+2)*TILEWID)-TILEWID+1,(SHOWHGT*TILEHGT)/2),false);
}

void display::printShipStatsSection(ship_mob *s)
{
    std::string hull_string = "Hull: " + int2String((int)std::max(0,s->getHullStatus())) + "/" + int2String(s->getMaxHull());
    std::string crew_string = "Crew: " + int2String(s->getTotalMTFillRemaining(MODULE_CREW)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_CREW));
    std::string fuel_string = "Fuel: " + int2String(s->getTotalMTFillRemaining(MODULE_FUEL)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_FUEL));
    std::string credit_string = "Credits: " + uint642String(s->getNumCredits());
    std::string spd_string = "Speed: " + double2String(s->getSpeed());
    std::string acc_string = "Accuracy: " + double2String(s->getAccuracy());
    std::string eva_string = "Evasion: " + double2String(s->getEvasion());
    std::string shields_string = "Shields: " + int2String(s->getTotalMTFillRemaining(MODULE_SHIELD)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_SHIELD));
    addString(hull_string+"                      ",cp_grayonblack,point(SHOWWID+3,SHOWHGT/2+2));
    addString(crew_string+"                      ",cp_lightgrayonblack,point(SHOWWID+3,SHOWHGT/2+3));
    addString(credit_string+"                      ",cp_whiteonblack,point(SHOWWID+3,SHOWHGT/2+5));
    addString(shields_string+"                      ",cp_lightblueonblack,point(SHOWWID+3,SHOWHGT/2+4));
    addString(spd_string+"       ",cp_purpleonblack,point(SHOWWID+23,SHOWHGT/2+2));
    addString(acc_string+"       ",cp_redonblack,point(SHOWWID+23,SHOWHGT/2+3));
    addString(eva_string+"       ",cp_darkredonblack,point(SHOWWID+23,SHOWHGT/2+4));
    addString(fuel_string+"       ",cp_brownonblack,point(SHOWWID+23,SHOWHGT/2+5));
    printShipGraphic(s);
}

// print ship window inside
void display::printShipGraphic(ship_mob *s)
{
    int ship_body_size = s->getMaxNumModules();
    int num_ship_modules = s->getNumInstalledModules();
    addMiscShipGraphicDetails(ship_body_size);
    for (int i = 0; i < num_ship_modules; ++i)
    {
        addModuleGraphic(s,i,ship_body_size,SHOWHGT/2);
    }
}

// helper...
void display::addMiscShipGraphicDetails(int ship_body_size)
{
    for (int x = 0; x < ship_body_size+2; ++x)
    {
        for (int y = 0; y < 9; ++y)
        {
            addChar(gray_rect,point(SHOWWID+37-x,SHOWHGT/2+9+y));
        }
    }
    for (int i = 0; i < 3; ++i)
        addChar(gray_rect,point(SHOWWID+40,SHOWHGT/2+12+i));
    for (int i = 0; i < 7; ++i)
        addChar(gray_rect,point(SHOWWID+38,SHOWHGT/2+10+i));
    for (int i = 0; i < 5; ++i)
        addChar(gray_rect,point(SHOWWID+39,SHOWHGT/2+11+i));
    for (int i = 0; i < 9; ++i)
        addChar(gray_horizontal_pipe,point(SHOWWID+35-ship_body_size,SHOWHGT/2+9+i));
    for (int i = 0; i < 9; ++i)
        addChar(lightblue_flame,point(SHOWWID+34-ship_body_size,SHOWHGT/2+9+i));
    for (int i = 0; i < 7; ++i)
        addChar(lightblue_flame,point(SHOWWID+33-ship_body_size,SHOWHGT/2+10+i));
    for (int i = 0; i < 5; ++i)
        addChar(lightblue_flame,point(SHOWWID+32-ship_body_size,SHOWHGT/2+11+i));
    for (int i = 0; i < 3; ++i)
        addChar(lightblue_flame,point(SHOWWID+31-ship_body_size,SHOWHGT/2+12+i));
    addChar(gray_rightarrow,point(SHOWWID+41,SHOWHGT/2+12));
    addChar(gray_rightarrow,point(SHOWWID+41,SHOWHGT/2+13));
    addChar(gray_rightarrow,point(SHOWWID+41,SHOWHGT/2+14));
}

// print module graphic bar
void display::addModuleGraphic(ship_mob *s, int i, int ship_body_size,int y_global_offset)
{
    point arrow_loc;
    switch(s->getModule(i)->getModuleType())
    {
        case(MODULE_FUEL):
        {
            gfx_obj.addBitmapVerticalString("f      ",cp_lightgrayonblack,point(SHOWWID+36-ship_body_size+i+1,y_global_offset+10));
            printMeter(s->getModule(i),ship_body_size,i,TILEWID,MODULE_FUEL,getFuelMeterColor(s->getModule(i)),y_global_offset);
            break;
        }
        case(MODULE_CREW):
        {
            printCrewMeter(s->getModule(i),ship_body_size,i,y_global_offset);
            break;
        }
        case(MODULE_WEAPON):
        {
            printWeaponShieldEngineGraphic(s->getModule(i),ship_body_size,i,y_global_offset);
            printMeter(s->getModule(i),ship_body_size,i,1,MODULE_WEAPON,color_red,y_global_offset);
            break;
        }
        case(MODULE_SHIELD):
        {
            printWeaponShieldEngineGraphic(s->getModule(i),ship_body_size,i,y_global_offset);
            printMeter(s->getModule(i),ship_body_size,i,1,MODULE_SHIELD,color_white,y_global_offset);
            break;
        }
        case(MODULE_ENGINE):
        {
            printWeaponShieldEngineGraphic(s->getModule(i),ship_body_size,i,y_global_offset);
            printMeter(s->getModule(i),ship_body_size,i,1,MODULE_ENGINE,color_orange,y_global_offset);
            break;
        }
        default:
            break;
    }
    arrow_loc = point(SHOWWID+36-ship_body_size+i+1,y_global_offset+17);
    if (i == s->getModuleSelectionIndex())
        addChar(up_arrow_selector,arrow_loc);
}

// print fill meter of module
void display::printMeter(module *m, int ship_body_size, int i, int fill_width, module_type mt, color_type fill_color, int y_global_offset)
{
    int quantity = m->getFillQuantity();
    int max_quantity = m->getMaxFillQuantity();
    int max_qmq_print_ratio = 6*TILEHGT;
    int qmq_print_ratio = (int)(quantity*max_qmq_print_ratio/max_quantity);
    point p = point(SHOWWID+36-ship_body_size+i+1,y_global_offset+10);
    if (fill_width > 1)
        gfx_obj.drawRectangle(fill_color,point(p.x()*TILEWID,(p.y()+1)*TILEHGT+max_qmq_print_ratio-qmq_print_ratio),point(fill_width,qmq_print_ratio),true);
    else
        gfx_obj.drawRectangle(fill_color,point((p.x()+1)*TILEWID-1,(p.y()+1)*TILEHGT+max_qmq_print_ratio-qmq_print_ratio),point(fill_width,qmq_print_ratio),true);
}

// * * * *
//
// * * * *
void display::printCrewMeter(module *m, int ship_body_size, int i, int y_global_offset)
{
    int quantity = m->getFillQuantity();
    int max_qmq_print_ratio = 6*TILEHGT;
    int dot_offset_width_factor = (TILEWID % 2 == 1) ? TILEWID - 1 : TILEWID;
    point dot_offset;
    point p = point(SHOWWID+36-ship_body_size+i+1,y_global_offset+10);
    gfx_obj.addBitmapVerticalString("c      ",cp_lightgrayonblack,p);
    for (int n = 0; n < quantity; ++n)
    {
        dot_offset.set((2*n)%dot_offset_width_factor,max_qmq_print_ratio-((n/(dot_offset_width_factor/2))*2)-1);
        gfx_obj.drawRectangle(color_lightgray,point(p.x()*TILEWID+dot_offset.x(),(p.y()+1)*TILEHGT+dot_offset.y()),point(1,1),true);
    }

    for (int i = 5; i >= 1; --i)
    {
        if (m->getMaxFillQuantity() <= i*24)
            gfx_obj.addBitmapCharacter(crewpod_display_symbol,point(p.x(),p.y()+6-i));
        else
            return;
    }
}

void display::printWeaponShieldEngineGraphic(module *m, int ship_body_size, int i, int y_global_offset)
{
    point p = point(SHOWWID+36-ship_body_size+i+1,y_global_offset+10);
    chtype ct;
    chtype first_letter;

    switch(m->getModuleType())
    {
        case(MODULE_WEAPON):
            ct = first_letter = m->getWeaponStruct().disp_chtype;
            first_letter.ascii = (int)std::toupper(m->getWeaponStruct().name_modifier[0]);
            break;
        case(MODULE_SHIELD):
            ct = first_letter = m->getShieldStruct().disp_chtype;
            first_letter.ascii = (int)std::toupper(m->getShieldStruct().name_modifier[0]);
            break;
        case(MODULE_ENGINE):
            ct = first_letter = m->getEngineStruct().disp_chtype;
            first_letter.ascii = (int)std::toupper(m->getEngineStruct().name_modifier[0]);
            break;
        default:
            break;
    }

    for (int i = 0; i < 6; ++i)
         addChar(ct,point(p.x(),p.y()+1+i));

    addChar(first_letter,p);

    if (m->getModuleType() == MODULE_WEAPON)
    if (m->getWeaponStruct().num_shots > 1)
        gfx_obj.addBitmapVerticalString(int2String(m->getWeaponStruct().num_shots),cp_whiteonblack,point(p.x(),p.y()+6));

    if (m->getModuleType() == MODULE_SHIELD)
        gfx_obj.addBitmapVerticalString(int2String(m->getMaxFillQuantity()),cp_whiteonblack,point(p.x(),p.y()+1));
}

void display::delayAndUpdate(int ms)
{
    gfx_obj.updateScreen();
    SDL_Delay(ms);
}

void display::save(std::ofstream & os) const
{
    upper_left.save(os);
}

void display::load(std::ifstream & is)
{
    upper_left.load(is);
}

msgbuffer::msgbuffer()
{
  // set defaults
  message_string = "";
  message_cursor = 0;
  for (int i = 0; i < NUMMESSAGELINES * GRIDWID; ++i)
  {
      message_color_data[i] = cp_blackonblack;
  }
}

// assumes m is not larger than 2*GRIDWID
// Complete mess / might need to refactor at some point
bool msgbuffer::addMessage(std::string m, color_pair col)
{
  int space_iter = 0;
  message_string.append(m);
  message_string.append(" ");
  for (int i = 0; i < (int)m.size(); ++i)
  {
    if (i + message_cursor >= NUMMESSAGELINES * GRIDWID)
      break;
    if ((i + message_cursor) % GRIDWID == 0 && i + message_cursor >= GRIDWID)
    {
      space_iter = 0;
      if (message_string[i+message_cursor-1] != ' ')
      {
        do
        {
            space_iter++;
        }while(message_string[i+message_cursor-1-space_iter] != ' ');
        for (int n = 0; n < space_iter; ++n)
            message_string.insert(message_string.begin() + i + message_cursor - 1 - space_iter,' ');
      }
    }
    message_color_data[i+message_cursor] = col;
  }
  message_cursor += m.size() + 1;

  for (int i = 0; i < space_iter; ++i)
  {
    message_color_data[i+message_cursor-1] = col;
  }

  message_cursor += space_iter;

  if (message_cursor > GRIDWID * (NUMMESSAGELINES - 2))
  {
      message_string.append("[SPACE] -> more...");
      for (int i = 1; i < 19; ++i)
      {
        message_color_data[i+message_cursor - 1] = cp_blackonwhite;
      }
      message_cursor += 19;
      return true;
  }
  return false;
}

void msgbuffer::deleteAllMessages()
{
  message_cursor = 0;
  message_string = "";
  for (int i = 0; i < NUMMESSAGELINES * GRIDWID; ++i)
  {
      message_color_data[i] = cp_blackonblack;
  }
}

int msgbuffer::getMessageSize()
{
    return message_string.size();
}

int msgbuffer::getStringCharacter(int i)
{
    return (int)message_string[i];
}

color_pair msgbuffer::getMessageColorData(int i)
{
    return message_color_data[i];
}

color_type getFuelMeterColor(module *m)
{
    if (m->getMaxFillQuantity() >= 1024)
        return color_white;
    return getAnyColor(255,(Uint8)(m->getMaxFillQuantity()/16),(Uint8)(m->getMaxFillQuantity()/4));
}
