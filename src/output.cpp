#include "output.h"

tab_type current_tab;
display display_obj;

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
        return m->getCell(p).getLastFireSymbol();

    // ...Do NOT print NPCs if not in field of view
    else if (m->getMob(p) != NIL_m && m->getv(p))
        return m->getCell(p).getLastMobSymbol();

    else if (m->getItem(p) != NIL_i)
    {
        if (!chtypeEqual(m->getCell(p).getLastItemSymbol(), blank_ch))
           return m->getCell(p).getLastItemSymbol();
    }

    return m->getCell(p).getLastBackdropSymbol();
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

void display::displayMonitor(monitor_type mt, MobShip *mb)
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

void display::displayMachineBox()
{
    point loc = point(MACHINEX_OFFSET, MACHINEY_OFFSET);
    point area = point(MACHINEWID + 1, MACHINEHGT + 4);

    gfx_obj.drawRectangle(color_black, point(TILEWID * loc.x(), TILEHGT * loc.y() - (3*TILEHGT/2)), point(area.x() * TILEWID, area.y() * TILEHGT - 6), true);
    gfx_obj.drawRectangle(color_white, point(TILEWID * loc.x() - 1, TILEHGT * loc.y() - 1 - (3*TILEHGT/2)), point(area.x() * TILEWID + 2, area.y() * TILEHGT - 4), false);

    //gfx_obj.updateScreen();
}

void display::displayMenu(menu* mu)
{
    point loc = mu->getLoc();
    point area = mu->getArea();

    gfx_obj.drawRectangle(color_black,
        point(TILEWID * loc.x(), TILEHGT * loc.y()),
        point(area.x() * TILEWID, area.y() * TILEHGT),
        true);
    gfx_obj.drawRectangle(color_white,
        point(TILEWID * loc.x() - 1, TILEHGT * loc.y() - 1),
        point(area.x() * TILEWID + 2, area.y() * TILEHGT + 2),
        false);

    if (mu->getNumMenuMainTextStrings() > 0)
        addString(mu->getMenuMainText(0),
            cp_whiteonblack,
            point(((loc.x() + area.x()) / 2) - ((int)mu->getMenuMainText(0).size() / 2) + 1, loc.y()));

    const int visibleRows = mu->getVisibleRows();           // area.y - 3
    const int start = mu->getScrollOffset();
    const int end = std::min(start + visibleRows, mu->getNumMenuItems());

    // Draw only the visible slice
    for (int i = start; i < end; ++i)
    {
        const int row = i - start;                          // row 0..visibleRows-1
        const bool selected = (i == mu->getSelectionIndex());

        addChar(mu->getMenuItem(i).menu_symbol,
            point(loc.x() + 1, loc.y() + row + 2));

        addString(mu->getMenuItem(i).description,
            selected ? cp_blackonwhite : cp_whiteonblack,
            point(loc.x() + 3, loc.y() + row + 2));
    }

    gfx_obj.updateScreen();
}

/*
SHIP DESIGN GRAPHIC CODE STARTS HERE
*/

void display::displayNPCShipGraphic(MobShip *s)
{
    initNPCShipPixels(s);
    runShipDesignCADisplayRule(s);
    setNPCShipYCenterPixels(s);
    drawNPCShipDesign(s);
    //drawWeaponModuleTiles(s);
}

chtype getShipWeaponSymbolOnDisplay(weapon_struct& wstruct)
{
    int numShots = wstruct.num_shots;

    chtype ch;

    color_pair cp = wstruct.disp_chtype.color;

    color_type cpf = cp.fg;
    color_type cpb = cp.bg;

    switch (wstruct.wt)
    {
    case(WEAPONTYPE_BLAST):
    case(WEAPONTYPE_BEAM):
        ch.ascii = numShots > 1 ? int2String(numShots)[0] : std::toupper(wstruct.name_modifier[0]);
        cp.fg = cpb;
        cp.bg = cpf;
        break;
    case(WEAPONTYPE_MISSILE):
        ch.ascii = numShots > 1 ? 'D' : (int)'M';
        cp.fg = cpb;
        cp.bg = cpf;
        break;
    case(WEAPONTYPE_PULSE):
        ch.ascii = (int)'*';
        cp = cp_blackonpurple;
        break;
    case(WEAPONTYPE_SPREAD):
        ch.ascii = (int)'*';
        cp = cp_blackonorange;
        break;
    case(WEAPONTYPE_TOXICWALLOP):
        ch.ascii = (int)'*';
        cp = cp_blackongreen;
        break;
    case(WEAPONTYPE_ENERGYWALLOP):
        ch.ascii = (int)'*';
        cp = cp_blackonblue;
        break;
    case(WEAPONTYPE_MECH):
        ch.ascii = wstruct.disp_chtype.ascii;
        cp = cp_blackondarkgray;
        break;
    case(WEAPONTYPE_HELL):
        ch.ascii = 239;
        cp = cp_blackondarkred;
        break;
    default:
        break;
    }

    ch.color = cp;

    return ch;
}

