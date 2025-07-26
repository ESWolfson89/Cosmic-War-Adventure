#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <algorithm>
#include <numeric>
#include <cstdint>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TILEWID 8
#define TILEHGT 8

#define BITMAPROWS 16
#define BITMAPCOLS 16

#define GRIDWID 80
#define GRIDHGT 48

#define SCREENWID (TILEWID * GRIDWID)
#define SCREENHGT (TILEHGT * GRIDHGT)

#define STARMAPWID 36
#define STARMAPHGT 36

#define SHOWWID 36
#define SHOWHGT 36

#define HALFWID 18
#define HALFHGT 18

#define MINLOCALELENGTH 12
#define MAXLOCALELENGTH 72

#define LOCALEREGIONDEFAULTWID (SHOWWID)
#define LOCALEREGIONDEFAULTHGT (SHOWHGT)

#define CSYS (universe.getSubAreaMapType() == SMT_PERSISTENT ? universe.getSubArea(current_subarea_id) : universe.getNPSubArea())

#define uint unsigned int
#define uint_64 unsigned long long int

#define NUM_TOTAL_MODULE_TYPES 7

#define NUM_TOTAL_WEAPON_TYPES 17

#define NUM_TOTAL_ENGINE_TYPES 3

#define NUM_TOTAL_SHIELD_TYPES 3

#define NUM_TOTAL_RACES 75

#define PLAYER_START_X 2
#define PLAYER_START_Y 33

#define MAX_SHIPS_PER_LOCALE 60

#define NUM_POSSIBLE_SHIP_COLORS 24

#define NUM_POSSIBLE_SHIPDESIGN_SYMBOLS 25

#define NUM_STAR_TYPES 4

#define NUM_STAR_SYSTEMS 167

#define MIN_UNDSC_STAR_INT (int)SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE
#define MAX_UNDSC_STAR_INT (int)SMBACKDROP_WHITESTARSUBAREAENTRANCE


// color structure
struct color_type
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

struct color_pair
{
    // foreground colors
    color_type fg;
    // background colors
    color_type bg;
};

struct chtype
{
    color_pair color;
    int ascii;
};

enum StationType
{
    STATION_SHIP,
    STATION_ENTERTAINMENT
};

enum EntertainmentType
{
    ET_SLOTS,
    ET_DIAMONDS
};

enum MapType
{
    MAPTYPE_STARMAP,
    MAPTYPE_LOCALEMAP
};

enum subarea_MapType
{
    SMT_NONE,
    SMT_NONPERSISTENT,
    SMT_PERSISTENT
};

enum subarea_specific_type
{
    SST_NONE,
    SST_EMPTYVOID,
    SST_PIRATEVOID,
    SST_RACEHOME,
    SST_EMPTYSYSTEM,
    SST_WARZONE
};

enum mob_t
{
    NIL_m,
    SHIP_PLAYER,
    SHIP_PROCGEN
};

enum item_t
{
    NIL_i,
    ITEM_WRECKAGECREDIT
};

enum backdrop_t
{
    NIL_b,
    SMBACKDROP_SPACE,
    SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE,
    SMBACKDROP_REDSTARSUBAREAENTRANCE,
    SMBACKDROP_BLUESTARSUBAREAENTRANCE,
    SMBACKDROP_WHITESTARSUBAREAENTRANCE,
    SMBACKDROP_FRIENDRACE_MAINSEQSTARSUBAREAENTRANCE,
    SMBACKDROP_FRIENDRACE_REDSTARSUBAREAENTRANCE,
    SMBACKDROP_FRIENDRACE_BLUESTARSUBAREAENTRANCE,
    SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE,
    SMBACKDROP_HOSTILERACE_MAINSEQSTARSUBAREAENTRANCE,
    SMBACKDROP_HOSTILERACE_REDSTARSUBAREAENTRANCE,
    SMBACKDROP_HOSTILERACE_BLUESTARSUBAREAENTRANCE,
    SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE,
    SMBACKDROP_WARZONE_MAINSEQSTARSUBAREAENTRANCE,
    SMBACKDROP_WARZONE_REDSTARSUBAREAENTRANCE,
    SMBACKDROP_WARZONE_BLUESTARSUBAREAENTRANCE,
    SMBACKDROP_WARZONE_WHITESTARSUBAREAENTRANCE,
    SMBACKDROP_EMPTY_MAINSEQSTARSUBAREAENTRANCE,
    SMBACKDROP_EMPTY_REDSTARSUBAREAENTRANCE,
    SMBACKDROP_EMPTY_BLUESTARSUBAREAENTRANCE,
    SMBACKDROP_EMPTY_WHITESTARSUBAREAENTRANCE,
    LBACKDROP_PLANET,
    LBACKDROP_ENSLAVEDPLANET,
    LBACKDROP_SPACESTATION_SHIP,
    LBACKDROP_SPACESTATION_ENTERTAINMENT,
    LBACKDROP_SPACEWALL,
    LBACKDROP_SPACE_UNLIT,
    LBACKDROP_SPACE_LIT,
    LBACKDROP_MAINSEQSTARBACKGROUND,
    LBACKDROP_REDSTARBACKGROUND,
    LBACKDROP_BLUESTARBACKGROUND,
    LBACKDROP_WHITESTARBACKGROUND
};

