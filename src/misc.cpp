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

void chtypeSave(std::ofstream &os, const chtype &ct)
{
    os.write((const char *)&ct.ascii,sizeof(int));
    colorPairSave(os, ct.color);
}

void chtypeLoad(std::ifstream &is, chtype &ct)
{
    is.read((char *)&ct.ascii,sizeof(int));
    colorPairLoad(is, ct.color);
}

void colorPairSave(std::ofstream& os, const color_pair &cp)
{
    colorTypeSave(os, cp.fg);
    colorTypeSave(os, cp.bg);
}

void colorPairLoad(std::ifstream& is, color_pair& cp)
{
    colorTypeLoad(is, cp.fg);
    colorTypeLoad(is, cp.bg);
}

void colorTypeSave(std::ofstream& os, const color_type &col)
{
    os.write((const char*)&col.r, sizeof(Uint8));
    os.write((const char*)&col.g, sizeof(Uint8));
    os.write((const char*)&col.b, sizeof(Uint8));
}

void colorTypeLoad(std::ifstream& is, color_type& col)
{
    is.read((char*)&col.r, sizeof(Uint8));
    is.read((char*)&col.g, sizeof(Uint8));
    is.read((char*)&col.b, sizeof(Uint8));
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

