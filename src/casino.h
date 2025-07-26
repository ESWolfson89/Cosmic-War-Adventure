#ifndef CASINO_H_
#define CASINO_H_

#include "point.h"
#include "rng.h"
#include "misc.h"
#include "graphics.h"
// slot
#define BASE_NUM_SLOT_REEL_TILES 55

#define NUM_SLOT_REEL_TILES_OFFSET 1

#define NUM_SLOT_REELS 3

#define NUM_VISIBLE_LINES 3
// diamonds
#define NUM_VISIBLE_DIAMOND_SLOTS 16

#define DIAMOND_REEL_BONUS_TILE_VALUE 10

#define MACHINEWID 24

#define MACHINEHGT 24

#define NUM_TOTAL_SLOT_TILES 12

#define MACHINEX_OFFSET 28

#define MACHINEY_OFFSET 7

enum MachineType
{
    MACHINE_SLOT,
    MACHINE_DIAMOND
};

// top right corner  191
// bttm left corner  192
// bttm right corner 217
// top left corner   218
// horiz up corner   193
// horiz down corner 194
// vert right corner 195
// vert left corner  180
// horiz pipe        196
// vert pipe         179
// cross section     197

static const int slot_backdrop_array[MACHINEHGT][MACHINEWID] =
{
    { 0 , 0 , 0 ,218,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,191, 0 , 0 },
    { 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 },
    { 0 ,218,196,193,196,196,196,196,196,196,196,196,194,196,196,196,196,196,196,196,196,193,196,191},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,193,191, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,196,196,196,194,196,219,196,219,196,194,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 ,179, 0 ,179, 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 ,179, 0 ,179, 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 ,179, 0 ,179, 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,196,196,196,193,196,193,196,193,196,193,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,196,196,196,196,196,196,194,196,196,196,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,196,196,196,196,196,196,193,196,196,196,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,192,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,217}
};

static const int diamonds_backdrop_array[MACHINEHGT][MACHINEWID] =
{
    { 0 , 0 , 0 ,218,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,191, 0 , 0 },
    { 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 },
    { 0 ,218,196,193,196,196,194,196,196,196,196,196,196,196,194,196,196,196,196,196,196,193,196,191},
    { 0 ,179, 0 , 0 , 0 , 0 ,195,196,196,196,196,196,196,196,180, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,195,196,196,196,196,196,196,196,180, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,195,196,196,196,196,196,196,196,180, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,195,196,196,196,196,196,196,196,180, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 ,195,196,196,196,196,196,196,196,193,196,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,193,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,196,196,196,196,196,196,194,196,196,196,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,195,196,196,196,196,196,196,196,196,196,196,193,196,196,196,196,196,196,196,196,196,196,180},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,179, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179},
    { 0 ,192,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,217}
};

// ...chtype symbol
/*
static const int slot_tile_char[NUM_TOTAL_SLOT_TILES*3 - 4] =
{
    (int)'N', (int)' ', 155, (int)'$', (int)'*', (int)'-', (int)'=', 240, 4, (int)'7', (int)19, (int)'&',
    11,12,(int)'+',224,225,226,3,(int)'0','D',228,231,235,37,13,14,15,6,(int)'6',21,236
};
*/
static const int slot_tile_char[12] =
{
    (int)'N',
    (int)' ',
    (int)235,
    (int)'$',
    (int)'*',
    (int)'-',
    (int)'=',
    (int)240,
    (int)'7',
    (int)'7',
    (int)4,
    (int)'J'
};

static const uint slot_base_payout[10] =
{
    1,2,5,10,50,100,250,1000,4000,25000
};

static const uint slot_base_danger_multiplier[20] =
{
    1,2,3,5,8,10,12,14,16,20,23,26,29,33,37,44,52,60,70,80
};

static const uint diamond_base_danger_multiplier[20] =
{
    1,2,2,2,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9
};

static const uint diamond_column_base_quantity[20] =
{
    4,4,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8
};

static const uint slot_base_bet[5] =
{
    1,2,5,10,25
};

static const uint slot_base_lines[5] =
{
    1,3,5,7,9
};

static const uint base_dollar_match_multiplier[20] =
{
    1,2,2,2,3,3,3,3,3,4,4,4,5,5,5,6,6,6,7,7
};

static const uint slot_base_bonus_multiplier[2] =
{
    2,3
};

static const uint diamond_base_bet[3] =
{
    10,50,100
};

static const uint_64 diamond_base_payout_mod10[9] =
{
    0,1,10,100,500,10000,250000,10000000,500000000
};

static const uint diamond_bonus_spin_amount[20] =
{
    1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,3,3,3,3
};

static const int diamond_bonus_spin_tile[3] =
{
    (int)'S',
    (int)'2',
    (int)'3'
};