enum fire_t
{
    NIL_f,
    FIRET_BLUELINE,
    FIRET_BLUELINEHORIZ,
    FIRET_BLUELINEDIAG1,
    FIRET_BLUELINEDIAG2,
    FIRET_REDLINE,
    FIRET_REDLINEHORIZ,
    FIRET_REDLINEDIAG1,
    FIRET_REDLINEDIAG2,
    FIRET_PURPLELINE,
    FIRET_PURPLELINEHORIZ,
    FIRET_PURPLELINEDIAG1,
    FIRET_PURPLELINEDIAG2,
    FIRET_GRAYLINE,
    FIRET_GRAYLINEHORIZ,
    FIRET_GRAYLINEDIAG1,
    FIRET_GRAYLINEDIAG2,
    FIRET_EXPLOSION,
    FIRET_DAMAGINGEXPLOSION,
    FIRET_PURPLEPLASMA,
    FIRET_GREENWALLOP,
    FIRET_DARKRAZOR,
    FIRET_CURSOR,
    FIRET_CURSORPATH
};

static const color_type color_white = {255,255,255};
static const color_type color_black = {0,0,0};
static const color_type color_darkgray = {64,64,64};
static const color_type color_creditgray = {96,96,96};
static const color_type color_verydarkgray = {30,30,30};
static const color_type color_darkergray = { 45,45,45 };
static const color_type color_space {15,15,15};
static const color_type color_gray = {128,128,128};
static const color_type color_red = {255,0,0};
static const color_type color_lightred = {255,144,144};
static const color_type color_yellow = {255,255,0};
static const color_type color_green = {0,255,0};
static const color_type color_darkgreen = { 0,128,0 };
static const color_type color_darkred = {128,0,0};
static const color_type color_orange = {255,128,0};
static const color_type color_brown = {180,90,0};
static const color_type color_cyan = {0,192,192};
static const color_type color_lightblue = {100,200,255};
static const color_type color_lightgray = {192,192,192};
static const color_type color_blue = {0,0,255};
static const color_type color_purple = {255,0,255};
static const color_type color_darkpurple = {128,0,128};
static const color_type color_mainseq = {255,255,128};
static const color_type color_planetblue = {0,128,255};