void display::drawWeaponModuleTiles(MobShip* s)
{
    //return;
    const int H = NPCSHIP_PIXEL_MAXHEIGHT;
    const int W = NPCSHIP_PIXEL_MAXWIDTH;

    int count = 0;

    // Use string (name_modifier) as key
    std::unordered_map<std::string, int> weaponMap;

    // Initialize counts to zero
    for (int i = 1; i < NUM_TOTAL_WEAPON_TYPES; i++)
    {
        weaponMap[allbasicweapon_stats[i].name_modifier] = 0;
    }

    // Count installed modules
    for (int i = 0; i < s->getNumInstalledModulesOfType(MODULE_WEAPON); i++)
    {
        Module* m = s->getModule(i);
        if (m->getModuleType() == MODULE_WEAPON)
        {
            weaponMap[m->getWeaponStruct().name_modifier]++;
        }
    }

    // Draw weapons
    for (int i = 1; i < NUM_TOTAL_WEAPON_TYPES; ++i)
    {
        weapon_struct wstruct = allbasicweapon_stats[i];
        const int n = weaponMap[wstruct.name_modifier];
        if (n <= 0) continue;

        const chtype ch = getShipWeaponSymbolOnDisplay(wstruct);
        const shipmob_classtype classType = s->getStatStruct().sctype;

        const int frontOffset = s->getDesignStruct().weaponFrontOffset;

        auto offsetsFor = [](int k) -> std::vector<int>
            {
                switch (k)
                {
                case 1:  return { 0 };
                case 2:  return { -3, +3 };
                case 3:  return { -2, 0, +2 };
                case 4:  return { -4, -2, +2, +4 };
                case 5:  return { -5, -3, 0, +3, +5 };
                case 6:  return { -6, -4, -2, +2, +4, +6 };
                case 7:  return { -3, -2, -1, 0, +1, +2, +3 };
                case 8:  return { -5, -4, -3, -2, +2, +3, +4, +5 };                  
                case 9:  return { 0, -4, -3, -2, -1, +1, +2, +3, +4 };              
                case 10: return { -5, -4, -3, -2, -1, +1, +2, +3, +4, +5 };          
                case 11: return { 0, -5, -4, -3, -2, -1, +1, +2, +3, +4, +5 };      
                case 12: return { -6, -5, -4, -3, -2, -1, +1, +2, +3, +4, +5, +6 };  
                case 13: return { 0, -6, -5, -4, -3, -2, -1, +1, +2, +3, +4, +5, +6 };
                default:
                    return { 0 };
                }
            };

        const int colX = SHOWWID + W - frontOffset - count * static_cast<int>(classType);
        const int baseY = 4 + H / 2;

        for (int dy : offsetsFor(n))
            addChar(ch, point(colX, baseY + dy));

        ++count;
    }
}

void display::initNPCShipPixels(MobShip* s)
{
    const int H = NPCSHIP_PIXEL_MAXHEIGHT;
    const int W = NPCSHIP_PIXEL_MAXWIDTH;
    const auto& ds = s->getDesignStruct();
    const int xStart = W - ds.xExtension;

    for (int i = 0; i < H; ++i)
    {
        for (int j = 0; j < W; ++j)
        {
            const bool filled = (j > xStart);
            const int v = filled ? 1 : 0;

            npc_ship_pixels[i][j] = v;
            npc_ship_pixels_temp[i][j] = v;
            npc_ship_chars[i][j] = blank_ch;
        }
    }
}

void display::runShipDesignCADisplayRule(MobShip* s)
{
    const int H = NPCSHIP_PIXEL_MAXHEIGHT;
    const int W = NPCSHIP_PIXEL_MAXWIDTH;
    const auto& ds = s->getDesignStruct();
    const int xStart = W - ds.xExtension;

    for (int gen = 0; gen < ds.CAShipGenerations; ++gen)
    {
        // Update temp grid from current pixels
        for (int i = 1; i < H - 1; ++i)
        {
            for (int j = xStart + 1; j < W - 1; ++j)
            {
                int adjCount = 0;
                for (int dy = -1; dy <= 1; ++dy)
                {
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        if (dx == 0 && dy == 0) continue;
                        const int y = i + dy;
                        const int x = j + dx;
                        const int val = npc_ship_pixels[y][x];
                        if (val > 0) adjCount += val;
                    }
                }

                adjCount %= 9;

                const int cur = npc_ship_pixels[i][j];
                int next = 0;

                // Rule mapping (kept identical)
                if ((adjCount == 2 || adjCount == 3) && cur == 1)
                    next = 1;
                else if (adjCount == 3 && cur == 0)
                    next = 1;
                else if (adjCount == 4)
                    next = 2;
                else if (adjCount == 5)
                    next = 3;
                else
                    next = 0;

                npc_ship_pixels_temp[i][j] = next;
            }
        }

        // Commit temp to main
        for (int i = 1; i < H - 1; ++i)
        {
            for (int j = 1; j < W - 1; ++j)
            {
                npc_ship_pixels[i][j] = npc_ship_pixels_temp[i][j];
            }
        }
    }
}

