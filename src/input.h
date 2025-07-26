#ifndef INPUT_H_
#define INPUT_H_

#include "globals.h"
#include "point.h"

enum input_t
{
	INP_NONE,
	INP_DELTA,
	INP_QUIT,
	INP_SAVE,
	INP_LOAD,
	INP_SELECT,
	INP_WEAPONFIRE,
	INP_EXAMINE,
	INP_CHANGETABFOCUS,
	INP_TOGGLE,
	INP_DEBUG,
	INP_WAIT,
	INP_WAITSPECIFY,
};

class input
{

public:
	input();

	void setAction();

	void setDelta(point);

	void waitForKey(char);

	bool checkForKey(char);

	void clearInputBuffer(SDL_Event *);

	void promptAction();

	SDL_Keycode getKeyPressed();

	point getDelta();

	input_t getAction();

private:
	SDL_Event evt;
	point delta;
	input_t action;

};

extern input event_handler;

#endif
