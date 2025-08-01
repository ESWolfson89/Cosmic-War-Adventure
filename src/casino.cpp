#include "casino.h"

machine::machine()
{
}

uint_64 machine::getMachineCredits()
{
    return 1;
}

point machine::at()
{
    return loc;
}

uint_64 machine::getCredits()
{
    return credits;
}

void machine::setCredits(uint_64 c)
{
    credits = c;
}

void machine::delayFunc(Uint32 t)
{
    gfx_obj.updateScreen();
    SDL_Delay(t);
}

void machine::updateFunc()
{
    gfx_obj.updateScreen();
}

void machine::initMachineColorAttributes()
{
    machine_color.fg = getPresetColor(randIntZ(11));

    arrow_color.fg = getPresetColor(randIntZ(11));

    title_color.fg = getPresetColor(randIntZ(11));

    number_color.fg = getPresetColor(randIntZ(11));

    text_color.fg = getPresetColor(randIntZ(11));

    machine_color.bg = title_color.bg = text_color.bg = arrow_color.bg = number_color.bg = color_black;
}

void machine::gotoNextState(point delta)
{
    if (machine_type == MACHINE_SLOT)
    {
        if (delta.x() == 1 && delta.y() == 0)
        {
            current_state = (machine_state)(((int)current_state + 4) % 8);
        }
        else if (delta.x() == -1 && delta.y() == 0)
        {
            current_state = (machine_state)(((int)current_state - 4) + ((int)current_state > 3 ? 0 : 8));
        }
        else if (delta.x() == 0 && delta.y() == -1)
        {
            current_state = (machine_state)(((int)current_state - 1) + ((int)current_state > 0 ? 0 : 8));
        }
        else if (delta.x() == 0 && delta.y() == 1)
        {
            current_state = (machine_state)(((int)current_state + 1) + ((int)current_state < 7 ? 0 : -8));
        }
    }

    if (machine_type == MACHINE_DIAMOND)
    {
        if (delta.x() == 1 && delta.y() == 0)
        {
            current_state = (machine_state)(((int)current_state + 3) + ((int)current_state < 11 ? 0 : -6));
        }
        else if (delta.x() == -1 && delta.y() == 0)
        {
            current_state = (machine_state)(((int)current_state - 3) + ((int)current_state > 10 ? 0 : 6));
        }
        else if (delta.x() == 0 && delta.y() == -1)
        {
            current_state = (machine_state)(((int)current_state - 1) + ((int)current_state > 8 ? 0 : 6));
        }
        else if (delta.x() == 0 && delta.y() == 1)
        {
            current_state = (machine_state)(((int)current_state + 1) + ((int)current_state < 13 ? 0 : -6));
        }
    }
}

void machine::initState()
{
    if (machine_type == MACHINE_SLOT)
    {
        current_state = (machine_state)MSTATE_SLOTSPINMAX;
    }
    if (machine_type == MACHINE_DIAMOND)
    {
        current_state = (machine_state)MSTATE_DIAMONDPLAY;
    }
}

void machine::setState(machine_state ms)
{
    current_state = ms;
}

MachineType machine::getMachineType()
{
    return machine_type;
}

machine_state machine::getState()
{
    return current_state;
}

color_pair machine::getMachineColor()
{
    return machine_color;
}

void machine::save(std::ofstream& os) const
{
    os.write(reinterpret_cast<const char*>(&credits), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&rig_factor), sizeof(int));
    os.write(reinterpret_cast<const char*>(&danger_level), sizeof(int));
    colorPairSave(os, machine_color);
    colorPairSave(os, title_color);
    colorPairSave(os, text_color);
    colorPairSave(os, arrow_color);
    colorPairSave(os, number_color);
    os.write(reinterpret_cast<const char*>(&machine_type), sizeof(MachineType));
    os.write(reinterpret_cast<const char*>(&current_state), sizeof(machine_state));
    loc.save(os);
}

void machine::load(std::ifstream& is)
{
    is.read(reinterpret_cast<char*>(&credits), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&rig_factor), sizeof(int));
    is.read(reinterpret_cast<char*>(&danger_level), sizeof(int));
    colorPairLoad(is, machine_color);
    colorPairLoad(is, title_color);
    colorPairLoad(is, text_color);
    colorPairLoad(is, arrow_color);
    colorPairLoad(is, number_color);
    is.read(reinterpret_cast<char*>(&machine_type), sizeof(MachineType));
    is.read(reinterpret_cast<char*>(&current_state), sizeof(machine_state));
    loc.load(is);
}

slot::slot()
{

}

slot::slot(point p, int dl, uint_64 c)
{
    credits = c;
    danger_level = dl;
    loc = p;
    machine_type = MACHINE_SLOT;
}

void slot::drawMachine()
{
    drawSlotFrame();
    drawSlotTileKey();
    drawSlotReels();
    drawSlotText();
    drawAllArrows(true);
}

void slot::drawSlotStateIndicator(std::string txt, machine_state s)
{
    color_pair select_color = {text_color.fg,color_verydarkgray};

    point loc = point(2+(((int)((int)s/4))*11)+MACHINEX_OFFSET,16+(int)(s % 4) + MACHINEY_OFFSET);

    if (s != current_state)
    {
        gfx_obj.addBitmapString(txt, text_color, loc);
    }
    else
    {
        gfx_obj.addBitmapString(txt, select_color, loc);
    }
}