void display::setNPCShipYCenterPixels(MobShip* s)
{
    const int H = NPCSHIP_PIXEL_MAXHEIGHT;
    const int W = NPCSHIP_PIXEL_MAXWIDTH;
    const auto& ds = s->getDesignStruct();
    const int xStart = W - ds.xExtension;
    const int centerY = 7; // preserves original behavior

    for (int j = 1; j < W - 1; ++j)
    {
        if (centerY >= 1 && centerY < H - 1 && j >= xStart + 1)
        {
            npc_ship_pixels[centerY][j] = 1;
        }
    }
}

void display::drawNPCShipDesign(MobShip* s)
{
    const int H = NPCSHIP_PIXEL_MAXHEIGHT;
    const int W = NPCSHIP_PIXEL_MAXWIDTH;
    const auto& ds = s->getDesignStruct();

    chtype primary_design_ch;
    chtype secondary_design_ch;
    chtype third_design_ch;
    chtype fourth_design_ch;
    chtype front_tile_ch;

    // Colors/tiles
    primary_design_ch.ascii = 219;
    primary_design_ch.color = s->getShipSymbol().color;

    secondary_design_ch.ascii = ds.CASecondValue; 
    secondary_design_ch.color.fg = ds.ctSecondColor;
    secondary_design_ch.color.bg = getDimmedColor(s->getShipSymbol().color.fg, 3, 0);

    third_design_ch.ascii = ds.CAThirdValue;
    third_design_ch.color.fg = ds.ctThirdColor;
    third_design_ch.color.bg = getDimmedColor(secondary_design_ch.color.fg, 3, 0);

    fourth_design_ch.ascii = ds.CAFourthValue;
    fourth_design_ch.color.fg = ds.ctFourthColor;
    fourth_design_ch.color.bg = getDimmedColor(third_design_ch.color.fg, 3, 0);

    front_tile_ch.ascii = gray_rightarrow.ascii;
    front_tile_ch.color = s->getShipSymbol().color;

    // Fill main body (refactored)
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            // frame blanking on borders
            if (i == 0 || j == 0 || i == H - 1 || j == W - 1) {
                npc_ship_chars[i][j] = blank_ch;
                continue;
            }

            const int cur = npc_ship_pixels[i][j];

            // primary/secondary design fills
            if (cur == 1 || cur == 2) {
                npc_ship_chars[i][j] = primary_design_ch;
                continue;
            }
            if (cur == 3) {
                npc_ship_chars[i][j] = secondary_design_ch;
                continue;
            }

            // interior checks only needed for empty cells
            if (cur != 0) continue;

            const bool inX = (j > 1 && j < W - 2);
            const bool inY = (i > 1 && i < H - 2);

            auto isFilled = [&](int y, int x) {
                const int v = npc_ship_pixels[y][x];
                return v == 1 || v == 2;
                };

            // horizontal pipe between filled neighbors
            if (inX && isFilled(i, j - 1) && isFilled(i, j + 1)) {
                npc_ship_chars[i][j] = gray_horizontal_pipe;
            }
            // vertical pipe between filled neighbors
            else if (inY && isFilled(i - 1, j) && isFilled(i + 1, j)) {
                npc_ship_chars[i][j] = gray_vertical_pipe;
            }
            // secondary accent in sparse neighborhoods
            else if (inX && inY && numShipPixelsAdj(i, j, 0) < 8) {
                npc_ship_chars[i][j] = secondary_design_ch;
            }
        }
    }

    // Exhaust / flame trails
    for (int i = 1; i < H - 1; ++i)
    {
        for (int j = 1; j < W - 5; ++j)
        {
            if (npc_ship_pixels[i][j + 2] > 0)
            {
                npc_ship_chars[i][j + 1] = gray_horizontal_pipe;
                npc_ship_chars[i][j] = s->isActivated() ? ds.shipFlameCh : blank_ch;
                break;
            }
        }
    }

    // Front |> tiles
    for (int i = 1; i < H - 1; ++i)
    {
        for (int j = W - 1; j >= 2; --j)
        {
            if (npc_ship_pixels[i][j - 1] > 0)
            {
                npc_ship_chars[i][j] = front_tile_ch;
                break;
            }
        }
    }

    // Blit layer 1
    for (int i = 1; i < H - 1; ++i)
    {
        for (int j = 1; j < W - 1; ++j)
        {
            addChar(npc_ship_chars[i][j], point(SHOWWID + 3 + j, 4 + i));
        }
    }

    // Accents (third/fourth) based on surrounding matches
    for (int i = 1; i < H - 1; ++i)
    {
        for (int j = 1; j < W - 1; ++j)
        {
            const point loc(SHOWWID + 3 + j, 4 + i);

            if (allSurroundingNPCShipPixelsMatch(i, j, { primary_design_ch }))
            {
                addChar(third_design_ch, loc);
            }
            if (allSurroundingNPCShipPixelsMatch(i, j, { secondary_design_ch }))
            {
                addChar(fourth_design_ch, loc);
            }
        }
    }
}

