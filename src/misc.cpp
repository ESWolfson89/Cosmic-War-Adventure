#include "misc.h"

void stringSave(std::ofstream& os, const std::string& s)
{
    int string_size = (int)s.size();
    os.write(reinterpret_cast<const char*>(&string_size), sizeof(int));
    os.write(s.data(), string_size); // Write all characters at once
}

void stringLoad(std::ifstream& is, std::string& s)
{
    int string_size = 0;
    is.read(reinterpret_cast<char*>(&string_size), sizeof(int));

    s.resize(string_size);  // allocate space
    is.read(&s[0], string_size);  // read all characters at once
}

void chtypeSave(std::ofstream& os, const chtype& ct)
{
    os.write(reinterpret_cast<const char*>(&ct.ascii), sizeof(int));
    colorPairSave(os, ct.color);
}

void chtypeLoad(std::ifstream& is, chtype& ct)
{
    is.read(reinterpret_cast<char*>(&ct.ascii), sizeof(int));
    colorPairLoad(is, ct.color);
}

void colorPairSave(std::ofstream& os, const color_pair& cp)
{
    colorTypeSave(os, cp.fg);
    colorTypeSave(os, cp.bg);
}

void colorPairLoad(std::ifstream& is, color_pair& cp)
{
    colorTypeLoad(is, cp.fg);
    colorTypeLoad(is, cp.bg);
}

void colorTypeSave(std::ofstream& os, const color_type& col)
{
    os.write(reinterpret_cast<const char*>(&col.r), sizeof(Uint8));
    os.write(reinterpret_cast<const char*>(&col.g), sizeof(Uint8));
    os.write(reinterpret_cast<const char*>(&col.b), sizeof(Uint8));
}

void colorTypeLoad(std::ifstream& is, color_type& col)
{
    is.read(reinterpret_cast<char*>(&col.r), sizeof(Uint8));
    is.read(reinterpret_cast<char*>(&col.g), sizeof(Uint8));
    is.read(reinterpret_cast<char*>(&col.b), sizeof(Uint8));
}

void saveDiceRoll(std::ofstream& os, const dice_roll& d)
{
    os.write(reinterpret_cast<const char*>(&d.num_dice), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&d.num_sides), sizeof(uint));
    os.write(reinterpret_cast<const char*>(&d.num_bonus), sizeof(uint));
}

void loadDiceRoll(std::ifstream& is, dice_roll& d)
{
    is.read(reinterpret_cast<char*>(&d.num_dice), sizeof(uint));
    is.read(reinterpret_cast<char*>(&d.num_sides), sizeof(uint));
    is.read(reinterpret_cast<char*>(&d.num_bonus), sizeof(uint));
}

std::string uint642String(uint_64 u64)
{
    std::stringstream ss;
    ss << u64;
    return ss.str();
}

std::string double2String(double d)
{
    std::stringstream ss;
    ss << d;
    return ss.str();
}

std::string int2String(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

std::string capitalizeString(std::string s)
{
    std::string ret_val = s;
    for (int i = 0; i < (int)ret_val.size(); ++i)
    {
        if ((int)ret_val[i] >= 'a' && (int)ret_val[i] <= 'z')
        {
            ret_val[i] = toupper(ret_val[i]);
        }
    }
    return ret_val;
}