void slot::drawSlotText()
{
    gfx_obj.addBitmapString("SLOTS",title_color, point(10+MACHINEX_OFFSET,1 + MACHINEY_OFFSET));

    gfx_obj.addBitmapString("T      ",text_color,point(16 + MACHINEX_OFFSET,10 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("L      ",text_color,point(16 + MACHINEX_OFFSET,11 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("B      ",text_color,point(16 + MACHINEX_OFFSET,12 + MACHINEY_OFFSET));

    drawSlotStateIndicator(" MAX SPIN ",MSTATE_SLOTSPINMAX);
    drawSlotStateIndicator("   SPIN   ",MSTATE_SLOTSPIN);
    drawSlotStateIndicator("  INSERT  ",MSTATE_SLOTINSERT);
    drawSlotStateIndicator(" CASH OUT ",MSTATE_SLOTCASHOUT);
    drawSlotStateIndicator(" SPIN ONE ",MSTATE_SLOTSPINONE);
    drawSlotStateIndicator(" NEXT BET ",MSTATE_SLOTBETAMOUNT);
    drawSlotStateIndicator("NEXT LINES",MSTATE_SLOTBETLINES);
    drawSlotStateIndicator("LEAVE GAME",MSTATE_SLOTLEAVE);

    gfx_obj.addBitmapString("CREDIT=", text_color, point(2 + MACHINEX_OFFSET, 14 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("              ", number_color, point(9 + MACHINEX_OFFSET, 14 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(uint642String(credits), number_color, point( 9+ MACHINEX_OFFSET, 14 + MACHINEY_OFFSET));

    for (uint i = 0; i < 3; ++i)
    {
        gfx_obj.addBitmapCharacter({text_color, 26}, point(17 + MACHINEX_OFFSET, 10 + i + MACHINEY_OFFSET));
    }

    gfx_obj.addBitmapString(int2String(current_bet), number_color, point(18 + MACHINEX_OFFSET,12 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(int2String(current_lines), number_color, point(18 + MACHINEX_OFFSET,11 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(int2String(current_total_bet), number_color, point(18 + MACHINEX_OFFSET,10 + MACHINEY_OFFSET));

    color_pair dollar_multiplier_indicator_color = {color_black,machine_color.fg};

    gfx_obj.addBitmapString("x", dollar_multiplier_indicator_color, point(11 + MACHINEX_OFFSET,9 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(int2String(dollar_match_multiplier), dollar_multiplier_indicator_color, point(13 + MACHINEX_OFFSET,9 + MACHINEY_OFFSET));

    std::string last_won_indicator1 = uint642String(last_winnings);
    std::string last_won_indicator2 = "";

    if (last_won_indicator1.size() > 7)
    {
        last_won_indicator2 = std::string(last_won_indicator1.begin()+7,last_won_indicator1.end());
        last_won_indicator1 = std::string(last_won_indicator1.begin(),last_won_indicator1.begin()+7);
    }

    if (last_winnings == 0ULL)
    {
        gfx_obj.addBitmapString("SPIN   ", text_color, point(2 + MACHINEX_OFFSET, 10 + MACHINEY_OFFSET));
        gfx_obj.addBitmapString("   2   ", text_color, point(2 + MACHINEX_OFFSET, 11 + MACHINEY_OFFSET));
        gfx_obj.addBitmapString("   WIN!", text_color, point(2 + MACHINEX_OFFSET, 12 + MACHINEY_OFFSET));
    }
    else
    {
        if (last_line_win_tile <= 10)
        {
            gfx_obj.addBitmapString("YOU WIN", text_color, point(2 + MACHINEX_OFFSET, 10 + MACHINEY_OFFSET));
        }
        if (last_line_win_tile == 11)
        {
            gfx_obj.addBitmapString("JACKPOT", text_color, point(2 + MACHINEX_OFFSET, 10 + MACHINEY_OFFSET));
        }

        gfx_obj.addBitmapString("       ", number_color, point(2 + MACHINEX_OFFSET, 11 + MACHINEY_OFFSET));
        gfx_obj.addBitmapString("       ", number_color, point(2 + MACHINEX_OFFSET, 12 + MACHINEY_OFFSET));

        gfx_obj.addBitmapString(last_won_indicator1, number_color, point(2 + MACHINEX_OFFSET, 11 + MACHINEY_OFFSET));
        gfx_obj.addBitmapString(last_won_indicator2, number_color, point(2 + MACHINEX_OFFSET, 12 + MACHINEY_OFFSET));
    }
}

void slot::drawSlotReels()
{
    slot_tile t;

    point p;

    for (uint i = 0; i < NUM_SLOT_REELS; ++i)
    {
        for (uint j = 0; j < NUM_VISIBLE_LINES; ++j)
        {
            t = reel[reel_state[i][j]];
            p = point((i + 1) * 2 - 1, j);
            drawSlotTile(point(p.x() + MACHINEX_OFFSET + 9, p.y() + 10 + MACHINEY_OFFSET), t);
        }
    }

    // draw the dollar sign additionally
    gfx_obj.addBitmapCharacter({ dollar_colors[current_dollar_color],'$' }, point(12 + MACHINEX_OFFSET, 8 + MACHINEY_OFFSET));
}

void slot::drawArrow(uint n, bool lit)
{
    color_pair colp = (lit == true ? arrow_color : cp_darkergrayonblack);
    gfx_obj.addBitmapCharacter({colp, 31}, point(10 + MACHINEX_OFFSET + (2*n), 9 + MACHINEY_OFFSET));
    gfx_obj.addBitmapCharacter({colp, 30}, point(10 + MACHINEX_OFFSET + (2*n), 13 + MACHINEY_OFFSET));
}

void slot::flickerDollarSignColor()
{
    current_dollar_color = (current_dollar_color + 1) % 10;
}

void slot::drawSlotTile(point p, slot_tile t)
{
    chtype ct = slot_tile_symbol[(int)t];
    gfx_obj.addBitmapCharacter(slot_tile_symbol[(int)t], p);
}

void slot::drawSlotFrame()
{
    int current_ascii;

    // print machine border one block at a time
    for (int x = 0; x < MACHINEWID; ++x)
    {
        for (int y = 0; y < MACHINEHGT; ++y)
        {
            current_ascii = slot_backdrop_array[y][x];

            gfx_obj.addBitmapCharacter({ machine_color, current_ascii }, point(x + MACHINEX_OFFSET, y + MACHINEY_OFFSET));
        }
    }
}

void slot::drawAllArrows(bool lit)
{
    for (uint i = 0; i < 3; ++i)
    {
        drawArrow(i, lit);
    }
}

void slot::drawSlotTileKey()
{
    point p;

    uint ith_payout;

    std::string payout_string;

    std::string payout_string_modifier;

    for (uint i = 11; i >= 2; --i)
    {
        for (uint j = 1; j < 4; ++j)
        {
            p.set(j+1+MACHINEX_OFFSET,14-i+MACHINEY_OFFSET);

            if (i <= 5)
            {
                p = addPoints(p, point(11, -6));
            }

            drawSlotTile(p,(slot_tile)i);
        }

        p.setx(p.x() + 1);

        payout_string = "x";

        payout_string_modifier = "";

        ith_payout = line_payouts[i-2];

        if (ith_payout % 1000 == 0)
        {
            payout_string_modifier = "K";
            ith_payout /= 1000;
        }

        payout_string += int2String((uint_64)ith_payout);
        payout_string += payout_string_modifier;

        gfx_obj.addBitmapString(payout_string, text_color, p);
    }

    p.set(14+MACHINEX_OFFSET,7+MACHINEY_OFFSET);

    for (uint i = 0; i < 2; ++i)
    {
        drawSlotTile(point(p.x(),p.y() + i),(slot_tile)(11-i));

        payout_string = "x" + int2String((uint_64)tile_multipliers[1-i]) + " each";

        gfx_obj.addBitmapString(payout_string, text_color, point(p.x() + 1, p.y() + i));
    }
}

void slot::initSlotDisplayAttributes()
{
    chtype ct;

    std::vector<uint> pcolor_list;

    uint index = 0;

    for (uint i = 0; i <= 11; ++i)
    {
        pcolor_list.push_back(i);
    }

    for (uint i = 0; i < NUM_TOTAL_SLOT_TILES; ++i)
    {
        index = randIntZ(pcolor_list.size()-1);
        ct.ascii = getSlotAscii(i);
        ct.color.fg = getPresetColor(pcolor_list[index]);
        ct.color.bg = color_black;
        slot_tile_symbol[i] = ct;
        pcolor_list.erase(pcolor_list.begin() + index);
    }

    initMachineColorAttributes();

    for (uint i = 0; i < 10; ++i)
    {
        dollar_colors[i] = slot_tile_symbol[i + 2].color;
    }
}

void slot::initSlots()
{
    uint tval = 1;

    const uint num_slot_reel_tiles = BASE_NUM_SLOT_REEL_TILES +
                                     (NUM_SLOT_REEL_TILES_OFFSET *
                                      (danger_level - 1));

    initSlotDisplayAttributes();

    current_dollar_color = randIntZ(9);

    for (uint i = 1; i <= num_slot_reel_tiles; ++i)
    {
        reel.push_back((slot_tile)(12 - tval));
        if (i == (uint)(((double)tval * ((double)tval + 1.0) / 2.0)) && i <= BASE_NUM_SLOT_REEL_TILES)
        {
            tval++;
        }
    }

    for (uint i = 0; i < 3; ++i)
    {
        reel_state[i][0] = randIntZ(num_slot_reel_tiles - 1);
        reel_state[i][1] = (reel_state[i][0] + 1) % num_slot_reel_tiles;
        reel_state[i][2] = (reel_state[i][1] + 1) % num_slot_reel_tiles;
    }

    std::shuffle(reel.begin(),reel.end(),random_number_generator);
    std::shuffle(reel.begin(),reel.end(),random_number_generator);

    uint multiplier = 1;

    if (danger_level > 1)
    {
        multiplier = randIntZ(slot_base_danger_multiplier[danger_level - 1] -
            slot_base_danger_multiplier[danger_level - 2] - 1) +
            slot_base_danger_multiplier[danger_level - 2] + 1;
    }

    for (uint i = 0; i < 10; ++i)
    {
        line_payouts[i] = slot_base_payout[i] * multiplier;
    }

    multiplier = (uint)(multiplier / 10) + 1;

    for (uint i = 0; i < 5; ++i)
    {
        bet_values[i] = slot_base_bet[i] * multiplier;
    }

    setBet(randIntZ(4),randIntZ(4));

    initState();

    dollar_match_multiplier = randIntZ(base_dollar_match_multiplier[danger_level - 1]) + 2;

    multiplier = randIntZ((int)(danger_level/6));

    tile_multipliers[0] = 2 + multiplier;
    tile_multipliers[1] = 3 + multiplier;

    last_winnings = 0ULL;

    last_line_win_tile = -1;
}

void slot::setBet(uint b, uint l)
{
    bet_iter = b <= 4 ? b : 0;
    lines_iter = l <= 4 ? l : 0;

    current_bet = bet_values[bet_iter];
    current_lines = slot_base_lines[lines_iter];

    current_total_bet = current_bet * current_lines;
}

uint_64 slot::useMachine(uint_64 inp_credits)
{
    uint_64 out_cred = 0;

    switch(current_state)
    {
        case(MSTATE_SLOTSPIN):
        {
            spinReel();
            break;
        }
        case(MSTATE_SLOTSPINMAX):
        {
            setBet(4, 4);
            drawSlotText();
            spinReel();
            break;
        }
        case(MSTATE_SLOTSPINONE):
        {
            setBet(0, 0);
            drawSlotText();
            spinReel();
            break;
        }
        case(MSTATE_SLOTBETAMOUNT):
        {
            setBet(bet_iter + 1, lines_iter);
            drawSlotText();
            break;
        }
        case(MSTATE_SLOTBETLINES):
        {
            setBet(bet_iter, lines_iter + 1);
            drawSlotText();
            break;
        }
        case(MSTATE_SLOTCASHOUT):
        {
            out_cred = credits;
            credits = 0;
            break;
        }
        case(MSTATE_SLOTINSERT):
        {
            credits += inp_credits;
            break;
        }
        default:
        {
            break;
        }
    }

    return out_cred;
}

void slot::spinReel()
{
    if (current_total_bet > credits)
    {
        return;
    }

    credits -= current_total_bet;

    drawAllArrows(false);
    drawSlotText();
    // reel_spin_factor determines how many units will
    // each reel will turn in succession
    // (1st randInt is first reel, 2nd randInt
    // is second reel, 3rd randInt is third reel)
    int reel_spin_factor[3] = {(int)randIntZ(55)+15,
                               (int)randIntZ(55)+15,
                               (int)randIntZ(55)+15
                              };

    // n indicates which reel is being spun
    uint n = 0;
    // indicates speed of reel (turn/millisec)
    // for each reel spinning...

    int new_state;

    while(n <= 2)
    {
        // do for reel_speed milliseconds...
        // ONLY IF player is using
        delayFunc(5);

        // ...flicker dollar sign,
        flickerDollarSignColor();

        // (do col*row times)
        for (uint i = n; i < NUM_SLOT_REELS; ++i)
        {
            for (uint j = 0; j < NUM_VISIBLE_LINES; ++j)
            {
                new_state = reel_state[i][j];

                if (new_state == 0)
                {
                    new_state = reel.size() - 1;
                }
                else
                {
                    new_state--;
                }

                reel_state[i][j] = new_state;
            }
        }
        // decrement counter (until 0)
        reel_spin_factor[n]--;
        // redisplay reels only
        drawSlotReels();
        // gfx update via sdl function call (from graphics.cpp/h)
        updateFunc();
        // when counter == 0 (one of three counters)
        if (reel_spin_factor[n] == 0)
        {
            drawArrow(n,true);
            n++; // increment reel
        }
    }

    calculateSpinWinnings();
}

void slot::setLineWinnings(slot_tile st1, slot_tile st2, slot_tile st3)
{
    if (st1 == BLANK || st2 == BLANK || st3 == BLANK)
    {
        return;
    }

    if (!(st1 == st2 && st2 == st3))
    {
        return;
    }

    uint temp = line_payouts[(int)st1 - 2];

    if ((uint_64)temp > last_winnings)
    {
        last_line_win_tile = (int)st1;
        last_winnings = temp;
    }
}

uint slot::getTop2MatchMultiplier()
{
    uint ret_val = 1;

    if (last_line_win_tile > -1)
    {
        if ((slot_tile)last_line_win_tile == SS_9 ||
            (slot_tile)last_line_win_tile == SS_10)
        {
            return 0;
        }
    }

    for (uint i = 0; i < 3; ++i)
    for (uint j = 0; j < 3; ++j)
    {
        if (reel[reel_state[i][j]] == SS_9)
        {
            ret_val *= tile_multipliers[0];
        }
        if (reel[reel_state[i][j]] == SS_10)
        {
            ret_val *= tile_multipliers[1];
        }
    }

    return ret_val - 1;
}

uint slot::getDollarMatchMultiplier(slot_tile st)
{
    if (isColor(dollar_colors[current_dollar_color].fg, dollar_colors[(int)st - 2].fg))
    {
        return dollar_match_multiplier;
    }

    return 1;
}

void slot::calculateSpinWinnings()
{
    last_line_win_tile = -1;

    last_winnings = 0ULL;

    uint top2winnings = 0;

    slot_tile t[3][3];

    for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
        t[i][j] = reel[reel_state[i][j]];

    if (current_lines >= 9)
    {
        setLineWinnings(t[0][0],t[1][2],t[2][0]);
        setLineWinnings(t[0][2],t[1][0],t[2][2]);
    }
    if (current_lines >= 7)
    {
        setLineWinnings(t[0][0],t[1][1],t[2][0]);
        setLineWinnings(t[0][2],t[1][1],t[2][2]);
    }
    if (current_lines >= 5)
    {
        setLineWinnings(t[0][2],t[1][1],t[2][0]);
        setLineWinnings(t[0][0],t[1][1],t[2][2]);
    }
    if (current_lines >= 3)
    {
        setLineWinnings(t[0][2],t[1][2],t[2][2]);
        setLineWinnings(t[0][0],t[1][0],t[2][0]);
    }
    if (current_lines >= 1)
    {
        setLineWinnings(t[0][1],t[1][1],t[2][1]);
    }

    if (last_line_win_tile > -1)
        last_winnings *= getDollarMatchMultiplier((slot_tile)last_line_win_tile);

    top2winnings = getTop2MatchMultiplier();

    if (last_winnings == 0ULL)
    {
        if (top2winnings > 0)
        {
            last_winnings = 1 + top2winnings;
        }
    }
    else
    {
        last_winnings *= (1 + top2winnings);
    }

    last_winnings *= current_bet;

    credits += last_winnings;

    drawSlotText();
}

uint slot::getBetVal(uint i)
{
    return bet_values[i];
}

uint slot::getLineVal(uint i)
{
    return slot_base_lines[i];
}

int getSlotAscii(uint i)
{
    return slot_tile_char[i];
}

void slot::save(std::ofstream& os) const
{
    machine::save(os); // call base class member function

    size_t reelSize = reel.size();
    os.write(reinterpret_cast<const char*>(&reelSize), sizeof(size_t));
    os.write(reinterpret_cast<const char*>(reel.data()), reelSize * sizeof(slot_tile));

    os.write(reinterpret_cast<const char*>(reel_state), sizeof(reel_state));
    os.write(reinterpret_cast<const char*>(line_payouts), sizeof(line_payouts));
    os.write(reinterpret_cast<const char*>(bet_values), sizeof(bet_values));
    os.write(reinterpret_cast<const char*>(tile_multipliers), sizeof(tile_multipliers));

    for (int i = 0; i < NUM_TOTAL_SLOT_TILES; ++i)
        chtypeSave(os, slot_tile_symbol[i]);

    for (int i = 0; i < 10; ++i)
        colorPairSave(os, dollar_colors[i]);

    os.write(reinterpret_cast<const char*>(&dollar_match_multiplier), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&current_dollar_color), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&current_bet), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&current_lines), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&current_total_bet), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&last_winnings), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&last_line_win_tile), sizeof(int));
    os.write(reinterpret_cast<const char*>(&bet_iter), sizeof(int));
    os.write(reinterpret_cast<const char*>(&lines_iter), sizeof(int));
}

void slot::load(std::ifstream& is)
{
    machine::load(is); // call base class member function

    size_t reelSize;
    is.read(reinterpret_cast<char*>(&reelSize), sizeof(size_t));
    reel.resize(reelSize);
    is.read(reinterpret_cast<char*>(reel.data()), reelSize * sizeof(slot_tile));

    is.read(reinterpret_cast<char*>(reel_state), sizeof(reel_state));
    is.read(reinterpret_cast<char*>(line_payouts), sizeof(line_payouts));
    is.read(reinterpret_cast<char*>(bet_values), sizeof(bet_values));
    is.read(reinterpret_cast<char*>(tile_multipliers), sizeof(tile_multipliers));

    for (int i = 0; i < NUM_TOTAL_SLOT_TILES; ++i)
        chtypeLoad(is, slot_tile_symbol[i]);

    for (int i = 0; i < 10; ++i)
        colorPairLoad(is, dollar_colors[i]);

    is.read(reinterpret_cast<char*>(&dollar_match_multiplier), sizeof(uint));
    is.read(reinterpret_cast<char*>(&current_dollar_color), sizeof(uint));
    is.read(reinterpret_cast<char*>(&current_bet), sizeof(uint));
    is.read(reinterpret_cast<char*>(&current_lines), sizeof(uint));
    is.read(reinterpret_cast<char*>(&current_total_bet), sizeof(uint));
    is.read(reinterpret_cast<char*>(&last_winnings), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&last_line_win_tile), sizeof(int));
    is.read(reinterpret_cast<char*>(&bet_iter), sizeof(int));
    is.read(reinterpret_cast<char*>(&lines_iter), sizeof(int));
}

// diamonds machine class implementation

diamond::diamond()
{

}

diamond::diamond(point p, int dl, uint_64 c)
{
    credits = c;
    danger_level = dl;
    loc = p;
    diamond_columns = diamond_column_base_quantity[dl-1];
    diamond_rows = diamond_columns + 1;
    machine_type = MACHINE_DIAMOND;
}

void diamond::initDiamonds()
{
    diamond_lights.resize(diamond_columns, std::vector<bool>( diamond_rows, false));
    num_diamond_lights.resize(diamond_columns, 0);
    // reel.resize((diamond_columns * 5)+5, 0);
    diamond_colors.resize(diamond_columns, cp_darkergrayonblack);
    payouts.resize(diamond_rows, 1);

    // reel[reel.size()-1] = DIAMOND_REEL_BONUS_TILE_VALUE;

    uint secondary_multiplier = randIntZ(danger_level) + 1;

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < (int)diamond_columns; ++j)
        {
            reel.push_back((uint)j);
        }
        reel.push_back((uint)DIAMOND_REEL_BONUS_TILE_VALUE);
    }

    for (uint i = 0; i < payouts.size(); ++i)
    {
        payouts[i] = diamond_base_payout_mod10[i] * diamond_base_danger_multiplier[danger_level-1];

        if (i >= 2 && i <= 5)
        {
            payouts[i] *= secondary_multiplier;
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        bet_values[i] = diamond_base_bet[i] * (uint)((danger_level + 1) / 2);
    }

    current_reel_position = 0;

    is_playing = false;
    bonus_round = false;

    current_winnings = 0ULL;
    spin_rounds_left = max_spin_rounds = diamond_rows;

    bet_iter = randIntZ(2);

    setBet(bet_iter);

    initState();

    initDiamondDisplayAttributes();
}

void diamond::drawDiamondLights()
{
    for (uint i = 0; i < diamond_columns; ++i)
    {
        for (uint j = 0; j < diamond_rows; ++j)
        {
            if (j == 0 && bonus_round)
            {
                gfx_obj.addBitmapCharacter({ getInvertedColorpair(diamond_colors[i]), (int)'B' }, point(MACHINEX_OFFSET + 15 + i, 11 - j + MACHINEY_OFFSET));
            }
            else
            {
                if (!diamond_lights[i][j])
                {
                    gfx_obj.addBitmapCharacter({ diamond_colors[i], 4 }, point(MACHINEX_OFFSET + 15 + i, 11 - j + MACHINEY_OFFSET));
                }
                else
                {
                    gfx_obj.addBitmapCharacter({ getInvertedColorpair(diamond_colors[i]), 4 }, point(MACHINEX_OFFSET + 15 + i, 11 - j + MACHINEY_OFFSET));
                }
            }
        }
    }
}

void diamond::initDiamondDisplayAttributes()
{
    initMachineColorAttributes();

    std::vector<uint> pcolor_list;

    uint index = 0;

    for (uint i = 0; i <= 11; ++i)
    {
        pcolor_list.push_back(i);
    }

    for (uint i = 0; i < diamond_colors.size(); ++i)
    {
        index = randIntZ(pcolor_list.size() - 1);
        diamond_colors[i].bg = color_black;
        diamond_colors[i].fg = getPresetColor(pcolor_list[index]);
        pcolor_list.erase(pcolor_list.begin() + index);
    }
}

void diamond::drawDiamondFrame()
{
    int current_ascii;

    // print machine border one block at a time
    for (int x = 0; x < MACHINEWID; ++x)
    {
        for (int y = 0; y < MACHINEHGT; ++y)
        {
            current_ascii = diamonds_backdrop_array[y][x];

            gfx_obj.addBitmapCharacter({ machine_color, current_ascii }, point(x + MACHINEX_OFFSET, y + MACHINEY_OFFSET));
        }
    }

    for (int x = 0; x < 8; ++x)
    for (int y = 0; y < 9; ++y)
        gfx_obj.addBitmapCharacter({ getInvertedColorpair(cp_darkergrayonblack), 4 }, point(MACHINEX_OFFSET + 15 + x, 11 - y + MACHINEY_OFFSET));

    switch (diamond_rows)
    {
        case(9):
        {
            gfx_obj.addBitmapString("       ", text_color, point(MACHINEX_OFFSET + 7, 3 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({machine_color, 179}, point(MACHINEX_OFFSET + 6, 3 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({machine_color, 179}, point(MACHINEX_OFFSET + 14, 3 + MACHINEY_OFFSET));
        }
        case(8):
        {
            gfx_obj.addBitmapString("       ", text_color, point(MACHINEX_OFFSET + 7, 4 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({ machine_color, 179 }, point(MACHINEX_OFFSET + 6, 4 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({ machine_color, 179 }, point(MACHINEX_OFFSET + 14, 4 + MACHINEY_OFFSET));
        }
        case(7):
        {
            gfx_obj.addBitmapString("       ", text_color, point(MACHINEX_OFFSET + 7, 5 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({ machine_color, 179 }, point(MACHINEX_OFFSET + 6, 5 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({ machine_color, 179 }, point(MACHINEX_OFFSET + 14, 5 + MACHINEY_OFFSET));
       }
       case(6):
       {
            gfx_obj.addBitmapString("       ", text_color, point(MACHINEX_OFFSET + 7, 6 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({ machine_color, 17 }, point(MACHINEX_OFFSET + 6, 6 + MACHINEY_OFFSET));
            gfx_obj.addBitmapCharacter({ machine_color, 179 }, point(MACHINEX_OFFSET + 14, 6 + MACHINEY_OFFSET));
       }
       default:
           break;
    }
}

void diamond::drawMachine()
{
    drawDiamondFrame();
    drawDiamondLights();
    drawDiamondText();
    drawDiamondPayouts();
    drawReelLights();
    drawArrows(true);
}

void diamond::drawReelLights()
{
    uint displacement_iter = 0;

    for (uint i = current_reel_position; i < current_reel_position + NUM_VISIBLE_DIAMOND_SLOTS; ++i)
    {

        point p = point(MACHINEX_OFFSET + 7 + displacement_iter, MACHINEY_OFFSET + 13);

        if (!is_playing)
        {
            gfx_obj.addBitmapCharacter({ arrow_color, 247 }, p);
        }
        else
        {
            if (reel[i % reel.size()] == DIAMOND_REEL_BONUS_TILE_VALUE)
            {
                gfx_obj.addBitmapCharacter({ cp_whiteonblack, diamond_bonus_spin_tile[diamond_bonus_spin_amount[danger_level - 1] - 1] }, p);
            }
            else
            {
                gfx_obj.addBitmapCharacter({ diamond_colors[reel[i % reel.size()]], 4 }, p);
            }
        }

        displacement_iter++;
    }
}

void diamond::drawDiamondText()
{
    gfx_obj.addBitmapString("DIAMONDS!", title_color, point(MACHINEX_OFFSET + 8, 1 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("SPIN", text_color, point(MACHINEX_OFFSET+2,12 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(int2String((uint_64)spin_rounds_left) + "/" + int2String((uint_64)max_spin_rounds), number_color, point(MACHINEX_OFFSET+2,13 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("BET", text_color, point(MACHINEX_OFFSET+2,9 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(int2String((uint_64)total_bet),number_color, point(MACHINEX_OFFSET+2,10 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("WON", text_color, point(MACHINEX_OFFSET+2,3 + MACHINEY_OFFSET));

    std::string winnings_indicator1 = uint642String(current_winnings);
    std::string winnings_indicator2 = "";
    std::string winnings_indicator3 = "";

    if (winnings_indicator1.size() > 4)
    {
        winnings_indicator2 = std::string(winnings_indicator1.begin()+4,winnings_indicator1.end());
        winnings_indicator1 = std::string(winnings_indicator1.begin(),winnings_indicator1.begin()+4);
    }

    if (winnings_indicator2.size() > 4)
    {
        winnings_indicator3 = std::string(winnings_indicator2.begin()+4,winnings_indicator2.end());
        winnings_indicator2 = std::string(winnings_indicator2.begin(),winnings_indicator2.begin()+4);
    }

    gfx_obj.addBitmapString(winnings_indicator1, number_color, point(MACHINEX_OFFSET + 2, 4 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(winnings_indicator2, number_color, point(MACHINEX_OFFSET + 2, 5 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(winnings_indicator3, number_color, point(MACHINEX_OFFSET + 2, 6 + MACHINEY_OFFSET));

    gfx_obj.addBitmapString("CREDIT=", text_color, point(2 + MACHINEX_OFFSET, 15 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString("              ", number_color, point(9 + MACHINEX_OFFSET, 15 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(uint642String(credits), number_color, point(9 + MACHINEX_OFFSET, 15 + MACHINEY_OFFSET));

    drawDiamondStateIndicator(" END GAME ",MSTATE_DIAMONDENDGAME);
    drawDiamondStateIndicator("   PLAY   ",MSTATE_DIAMONDPLAY);
    drawDiamondStateIndicator("  INSERT  ",MSTATE_DIAMONDINSERT);
    drawDiamondStateIndicator(" CASH OUT ",MSTATE_DIAMONDCASHOUT);
    drawDiamondStateIndicator(" NEXT BET ",MSTATE_DIAMONDBET);
    drawDiamondStateIndicator("LEAVE GAME",MSTATE_DIAMONDLEAVE);
}

void diamond::drawDiamondStateIndicator(std::string txt, machine_state s)
{
    color_pair select_color = {text_color.fg,color_verydarkgray};

    point loc = point(2+(((int)((int)(s-8)/3))*11)+MACHINEX_OFFSET,17+(int)((s-8)%3)+MACHINEY_OFFSET);

    if (s != current_state)
    {
        gfx_obj.addBitmapString(txt, text_color, loc);
    }
    else
    {
        gfx_obj.addBitmapString(txt, select_color, loc);
    }
}

uint_64 diamond::useMachine(uint_64 inp_credits)
{
    uint_64 out_cred = 0;

    switch(current_state)
    {
        case(MSTATE_DIAMONDPLAY):
        {
            playRoundOrBet();
            break;
        }
        case(MSTATE_DIAMONDENDGAME):
        {
            if (is_playing)
                resetDiamondMachine();
            break;
        }
        case(MSTATE_DIAMONDBET):
        {
            if (!is_playing)
            {
                setBet(bet_iter + 1);
                drawDiamondText();
            }
            break;
        }
        case(MSTATE_DIAMONDCASHOUT):
        {
            if (!is_playing)
            {
                out_cred = credits;
                credits = 0;
            }
            break;
        }
        case(MSTATE_DIAMONDINSERT):
        {
            credits += inp_credits;
            break;
        }
        default:
            break;
    }

    return out_cred;
}

void diamond::playRoundOrBet()
{
    if (!is_playing)
    {
        if (credits >= total_bet)
        {
            is_playing = true;
            credits -= total_bet;
        }
    }
    else if (spin_rounds_left > 0)
    {
        playRound();
    }
}

void diamond::playRound()
{
    spin_rounds_left--;
    spinReel();
    updateLights();
    checkBonus();
    checkTotalFilled();
    checkWinnings();
    drawDiamondText();
}

void diamond::resetDiamondMachine()
{
    for (uint i = 0; i < diamond_columns; ++i)
    {
        num_diamond_lights[i] = 0;
        for (uint j = 0; j < diamond_rows; ++j)
        {
            diamond_lights[i][j] = false;
        }
    }
    credits += current_winnings;
    current_winnings = 0ULL;
    bonus_round = false;
    is_playing = false;
    spin_rounds_left = max_spin_rounds = diamond_rows;
}

void diamond::drawDiamondPayouts()
{
    uint_64 ith_payout = 1;

    std::string payout_string;

    std::string payout_string_modifier;

    std::string payout_index1_string_modifier = "x0.";

    payout_index1_string_modifier += uint642String(payouts[1]);

    gfx_obj.addBitmapString("WON 0", text_color, point(MACHINEX_OFFSET + 7, 11 + MACHINEY_OFFSET));
    gfx_obj.addBitmapString(payout_index1_string_modifier, text_color, point(MACHINEX_OFFSET + 7, 10 + MACHINEY_OFFSET));

    for (uint i = 5; i < diamond_rows; ++i)
    {
        gfx_obj.addBitmapString("       ", text_color, point(MACHINEX_OFFSET + 7,11 - i + MACHINEY_OFFSET));
    }

    for (uint i = 2; i < payouts.size(); ++i)
    {
        payout_string = "x";
        payout_string_modifier = "";

        ith_payout = (uint_64)(payouts[i] / 10);

        if (ith_payout % 1000000 == 0)
        {
            payout_string_modifier = "M";
            ith_payout /= 1000000;
        }

        else if (ith_payout % 1000 == 0)
        {
            payout_string_modifier = "K";
            ith_payout /= 1000;
        }

        payout_string += uint642String(ith_payout);
        payout_string += payout_string_modifier;

        gfx_obj.addBitmapString(payout_string, text_color, point(MACHINEX_OFFSET + 7,11 - i + MACHINEY_OFFSET));
    }
}

void diamond::shiftReelNTimes(uint n)
{
    current_reel_position += n;

    if (current_reel_position >= reel.size())
    {
        current_reel_position %= reel.size();
    }
}

void diamond::spinReel()
{
    uint n = randIntZ(100) + 100;

    drawArrows(false);
    const uint max_n = randIntZ(10) + 25;

    Uint32 speed = 1;

    while(1)
    {
        delayFunc(speed);
        shiftReelNTimes(1);
        drawReelLights();
        updateFunc();
        n--;
        if (n < max_n)
        {
            speed += 5;
        }
        if (n == 0)
        {
            break;
        }
    }
}

void diamond::checkBonus()
{
    bool enter_bonus_round = true;

    for (uint i = 0; i < diamond_columns; ++i)
    {
        if (num_diamond_lights[i] != 1)
        {
            enter_bonus_round = false;
            break;
        }
    }

    if (enter_bonus_round)
    {
        bonus_round = true;
        if (spin_rounds_left < max_spin_rounds)
        {
            spin_rounds_left = max_spin_rounds;
        }
    }
}

void diamond::updateLights()
{
    uint n = 0;

    uint light_selected = reel[current_reel_position];

    if (light_selected == DIAMOND_REEL_BONUS_TILE_VALUE)
    {
        spin_rounds_left += (diamond_bonus_spin_amount[danger_level - 1] + 1);
    }
    else
    {
        while(1)
        {
            if (n == diamond_rows)
            {
                break;
            }

            if (!diamond_lights[light_selected][n])
            {
                diamond_lights[light_selected][n] = true;
                num_diamond_lights[light_selected]++;
                break;
            }

            n++;
        }
    }

    drawDiamondLights();
}

void diamond::checkWinnings()
{
    uint_64 winnings = 0ULL;

    for (uint i = 0; i < diamond_columns; ++i)
    {
        if (!bonus_round && num_diamond_lights[i] == 1)
        {
            winnings = 0ULL;
            break;
        }
        else if (num_diamond_lights[i] == 2)
        {
            winnings += (uint_64)(((long double)payouts[1] / 10.0L) * (long double)total_bet);
        }
        else if (num_diamond_lights[i] > 2)
        {
            winnings += (uint_64)((payouts[num_diamond_lights[i] - 1] / 10ULL) * (uint_64)total_bet);
        }
    }

    current_winnings = winnings;
}

void diamond::checkTotalFilled()
{
    int numTotalLights = std::accumulate(num_diamond_lights.begin(), num_diamond_lights.end(), 0);

    if (numTotalLights == diamond_columns * diamond_rows)
    {
        spin_rounds_left = 0;
    }
}

void diamond::setBet(uint b)
{
    bet_iter = b <= 2 ? b : 0;
    total_bet = bet_values[bet_iter];
}

void diamond::drawArrows(bool lit)
{
    color_pair colp = (lit == true ? arrow_color : cp_darkergrayonblack);
    gfx_obj.addBitmapCharacter({ colp,31 }, point(7 + MACHINEX_OFFSET, 12 + MACHINEY_OFFSET));
    gfx_obj.addBitmapCharacter({ colp,30 }, point(7 + MACHINEX_OFFSET, 14 + MACHINEY_OFFSET));
}

void diamond::save(std::ofstream& os) const
{
    machine::save(os); // base class member function

    size_t outer = diamond_lights.size();
    os.write(reinterpret_cast<const char*>(&outer), sizeof(size_t));

    for (const auto& row : diamond_lights) {
        size_t inner = row.size();
        os.write(reinterpret_cast<const char*>(&inner), sizeof(size_t));
        for (bool b : row) {
            os.write(reinterpret_cast<const char*>(&b), sizeof(bool));
        }
    }

    size_t size;

    size = reel.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    os.write(reinterpret_cast<const char*>(reel.data()), size * sizeof(uint));

    size = num_diamond_lights.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    os.write(reinterpret_cast<const char*>(num_diamond_lights.data()), size * sizeof(uint));

    size = payouts.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    os.write(reinterpret_cast<const char*>(payouts.data()), size * sizeof(uint_64));

    size = diamond_colors.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& color : diamond_colors)
        colorPairSave(os, color);

    os.write(reinterpret_cast<const char*>(&current_winnings), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&bonus_round), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&is_playing), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&current_reel_position), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&spin_rounds_left), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&max_spin_rounds), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&diamond_rows), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&diamond_columns), sizeof(uint));
    os.write(reinterpret_cast<const char*>(bet_values), sizeof(bet_values));
    os.write(reinterpret_cast<const char*>(&total_bet), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&bet_iter), sizeof(uint));
}

void diamond::load(std::ifstream& is)
{
    machine::load(is); // base class member function

    size_t outer;
    is.read(reinterpret_cast<char*>(&outer), sizeof(size_t));
    diamond_lights.resize(outer);
    for (auto& row : diamond_lights) {
        size_t inner;
        is.read(reinterpret_cast<char*>(&inner), sizeof(size_t));
        row.resize(inner);
        for (size_t i = 0; i < inner; ++i) {
            bool b;
            is.read(reinterpret_cast<char*>(&b), sizeof(bool));
            row[i] = b;
        }
    }

    size_t size;

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    reel.resize(size);
    is.read(reinterpret_cast<char*>(reel.data()), size * sizeof(uint));

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    num_diamond_lights.resize(size);
    is.read(reinterpret_cast<char*>(num_diamond_lights.data()), size * sizeof(uint));

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    payouts.resize(size);
    is.read(reinterpret_cast<char*>(payouts.data()), size * sizeof(uint_64));

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    diamond_colors.resize(size);
    for (auto& color : diamond_colors)
        colorPairLoad(is, color);

    is.read(reinterpret_cast<char*>(&current_winnings), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&bonus_round), sizeof(bool));
    is.read(reinterpret_cast<char*>(&is_playing), sizeof(bool));
    is.read(reinterpret_cast<char*>(&current_reel_position), sizeof(uint));
    is.read(reinterpret_cast<char*>(&spin_rounds_left), sizeof(uint));
    is.read(reinterpret_cast<char*>(&max_spin_rounds), sizeof(uint));
    is.read(reinterpret_cast<char*>(&diamond_rows), sizeof(uint));
    is.read(reinterpret_cast<char*>(&diamond_columns), sizeof(uint));
    is.read(reinterpret_cast<char*>(bet_values), sizeof(bet_values));
    is.read(reinterpret_cast<char*>(&total_bet), sizeof(uint));
    is.read(reinterpret_cast<char*>(&bet_iter), sizeof(uint));
}


// will return one of 12 different sets of colors
color_type getPresetColor(int roller)
{
    color_type c = { 0,0,0 };

    switch (roller)
    {
        // red/green/purple (blue is too dark)
        case(0):
        {
            c.r = (Uint8)randIntZ(100) + 155;
            break;
        }
        case(1):
        {
            c.g = (Uint8)randIntZ(100) + 155;
            break;
        }
        case(2):
        {
            c.r = c.b = (Uint8)randIntZ(100) + 155;
            break;
        }
        // yellow
        case(3):
        {
            c.r = c.g = (Uint8)randIntZ(155) + 100;
            break;
        }
        // cyan
        case(4):
        {
            c.g = c.b = (Uint8)randIntZ(100) + 155;
            break;
        }
        // orange/brown/etc...
        case(5):
        {
            c.r = (Uint8)randIntZ(100) + 155;
            c.g = (Uint8)(c.r / 2);
            break;
        }
        case(6):
        {
            c.g = (Uint8)randIntZ(100) + 155;
            c.r = (Uint8)(c.g / 2);
            break;
        }
        // green/blue blue/green variants
        case(7):
        {
            c.g = (Uint8)randIntZ(100) + 155;
            c.b = (Uint8)(c.g / 2);
            break;
        }
        case(8):
        {
            c.b = (Uint8)randIntZ(100) + 155;
            c.g = (Uint8)(c.b / 2);
            break;
        }
        // reddish/brown
        case(9):
        {
            c.r = (Uint8)randIntZ(100) + 155;
            c.g = (Uint8)(c.r / 3);
            break;
        }
        // pinkish color
        case(10):
        {
            c.r = (Uint8)randIntZ(100) + 155;
            c.g = c.b = (Uint8)(c.r / 2);
            break;
        }
        // white/gray
        case(11):
        {
            c.r = c.g = c.b = (Uint8)randIntZ(155) + 100;
            break;
        }
    }

    return c;
}