bool display::allSurroundingNPCShipPixelsMatch(int i, int j, const std::vector<chtype>& matchValues)
{
    auto matches = [&](int y, int x) -> bool {
        const chtype& ch = npc_ship_chars[y][x];
        return std::any_of(matchValues.begin(), matchValues.end(), [&](const chtype& target) {
            return chtypeMatches(ch, target);
            });
        };

    return matches(i, j) &&
        matches(i + 1, j) &&
        matches(i - 1, j) &&
        matches(i + 1, j + 1) &&
        matches(i - 1, j + 1) &&
        matches(i + 1, j - 1) &&
        matches(i - 1, j - 1) &&
        matches(i, j + 1) &&
        matches(i, j - 1);
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

color_type shipDisplayBoxColor(MobShip* s)
{
    race* shipRace = universe.getRace(s->getMobSubAreaGroupID());
    int playerAtt = shipRace->getPlayerAttStatus();

    if (playerAtt > 0)
        return color_green;
    if (playerAtt == 0)
        return color_orange;
    
    return color_red;
}

void display::displayNPCShipInfo(MobShip* s)
{
    const color_type shipBorderColor = shipDisplayBoxColor(s);

    const point topLeft((SHOWWID + 2) * TILEWID, 0);
    const point boxSize(SCREENWID - ((SHOWWID + 2) * TILEWID) - TILEWID + 1,
        TILEHGT + (SHOWHGT * TILEHGT) / 2);

    gfx_obj.drawRectangle(color_darkblue, topLeft, boxSize, true);
    printMonitorWindow(shipBorderColor);

    const std::string name = s->getShipName();
    const std::string hull = "Hull: " + int2String(s->getHullStatus()) + "/" + int2String(s->getMaxHull());
    std::string crew = "Crew: ";
    const std::string evasion = "Evasion: " + double2String(s->getEvasion());
    const std::string accuracy = "Accuracy: " + double2String(s->getAccuracy());
    const std::string speed = "Speed: " + double2String(s->getSpeed());
    const std::string shields = "Shields: " + int2String(s->getTotalMTFillRemaining(MODULE_SHIELD)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_SHIELD));

    crew += s->crewOperable()
        ? int2String(s->getTotalMTFillRemaining(MODULE_CREW)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_CREW))
        : "AUTOMATED";

    const int nameX = SHOWWID + (GRIDWID - SHOWWID) / 2 - static_cast<int>(name.size()) / 2;
    addString(name, cp_whiteonblack, point(nameX, 0));

    const point innerBoxTL((SHOWWID + 2) * TILEWID, TILEHGT - 1);
    const point innerBoxBR(SCREENWID - ((SHOWWID + 2) * TILEWID) - TILEWID + 1, (SHOWHGT * TILEHGT) / 2);
    gfx_obj.drawRectangle(shipBorderColor, innerBoxTL, innerBoxBR, false);

    Module* currentModule = getCurrentMobSelectedModule(s);
    // always a weapon for now
    std::string moduleName = currentModule->getWeaponStruct().name_modifier;
    color_pair moduleColor = currentModule->getWeaponStruct().disp_chtype.color;

    addString(hull + "                      ", cp_grayonblack, point(SHOWWID + 3, 1));
    addString(crew + "                      ", cp_lightgrayonblack, point(SHOWWID + 3, 2));
    addString(shields + "                      ", cp_lightblueonblack, point(SHOWWID + 3, 3));
    addString(speed + "       ", cp_purpleonblack, point(SHOWWID + 23, 1));
    addString(accuracy + "       ", cp_redonblack, point(SHOWWID + 23, 2));
    addString(evasion + "       ", cp_darkredonblack, point(SHOWWID + 23, 3));
    addString(capitalizeFirstLetter(moduleName), moduleColor, point(SHOWWID + 3, 4));

    displayNPCShipGraphic(s);

    // CAG generation
    //const point cagLabelPos(SHOWWID + 3, 4);
    //const point cagValuePos(SHOWWID + 7, 4);
    //addString("CAG:    ", cp_whiteonblack, cagLabelPos);
    //addString(int2String(s->getDesignStruct().CAShipGenerations), cp_whiteonblack, cagValuePos);
}

