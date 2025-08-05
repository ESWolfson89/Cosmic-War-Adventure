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
	INP_YES,
	INP_NO,
	INP_SELECT,
	INP_WEAPONFIRE,
	INP_EXAMINE,
	INP_CHANGETABFOCUS,
	INP_TOGGLE,
	INP_DEBUG,
	INP_WAIT,
	INP_WAITSPECIFY,
	INP_ESCAPE,
	INP_BACKSPACE
};

class input
{

public:
	input();

	void setAction();

	void setDelta(point);

	void waitForKey(char);

	bool checkForKey(char);

	bool isShiftOrCaps();

	void promptAction();

	void flushInput();

	SDL_Keycode getKeyPressed();

	point getDelta();

	input_t getAction();

private:
	SDL_Event evt;
	point delta;
	input_t action;
	bool shiftFlag;

};

extern input event_handler;

#endif