enum machine_state
{
    MSTATE_SLOTSPINMAX,
    MSTATE_SLOTSPIN,
    MSTATE_SLOTINSERT,
    MSTATE_SLOTCASHOUT,
    MSTATE_SLOTSPINONE,
    MSTATE_SLOTBETAMOUNT,
    MSTATE_SLOTBETLINES,
    MSTATE_SLOTLEAVE,
    MSTATE_DIAMONDENDGAME,
    MSTATE_DIAMONDPLAY,
    MSTATE_DIAMONDINSERT,
    MSTATE_DIAMONDCASHOUT,
    MSTATE_DIAMONDBET,
    MSTATE_DIAMONDLEAVE
};

// tile on slot reel
// SS stands for slot symbol
enum slot_tile
{
    NULL_s,
    BLANK,
    SS_1,SS_2,SS_3,SS_4,SS_5,SS_6,SS_7,SS_8,SS_9,SS_10
};

class machine
{
    public:

        machine();

        uint_64 getMachineCredits();

        point at();

        uint_64 getCredits();

        void setCredits(uint_64);

        void initMachineColorAttributes();

        void delayFunc(Uint32);

        void updateFunc();

        void gotoNextState(point);

        void setState(machine_state);

        void initState();

        virtual void save(std::ofstream& os) const;

        virtual void load(std::ifstream& is);

        MachineType getMachineType();

        machine_state getState();

        color_pair getMachineColor();

    protected:

        uint_64 credits;

        int rig_factor;

        int danger_level;

        color_pair machine_color;

        color_pair title_color;

        color_pair text_color;

        color_pair arrow_color;

        color_pair number_color;

        MachineType machine_type;

        machine_state current_state;

        point loc;
};

class slot : public machine
{
    public:

        slot();

        slot(point, int, uint_64);

        void initSlots();

        void initSlotDisplayAttributes();

        void drawSlotReels();

        void drawSlotTile(point, slot_tile);

        void drawSlotTileKey();

        void drawMachine();

        void drawSlotFrame();

        void drawAllArrows(bool);

        void drawSlotText();

        void drawSlotStateIndicator(std::string, machine_state);

        void drawArrow(uint, bool);

        void spinReel();

        void setBet(uint,uint);

        void flickerDollarSignColor();

        void calculateSpinWinnings();

        void setLineWinnings(slot_tile,slot_tile,slot_tile);

        void save(std::ofstream& os) const override;

        void load(std::ifstream& is) override;

        uint_64 useMachine(uint_64);

        uint getTop2MatchMultiplier();

        uint getDollarMatchMultiplier(slot_tile);

        uint getBetVal(uint);

        uint getLineVal(uint);

    private:

        std::vector<slot_tile> reel;

        uint reel_state[NUM_SLOT_REELS][NUM_VISIBLE_LINES];

        uint line_payouts[10];

        uint bet_values[5];

        uint tile_multipliers[2];

        chtype slot_tile_symbol[NUM_TOTAL_SLOT_TILES];

        color_pair dollar_colors[10];

        uint dollar_match_multiplier;

        uint current_dollar_color;

        uint current_bet;

        uint current_lines;

        uint current_total_bet;

        uint_64 last_winnings;

        int last_line_win_tile;

        int bet_iter;

        int lines_iter;
};

class diamond : public machine
{
    public:

        diamond();

        diamond(point,int,uint_64);

        void initDiamonds();

        void initDiamondDisplayAttributes();

        void drawDiamondLights();

        void drawDiamondFrame();

        void drawMachine();

        void drawDiamondText();

        void drawReelLights();

        void drawDiamondStateIndicator(std::string, machine_state);

        void drawDiamondPayouts();

        void drawArrows(bool);

        void playRound();

        void resetDiamondMachine();

        void setBet(uint);

        void spinReel();

        void updateLights();

        void shiftReelNTimes(uint);

        void checkWinnings();

        void checkBonus();

        void playRoundOrBet();

        void checkTotalFilled();

        void save(std::ofstream& os) const override;

        void load(std::ifstream& is) override;

        uint_64 useMachine(uint_64);

    private:

        std::vector<std::vector<bool>> diamond_lights;

        std::vector<uint> reel;

        std::vector<uint> num_diamond_lights;

        std::vector<uint_64> payouts;

        std::vector<color_pair> diamond_colors;

        uint_64 current_winnings;

        bool bonus_round;

        bool is_playing;

        uint current_reel_position;

        uint spin_rounds_left;

        uint max_spin_rounds;

        uint diamond_rows;

        uint diamond_columns;

        uint bet_values[3];

        uint total_bet;

        uint bet_iter;
};

int getSlotAscii(uint);

color_type getPresetColor(int roller);

#endif