void display::printShipStatsSection(MobShip* s)
{
    const std::string hull = "Hull: " + int2String(std::max(0, s->getHullStatus())) + "/" + int2String(s->getMaxHull());
    const std::string crew = "Crew: " + int2String(s->getTotalMTFillRemaining(MODULE_CREW)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_CREW));
    const std::string fuel = "Fuel: " + int2String(s->getTotalMTFillRemaining(MODULE_FUEL)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_FUEL));
    const std::string credits = "Credits: " + uint642String(s->getNumCredits());
    const std::string speed = "Speed: " + double2String(s->getSpeed());
    const std::string acc = "Accuracy: " + double2String(s->getAccuracy());
    const std::string evasion = "Evasion: " + double2String(s->getEvasion());
    const std::string shields = "Shields: " + int2String(s->getTotalMTFillRemaining(MODULE_SHIELD)) + "/" + int2String(s->getTotalMTFillCapacity(MODULE_SHIELD));

    const int baseY = SHOWHGT / 2 + 2;
    addString(hull + "                      ", cp_grayonblack, point(SHOWWID + 3, baseY));
    addString(crew + "                      ", cp_lightgrayonblack, point(SHOWWID + 3, baseY + 1));
    addString(shields + "                      ", cp_lightblueonblack, point(SHOWWID + 3, baseY + 2));
    addString(credits + "                  ", cp_whiteonblack, point(SHOWWID + 3, baseY + 3));

    addString(speed + "       ", cp_purpleonblack, point(SHOWWID + 23, baseY));
    addString(acc + "       ", cp_redonblack, point(SHOWWID + 23, baseY + 1));
    addString(evasion + "       ", cp_darkredonblack, point(SHOWWID + 23, baseY + 2));
    addString(fuel + "       ", cp_brownonblack, point(SHOWWID + 23, baseY + 3));

    addPlayerShipGraphicDetails(s->getMaxNumModules());
    printShipGraphic(s, SHOWHGT / 2);
}

void display::printWindowBorders(std::string mapName, std::string shipName, tab_type tt)
{
    // Full background
    gfx_obj.drawRectangle(color_darkblue, point(0, 0), point(SCREENWID, (SHOWHGT + 2) * TILEHGT - 1), true);

    // Starmap window
    const point starmapLabelPos(SHOWWID / 2 - static_cast<int>(mapName.size()) / 2, 0);
    const color_pair mapColor = (tt == TABTYPE_PLAYAREA ? cp_blackonwhite : cp_whiteonblack);
    addString(mapName, mapColor, starmapLabelPos);
    gfx_obj.drawRectangle(color_black, point(TILEWID, TILEHGT), point(SHOWWID * TILEWID, SHOWHGT * TILEHGT), true);
    gfx_obj.drawRectangle(color_orange, point(TILEWID - 1, TILEHGT - 1), point(SHOWWID * TILEWID + 2, SHOWHGT * TILEHGT + 2), false);

    // Monitor window
    printMonitorWindow(color_orange);
    addString("<<<OFFLINE>>>", cp_greenonblack, point(SHOWWID + (GRIDWID - SHOWWID - 10) / 2, SHOWHGT / 2 - 9));

    // Ship stats window
    const color_pair shipColor = (tt == TABTYPE_PLAYERSHIP ? cp_blackonwhite : cp_whiteonblack);
    addString(shipName, shipColor, point(SHOWWID + (GRIDWID - SHOWWID - 10) / 2 + 1, SHOWHGT / 2 + 1));

    const point innerTL((SHOWWID + 2) * TILEWID + 1, (SHOWHGT * TILEHGT) / 2 + TILEHGT * 2);
    const point innerBR(SCREENWID - ((SHOWWID + 2) * TILEWID) - TILEWID - 1, (SHOWHGT * TILEHGT) / 2 - TILEHGT);
    gfx_obj.drawRectangle(color_black, innerTL, innerBR, true);

    const point borderTL((SHOWWID + 2) * TILEWID, (SHOWHGT * TILEHGT) / 2 + TILEHGT * 2 - 1);
    const point borderBR(SCREENWID - ((SHOWWID + 2) * TILEWID) - TILEWID + 1, (SHOWHGT * TILEHGT) / 2 - TILEHGT + 2);
    gfx_obj.drawRectangle(color_orange, borderTL, borderBR, false);
}

void display::printMonitorWindow(color_type col)
{
    const point labelPos(SHOWWID + (GRIDWID - SHOWWID - 3) / 2, 0);
    addString("TARGET", cp_whiteonblack, labelPos);

    const point innerTL((SHOWWID + 2) * TILEWID + 1, TILEHGT);
    const point innerBR(SCREENWID - ((SHOWWID + 2) * TILEWID) - TILEWID - 1, (SHOWHGT * TILEHGT) / 2 - 2);
    gfx_obj.drawRectangle(color_black, innerTL, innerBR, true);

    const point borderTL((SHOWWID + 2) * TILEWID, TILEHGT - 1);
    const point borderBR(SCREENWID - ((SHOWWID + 2) * TILEWID) - TILEWID + 1, (SHOWHGT * TILEHGT) / 2);
    gfx_obj.drawRectangle(col, borderTL, borderBR, false);
}

// print ship window inside
void display::printShipGraphic(MobShip *s, int globalYOffset)
{
    int ship_body_size = s->getMaxNumModules();
    int num_ship_modules = s->getNumInstalledModules();

    for (int i = 0; i < num_ship_modules; ++i)
    {
        addModuleGraphic(s,i,ship_body_size,globalYOffset);
    }
}

