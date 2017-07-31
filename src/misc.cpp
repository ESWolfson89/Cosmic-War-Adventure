#include "misc.h"

void stringSave(std::ofstream &os, std::string s)
{
    char c;
    int string_size = (int)s.size();
    os.write((const char *)&string_size,sizeof(int));
    for (int i = 0; i < string_size; ++i)
    {
        c = s[i];
        os.write((const char *)&c,sizeof(char));
    }
}

void stringLoad(std::ifstream &is, std::string &s)
{
    char c;
    int string_size = 0;
    is.read((char *)&string_size,sizeof(int));
    for (int i = 0; i < string_size; ++i)
    {
        is.read((char *)&c,sizeof(char));
        s += c;
    }
}

void chtypeSave(std::ofstream &os, chtype ct)
{
    os.write((const char *)&ct.ascii,sizeof(int));
    os.write((const char *)&ct.color.fg.r,sizeof(Uint8));
    os.write((const char *)&ct.color.fg.g,sizeof(Uint8));
    os.write((const char *)&ct.color.fg.b,sizeof(Uint8));
    os.write((const char *)&ct.color.bg.r,sizeof(Uint8));
    os.write((const char *)&ct.color.bg.g,sizeof(Uint8));
    os.write((const char *)&ct.color.bg.b,sizeof(Uint8));
}

void chtypeLoad(std::ifstream &is, chtype &ct)
{
    is.read((char *)&ct.ascii,sizeof(int));
    is.read((char *)&ct.color.fg.r,sizeof(Uint8));
    is.read((char *)&ct.color.fg.g,sizeof(Uint8));
    is.read((char *)&ct.color.fg.b,sizeof(Uint8));
    is.read((char *)&ct.color.bg.r,sizeof(Uint8));
    is.read((char *)&ct.color.bg.g,sizeof(Uint8));
    is.read((char *)&ct.color.bg.b,sizeof(Uint8));
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

