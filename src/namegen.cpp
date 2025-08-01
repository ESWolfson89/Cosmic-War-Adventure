#include "namegen.h"

std::string genAlienName(int syllableCount)
{
    std::string name;
    std::string core;

    // 50% chance to begin with single-letter consonant or exotic multi-letter prefix
    if (roll(2)) 
    {
        name += startingConsonant[randInt(0, static_cast<int>(startingConsonant.size() - 1))];
    }
    else
    {
        name += exoticPrefixes[randInt(0, static_cast<int>(exoticPrefixes.size() - 1))];
    }

    bool startsWithVowel = roll(3);

    for (int i = 0; i < syllableCount; ++i)
    {
        std::string chunk;
        if (startsWithVowel)
        {
            chunk = roll(5)
                ? vowel[randInt(0, 4)]
                : doubleVowelCombos[randInt(0, static_cast<int>(doubleVowelCombos.size() - 1))];
        }
        else
        {
            chunk = roll(5)
                ? middleConsonant[randInt(0, 19)]
                : doubleConsonantCombos[randInt(0, static_cast<int>(doubleConsonantCombos.size() - 1))];
        }

        // Add optional glottal marker
        if (!core.empty() && roll(8))
            core += "'";

        core += chunk;
        startsWithVowel = !startsWithVowel;
    }

    name += core;

    if (roll(2))
        name += exoticSuffixes[randInt(0, static_cast<int>(exoticSuffixes.size() - 1))];

    if (roll(12) && name.size() < 8)
    {
        std::string reversed = core;
        std::reverse(reversed.begin(), reversed.end());
        name += reversed.substr(0, randInt(1, 3));
    }

    if (roll(5))
    {
        int codeEndRandNumber = randIntZ(19);

        if (codeEndRandNumber <= 17)
        {
            if (roll(2))
            {
                name += "-" + codeSet[randInt(0, static_cast<int>(codeSet.size() - 1))];
            }
            else
            {
                name += "-" + numberSet[randInt(0, static_cast<int>(numberSet.size() - 1))];
            }
        }
        else
        {
            name += "-";
            int numbers = randInt(2, 4);
            for (int i = 0; i < numbers; i++)
            {
                name += numberSet[randInt(0, static_cast<int>(numberSet.size() - 1))];
            }

            if (roll(3))
            {
                name += codeSet[randInt(0, static_cast<int>(codeSet.size() - 1))];
            }
        }
    }

    if (!name.empty())
        name[0] = static_cast<char>(std::toupper(name[0]));

    return name;
}

std::string getNamePrefix(MobShip* mob)
{
    if (mob->isCurrentPlayerShip())
        return "Your";
    return "The";
}