// helper...
void display::addPlayerShipGraphicDetails(int ship_body_size)
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
        addChar(player_ship.isActivated() ? shipFlames[0] : blank_ch, point(SHOWWID + 34 - ship_body_size, SHOWHGT / 2 + 9 + i));
    for (int i = 0; i < 7; ++i)
        addChar(player_ship.isActivated() ? shipFlames[0] : blank_ch,point(SHOWWID+33-ship_body_size,SHOWHGT/2+10+i));
    for (int i = 0; i < 5; ++i)
        addChar(player_ship.isActivated() ? shipFlames[0] : blank_ch,point(SHOWWID+32-ship_body_size,SHOWHGT/2+11+i));
    for (int i = 0; i < 3; ++i)
        addChar(player_ship.isActivated() ? shipFlames[0] : blank_ch,point(SHOWWID+31-ship_body_size,SHOWHGT/2+12+i));
    addChar(gray_rightarrow,point(SHOWWID+41,SHOWHGT/2+12));
    addChar(gray_rightarrow,point(SHOWWID+41,SHOWHGT/2+13));
    addChar(gray_rightarrow,point(SHOWWID+41,SHOWHGT/2+14));
}

// print module graphic bar
void display::addModuleGraphic(MobShip *s, int i, int ship_body_size,int y_global_offset)
{ 
   point arrow_loc;

    switch(s->getModule(i)->getModuleType())
    {
        case(MODULE_FUEL):
        {
            gfx_obj.addBitmapVerticalString("       ",cp_lightgrayonblack,point(SHOWWID+36-ship_body_size+i+1,y_global_offset+10));
            gfx_obj.addBitmapCharacter(fueltank_symbol, point(SHOWWID + 36 - ship_body_size + i + 1, y_global_offset + 10));
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
        {
            break;
        }
    }

    arrow_loc = point(SHOWWID+36-ship_body_size+i+1,y_global_offset+17);

    if (i == s->getModuleSelectionIndex())
    {
        addChar(up_arrow_selector, arrow_loc);
    }
}

// draw the exhaustion/fill bar for a module over the ship
void display::printMeter(Module* m,
    int shipBodySize,
    int idx,
    int fillWidth,
    module_type type,
    color_type fillColor,
    int yOffset)
{
    const int qty = m->getFillQuantity();
    const int maxQty = m->getMaxFillQuantity();
    const int fullBarH = 6 * TILEHGT;
    const int filledHeight = qty * fullBarH / maxQty;

    const point barPos(
        SHOWWID + 36 - shipBodySize + idx + 1,
        yOffset + 10
    );

    const int tileX = barPos.x() * TILEWID;
    const int tileY = (barPos.y() + 1) * TILEHGT + (fullBarH - filledHeight);

    if (fillWidth > 1)
    {
        gfx_obj.drawRectangle(fillColor,
            point(tileX, tileY), point(fillWidth, filledHeight), true);
    }
    else
    {
        gfx_obj.drawRectangle(fillColor,
            point(tileX + TILEWID - 1, tileY), point(fillWidth, filledHeight), true);
    }
}

void display::printCrewMeter(Module* m,
    int shipBodySize,
    int idx,
    int yOffset)
{
    // How many crew dots to draw
    const int quantity = m->getFillQuantity();
    const int fullBarH = 6 * TILEHGT;
    // How wide before wrapping to next row
    const int dotRowWidth = (TILEWID % 2 == 0 ? TILEWID : TILEWID - 1);

    // Base grid cell for this meter
    const point origin(
        SHOWWID + 36 - shipBodySize + idx + 1,
        yOffset + 10
    );

    // Draw the little label vertically
    gfx_obj.addBitmapVerticalString(
        "c      ",
        cp_lightgrayonblack,
        origin
    );

    // Draw green ticks for each crew member
    for (int n = 0; n < quantity; ++n)
    {
        int dx = (2 * n) % dotRowWidth;
        int row = n / (dotRowWidth / 2);
        int dy = fullBarH - (row * 2) - 1;

        gfx_obj.drawRectangle(
            color_green,
            point(origin.x() * TILEWID + dx,
                (origin.y() + 1) * TILEHGT + dy),
            point(1, 1),
            true
        );
    }

    // Draw up to five crew pod symbols based on the modules capacity
    const int maxCrew = m->getMaxFillQuantity();
    for (int slot = 5; slot >= 1; --slot)
    {
        if (maxCrew <= slot * 16)
        {
            gfx_obj.addBitmapCharacter(
                crewpod_display_symbol,
                point(origin.x(), origin.y() + 6 - slot)
            );
        }
        else
        {
            break;
        }
    }
}

// draw the vertical graphic for weapons, shields, or engines
void display::printWeaponShieldEngineGraphic(Module* m,
    int shipBodySize,
    int idx,
    int yOffset)
{
    const point basePos(
        SHOWWID + 36 - shipBodySize + idx + 1,
        yOffset + 10
    );

    chtype ct = {};
    chtype firstChar = {};

    switch (m->getModuleType())
    {
    case MODULE_WEAPON:
        ct = firstChar = m->getWeaponStruct().disp_chtype;
        firstChar.ascii = std::toupper(m->getWeaponStruct().name_modifier[0]);
        break;

    case MODULE_SHIELD:
        ct = firstChar = m->getShieldStruct().disp_chtype;
        firstChar.ascii = std::toupper(m->getShieldStruct().name_modifier[0]);
        break;

    case MODULE_ENGINE:
        ct = firstChar = m->getEngineStruct().disp_chtype;
        firstChar.ascii = std::toupper(m->getEngineStruct().name_modifier[0]);
        break;

    default:
        // no graphic on unknown module
        return;
    }

    // draw 6 vertical background characters
    for (int row = 0; row < 6; ++row)
    {
        addChar(ct, point(basePos.x(), basePos.y() + 1 + row));
    }

    // draw the module letter at the top
    addChar(firstChar, basePos);

    // additional indicators based on module specifics
    if (m->getModuleType() == MODULE_WEAPON &&
        m->getWeaponStruct().num_shots > 1)
    {
        gfx_obj.addBitmapVerticalString(
            int2String(m->getWeaponStruct().num_shots),
            cp_whiteonblack,
            point(basePos.x(), basePos.y() + 6)
        );
    }
    else if (m->getModuleType() == MODULE_SHIELD)
    {
        gfx_obj.addBitmapVerticalString(
            int2String(m->getMaxFillQuantity()),
            cp_whiteonblack,
            point(basePos.x(), basePos.y() + 1)
        );
    }
}

void display::delayAndUpdate(int ms)
{
    gfx_obj.updateScreen();
    SDL_Delay(ms);
    event_handler.flushInput();
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
  std::string().swap(message_string);
  for (int i = 0; i < NUMMESSAGELINES * GRIDWID; ++i)
  {
      message_color_data[i] = cp_blackonblack;
  }
}

int msgbuffer::getMessageSize()
{
    return message_string.size();
}

unsigned char msgbuffer::getStringCharacter(int i)
{
    return (unsigned char)message_string[i];
}

color_pair msgbuffer::getMessageColorData(int i)
{
    return message_color_data[i];
}

color_type getFuelMeterColor(Module *m)
{
    if (m->getMaxFillQuantity() >= 1024)
        return color_white;
    return getAnyColor(255,(Uint8)(m->getMaxFillQuantity()/16),(Uint8)(m->getMaxFillQuantity()/4));
}

void msgeAdd(std::string msg, color_pair col_p)
{
    bool reached_buffer_limit = false;
    // adding a message returns a boolean that says whether
    // or not the message buffer is full after adding last
    // message
    if (Game_active)
    {
        reached_buffer_limit = display_obj.addMessage(msg, col_p);
    }

    display_obj.printMessages();
    // clear buffer if full
    if (reached_buffer_limit)
    {
        reDisplay(false);
        event_handler.waitForKey(' ');
        display_obj.clearAndDeleteAllMessages();
    }
}

void reDisplay(bool calculateLOS)
{
    // print map
    if (wait_counter == 0)
    {
        reDisplayWithoutUpdate(calculateLOS);
    }

    gfx_obj.updateScreen();
}

void reDisplayWithoutUpdate(bool calculateLOS)
{
    gfx_obj.clearScreen();

    if (calculateLOS)
    {
        calculatePlayerLOS();
    }

    printWindowBorders();
    display_obj.clearRange(point(1, 1), point(SHOWWID, SHOWHGT));
    display_obj.printMap(getMap());
    printMobCells();
    display_obj.printShipStatsSection(getPlayerShip());
    display_obj.printMessages();
}

void printWindowBorders()
{
    std::string mapName = "STAR MAP";

    if (current_maptype == MAPTYPE_LOCALEMAP)
    {
        SubAreaRegion * region = currentRegion();
        if (universe.getSubAreaMapType() == SMT_PERSISTENT && region->isRaceAffiliated())
        {
            race* nativeRace = universe.getRace(region->getNativeRaceID());
            mapName = region->getSubAreaName() + " " + race_domain_suffix_string[(int)nativeRace->getRaceDomainType()];
        }
        else
        {
            mapName = region->getSubAreaName();
        }
    }

    display_obj.printWindowBorders(mapName, getPlayerShip()->getShipName(), current_tab);
}

void printMobCells()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    for (int i = 0; i < currentRegion()->getNumShipNPCs(); ++i)
    {
        MobShip* ship = currentRegion()->getNPCShip(i);
        point mobLoc = ship->at();

        if (!getMap()->getv(mobLoc) || getMap()->getFire(mobLoc) != NIL_f || getMap()->getMob(mobLoc) == NIL_m)
            continue;

        chtype symbol = ship->getShipSymbol();

        if (!ship->isActivated())
        {
            symbol.color.bg = symbol.color.fg;
            symbol.color.fg = color_black;
        }

        display_obj.printCell(getMap(), mobLoc, symbol);
    }
}

