#include "namegen.h"

std::string genName(int num_compounds)
{
    std::string curr_name = "";

    bool start_vowel = false;

	for (int i = 0; i <= num_compounds; ++i)
	{
	    if (i == 0)
        {
            if (roll(3))
            {
               curr_name.append(vowel[randInt(0,4)]);
               start_vowel = true;
            }
            else
            {
               curr_name.append(starting_consonant[randInt(0,9)]);
               start_vowel = false;
            }
        }
        else
        {
            if (start_vowel)
            {
                if (i % 2 == 0)
                    curr_name.append(toupper(curr_name[(int)curr_name.size()-1]) == 'Q' ? "u" : vowel[randInt(0,4)]);
                else
                    curr_name.append(middle_consonant[randInt(0,19)]);
            }
            else
            {
                if (i % 2 == 0)
                    curr_name.append(middle_consonant[randInt(0,19)]);
                else
                    curr_name.append(toupper(curr_name[(int)curr_name.size()-1]) == 'Q' ? "u" : vowel[randInt(0,4)]);
            }
        }
	}

    //if ((start_vowel == true && num_compounds % 2 == 0) || (start_vowel == false && num_compounds % 2 == 1))
    //{
    //    curr_name.append(middle_consonant[randInt(0,18)]);
    //}

    //curr_name.append(final_name_modifier[randInt(0,14)]);

    curr_name[0] = toupper(curr_name[0]);

    return curr_name;
}