static const color_pair cp_blackonwhite = {color_black,color_white};
static const color_pair cp_blackongray = {color_black,color_gray};
static const color_pair cp_grayonblack = {color_gray,color_black};
static const color_pair cp_darkgrayonblack = {color_darkgray,color_black};
static const color_pair cp_blackonblack = {color_black,color_black};
static const color_pair cp_mainseqonblack= {color_mainseq,color_black};
static const color_pair cp_redondarkgray = {color_red,color_darkgray};
static const color_pair cp_redonblack = {color_red,color_black};
static const color_pair cp_whiteonblack = {color_white,color_black};
static const color_pair cp_greenonblack = {color_green,color_black};
static const color_pair cp_lightredonblack = {color_lightred,color_black};
static const color_pair cp_lightblueonblack = {color_lightblue,color_black};
static const color_pair cp_lightgrayonblack = {color_lightgray,color_black};
static const color_pair cp_yellowonblack = {color_yellow,color_black};
static const color_pair cp_orangeonblack = {color_orange,color_black};
static const color_pair cp_brownonblack = {color_brown,color_black};
static const color_pair cp_darkredonblack = {color_darkred,color_black};
static const color_pair cp_darkgrayonwhite = {color_darkgray,color_white};
static const color_pair cp_purpleonblack = {color_purple,color_black};
static const color_pair cp_blueonblack = {color_blue,color_black};
static const color_pair cp_darkergrayonblack = { color_darkergray, color_black };
static const color_pair cp_verydarkgrayonblack = {color_verydarkgray,color_black};
static const color_pair cp_spaceonblack = {color_space,color_black};
static const color_pair cp_purpleonverydarkgray = {color_purple,color_verydarkgray};
static const color_pair cp_redonverydarkgray = {color_red,color_verydarkgray};
static const color_pair cp_blueonverydarkgray = {color_blue,color_verydarkgray};
static const color_pair cp_cyanonblack = {color_cyan,color_black};
static const color_pair cp_blackoncreditgray = {color_black,color_creditgray};
static const color_pair cp_creditgrayonblack = {color_creditgray,color_black};
static const color_pair cp_planetblueonverydarkgray = {color_planetblue,color_verydarkgray};
static const color_pair cp_blackondarkgray = {color_black,color_darkgray};
static const color_pair cp_blackonverydarkgray = {color_black,color_verydarkgray};
static const color_pair cp_blackonspace = {color_black,color_space};

static const chtype blank_ch = {cp_blackonblack,(int)' '};
static const chtype blank_grid_ch = {cp_spaceonblack,254};

static const chtype blanktile_ch = {cp_spaceonblack,219};
static const chtype gray_rect = {cp_grayonblack,219};
static const chtype white_rect = {cp_whiteonblack,219};
static const chtype lightblue_flame = {cp_lightblueonblack,247};
static const chtype gray_horizontal_pipe = {cp_grayonblack,205};
static const chtype gray_vertical_pipe = {cp_grayonblack,186};
static const chtype gray_rightarrow = {cp_grayonblack,16};
static const chtype up_arrow_selector = {cp_whiteonblack,24};
static const chtype hull_upgrade_symbol = {cp_lightgrayonblack,24};
static const chtype hull_repair_symbol = {cp_whiteonblack,(int)'+'};
static const chtype module_buy_symbol = {cp_purpleonblack,19};
static const chtype slot_upgrade_symbol = {cp_grayonblack,29};
static const chtype crew_upgrade_symbol = {cp_greenonblack,139};
static const chtype crewpod_symbol = {cp_greenonblack,(int)'@'};
static const chtype fuelupgrade_symbol = {cp_redonblack,219};
static const chtype fueltank_symbol = {cp_orangeonblack,159};
static const chtype module_sell_symbol = {cp_redonblack,(int)'$'};
static const chtype crewpod_display_symbol = {cp_lightgrayonblack,186};

static const int ship_design_pattern_ch[NUM_POSSIBLE_SHIPDESIGN_SYMBOLS] =
{
    (int)'0',
    (int)'1',
    (int)'5',
    (int)'8',
    (int)'o',
    (int)'0',
    (int)'=',
    (int)'&',
    (int)'X',
    (int)'@',
    (int)'>',
    (int)')',
    (int)']',
    21,
    175,
    195,
    196,
    197,
    206,
    216,
    219,
    233,
    239,
    240,
    249
};

static const color_pair procgen_ship_colors[NUM_POSSIBLE_SHIP_COLORS] =
{
  {{255,0,0},color_black},
  {{128,0,0},color_black},
  {{196,96,96},color_black},
  {{255,144,0},color_black},
  {{255,64,64},color_black},
  {{180,90,0},color_black},
  {{0,192,192},color_black},
  {{0,0,228},color_black},
  {{160,0,160},color_black},
  {{96,0,96},color_black},
  {{48,100,144},color_black},
  {{0,120,0},color_black},
  {{0,160,0},color_black},
  {{0,200,0},color_black},
  {{189,131,87},color_black},
  {{50,50,50},color_black},
  {{64,64,64},color_black},
  {{78,78,78},color_black},
  {{100,100,100},color_black},
  {{128,128,128},color_black},
  {{150,150,150},color_black},
  {{175,175,175},color_black},
  {{200,200,200},color_black},
  {{225,225,225},color_black}
};