void outputLOFTransition(point lof, point source, point dest, fire_t fireType, bool displayFullLine, int delayValue)
{
    if (!displayFullLine)
        clearAllFireCellsInRange(getMap(), lof, 1);

    fire_t selectedFire = selectFireCell(source, dest, fireType);
    printAndSetFireCell(getMap(), lof, selectedFire);

    display_obj.delayAndUpdate(delayValue);
}

void clearAllFireCellsInRange(map* m, point center, int radius)
{
    const point mapSize = m->getSize();

    for (int y = center.y() - radius; y <= center.y() + radius; ++y)
    {
        for (int x = center.x() - radius; x <= center.x() + radius; ++x)
        {
            point target(x, y);
            if (inMapRange(target, mapSize))
            {
                printAndSetFireCell(m, target, NIL_f);
            }
        }
    }
}

void clearAllFireCells(map* m)
{
    const point size = m->getSize();

    for (int y = 0; y < size.y(); ++y)
    {
        for (int x = 0; x < size.x(); ++x)
        {
            m->setFire(point(x, y), NIL_f);
        }
    }

    display_obj.printMap(m);
    printMobCells();
}

void printAndSetFireCell(map* m, point p, fire_t f)
{
    if (inMapRange(p, m->getSize()))
    {
        m->setFire(p, f);
        updateAllLastSymbols(m->getCellP(p));
        display_obj.printCell(m, p, display_obj.getSymbol(m, p));
        printMobCells();
    }
}

