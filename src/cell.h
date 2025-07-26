#ifndef CELL_H_
#define CELL_H_

#include "misc.h"

// A cell has five characteristics
// displayed in the following priority:
// foreground:
// 1) fire_t
// 2) mob_t
// 4) item_t
// 5) backdrop_t

class cell
{
	public:
		// init NIL values to all cell components
		cell();

		// return mob_t value of the cell
		mob_t getMob();

		// return item_t value of the cell
		item_t getItem();

		// return backdrop_t value of the cell
		backdrop_t getBackdrop();

		// return fire_t value of cell
		fire_t getFire();

		// get cell visiblity
		bool getv();

		// get cell visited flag
		bool getm();

		// set mob_t value of cell
		void setMob(mob_t);

		// set item_t value of cell
		void setItem(item_t);

		// set backdrop_t value of cell
		void setBackdrop(backdrop_t);

		// set fire_t value of cell
		void setFire(fire_t);

		// set visibility of cell
		void setv(bool);

		// set cell memory (was visited)
		void setm(bool);

		void setCurrentFireSymbol(chtype symbol);
		void setCurrentMobSymbol(chtype symbol);
		void setCurrentItemSymbol(chtype symbol);
		void setCurrentBackdropSymbol(chtype symbol);

		void setLastFireSymbol(chtype symbol);
		void setLastMobSymbol(chtype symbol);
		void setLastItemSymbol(chtype symbol);
		void setLastBackdropSymbol(chtype symbol);

		chtype getCurrentFireSymbol();
		chtype getCurrentMobSymbol();
		chtype getCurrentItemSymbol();
		chtype getCurrentBackdropSymbol();

		chtype getLastFireSymbol();
		chtype getLastMobSymbol();
		chtype getLastItemSymbol();
		chtype getLastBackdropSymbol();

		// save cell
        void save(std::ofstream &) const;

        // load cell
        void load(std::ifstream &);

	private:

		// fields for cell class (described above)
		fire_t fire;
		mob_t mob;
		item_t itm;
		backdrop_t backdrop;

		chtype currentFireSymbol;
		chtype currentMobSymbol;
		chtype currentItemSymbol;
		chtype currentBackdropSymbol;

		chtype lastFireSymbol;
		chtype lastMobSymbol;
		chtype lastItemSymbol;
		chtype lastBackdropSymbol;

		bool in_mem;

		bool is_vis;
};

#endif