static const chtype fire_symbol[24] =
{
    blank_ch,
    {cp_cyanonblack,179},
    {cp_cyanonblack,196},
    {cp_cyanonblack,92},
    {cp_cyanonblack,(int)'/'},
    {cp_redonblack,179},
    {cp_redonblack,196},
    {cp_redonblack,92},
    {cp_redonblack,(int)'/'},
    {cp_purpleonblack,179},
    {cp_purpleonblack,196},
    {cp_purpleonblack,92},
    {cp_purpleonblack,(int)'/'},
    {cp_grayonblack,179},
    {cp_grayonblack,196},
    {cp_grayonblack,92},
    {cp_grayonblack,(int)'/'},
    {cp_orangeonblack,(int)'*'},
    {cp_redonblack,(int)'*'},
    {cp_purpleonblack,(int)'*'},
    {cp_greenonblack,(int)'*'},
    {cp_darkgrayonblack,15},
    {cp_whiteonblack,(int)'X'},
    {cp_whiteonblack,(int)'*'}
};

static const chtype mob_symbol[9] =
{
    blank_ch,
    {cp_whiteonblack,(int)'@'},
    {cp_darkgrayonblack,(int)'o'},
    {cp_lightblueonblack,(int)'A'},
    {cp_blueonblack,1},
    {cp_purpleonblack,(int)'j'},
    {cp_orangeonblack,2},
    {cp_redonblack,(int)'6'},
    {cp_grayonblack,(int)219}
};

static const chtype item_symbol[2] =
{
    blank_ch,
    {cp_grayonblack,37}
};

static const chtype backdrop_symbol[33] =
{
    blank_ch,
    {cp_verydarkgrayonblack,(int)'~'},
    {cp_mainseqonblack,(int)'*'},
    {cp_lightredonblack,(int)'*'},
    {cp_lightblueonblack,(int)'*'},
    {cp_whiteonblack,(int)'*'},
    {{color_mainseq, color_darkgreen},(int)'*'},
    {{color_lightred, color_darkgreen},(int)'*'},
    {{color_lightblue, color_darkgreen},(int)'*'},
    {{color_white, color_darkgreen},(int)'*'},
    {{color_mainseq, color_darkred},(int)'*'},
    {{color_lightred, color_darkred},(int)'*'},
    {{color_lightblue, color_darkred},(int)'*'},
    {{color_white, color_darkred},(int)'*'},
    {{color_mainseq, color_darkpurple},(int)'*'},
    {{color_lightred, color_darkpurple},(int)'*'},
    {{color_lightblue, color_darkpurple},(int)'*'},
    {{color_white, color_darkpurple},(int)'*'},
    {{color_mainseq, color_gray},(int)'*'},
    {{color_lightred, color_gray},(int)'*'},
    {{color_lightblue, color_gray},(int)'*'},
    {{color_white, color_gray},(int)'*'},
    {cp_planetblueonverydarkgray,(int)'*'},
    {cp_purpleonverydarkgray,(int)'*'},
    {cp_lightgrayonblack,157},
    {cp_redonblack, 21},
    {cp_grayonblack,(int)'#'},
    {cp_darkgrayonblack,250},
    {cp_lightgrayonblack,250},
    {cp_mainseqonblack,219},
    {cp_lightredonblack,219},
    {cp_lightblueonblack,219},
    {cp_whiteonblack,219}
};

enum race_type
{
    RACETYPE_NONE,
    RACETYPE_AHRKON,
    RACETYPE_OOLIG,
    RACETYPE_PROCGEN
};

enum npc_ship_type
{
    NPCSHIPTYPE_NONE,
    NPCSHIPTYPE_WAR,
    NPCSHIPTYPE_PIRATE
};

extern bool game_active;
extern bool playerHasMoved;
extern int wait_counter;
extern int gmti;

#endif