void addHitSprite(map* m, point p, bool fullLineShot)
{
    printAndSetFireCell(m, p, FIRET_EXPLOSION);
    display_obj.delayAndUpdate(15);
    if (!fullLineShot)
       clearAllFireCellsInRange(m, p, 1);
}

void createDamagingExplosionAnimation(point p, int radius, int msUpdate, fire_t expSprite, bool printMessage)
{
    if (printMessage)
        msgeAdd("KABOOOOOOOOOOOOM!!!", fire_symbol[(int)expSprite].color);

    if (!getMap()->getv(p))
        return;

    printFireCircle(p, radius, expSprite);

    display_obj.delayAndUpdate(msUpdate);

    clearAllFireCells(getMap());
}

void printFireCircle(point center, int radius, fire_t fireType)
{
    for (int dy = -radius; dy <= radius; ++dy)
    {
        int dxRange = static_cast<int>(std::sqrt(radius * radius - dy * dy));

        for (int dx = -dxRange; dx <= dxRange; ++dx)
        {
            point target = addPoints(center, point(dx, dy));

            if (inMapRange(target, getMapSize()))
            {
                printAndSetFireCell(getMap(), target, fireType);
            }
        }
    }
}

void calculatePlayerLOS()
{
    const point origin = getPlayerShip()->at();
    const int detectRadius = (current_maptype == MAPTYPE_LOCALEMAP)
                             ? player_ship.getDetectRadius()
                             : player_ship.getDetectRadius() / 2;

    getMap()->setAllCellsUnvisible();

    for (int dy = -detectRadius; dy <= detectRadius; ++dy)
    {
        const int xRange = static_cast<int>(std::sqrt(detectRadius * detectRadius - dy * dy));

        for (int dx = -xRange; dx <= xRange; ++dx)
        {
            const point target = addPoints(origin, point(dx, dy));

            if (!inMapRange(target, getMapSize()))
                continue;

            if (!tracer.isBlocking(getMap(), origin, target, false, false))
            {
                cell* cellPtr = getMap()->getCellP(target);
                getMap()->setv(target, true);
                getMap()->setm(target, true);
                updateAllLastSymbols(cellPtr);
            }
        }
    }
}

void changeMobTile(point from, point to, mob_t whom)
{
    // set null to the mob tile at old location
    setMobTileToNIL(getMap(), from);
    // put mob tile at new location
    setMobTile(getMap(), to, whom);
    // print cells to avoid characters appearing twice
    display_obj.printCell(getMap(), from, display_obj.getSymbol(getMap(), from));
    display_obj.printCell(getMap(), to, display_obj.getSymbol(getMap(), to));
    printMobCells();
}

void setMobTileToNIL(map * m, point p)
{
    m->setMob(p, NIL_m);
    m->getCellP(p)->setLastMobSymbol(m->getCellP(p)->getCurrentMobSymbol());
}

void setMobTile(map * m, point p, mob_t mt)
{
    m->setMob(p, mt);
    m->getCellP(p)->setLastMobSymbol(m->getCellP(p)->getCurrentMobSymbol());
}

void updateAllLastSymbols(cell * c)
{
    c->setLastFireSymbol(c->getCurrentFireSymbol());
    c->setLastMobSymbol(c->getCurrentMobSymbol());
    c->setLastItemSymbol(c->getCurrentItemSymbol());
    c->setLastBackdropSymbol(c->getCurrentBackdropSymbol());
}

bool chtypeEqual(chtype a, chtype b)
{
    return a.color.fg.r == b.color.fg.r &&
           a.color.fg.g == b.color.fg.g &&
           a.color.fg.b == b.color.fg.b &&
           a.color.bg.r == b.color.bg.r &&
           a.color.bg.g == b.color.bg.g &&
           a.color.bg.b == b.color.bg.b &&
           a.ascii == b.ascii;
}
