// input

#include "input.h"

input event_handler;

input::input()
{
    delta.set(0,0);
    action = INP_NONE;
}

void input::promptAction()
{
    //flushInput(); // <-- Add this
    action = INP_NONE;

    do
    {
        SDL_WaitEvent(&evt);
    } while (evt.type != SDL_KEYDOWN);
}

void input::setAction()
{
    promptAction();

    const SDL_Keycode key = evt.key.keysym.sym;

    switch(key)
    {
        case(SDLK_KP_7):
            setDelta(point(-1,-1));
            break;
        case(SDLK_KP_8):
            setDelta(point(0,-1));
            break;
        case(SDLK_KP_9):
            setDelta(point(1,-1));
            break;
        case(SDLK_KP_4):
            setDelta(point(-1,0));
            break;
        case(SDLK_KP_6):
            setDelta(point(1,0));
            break;
        case(SDLK_KP_1):
            setDelta(point(-1,1));
            break;
        case(SDLK_KP_2):
            setDelta(point(0,1));
            break;
        case(SDLK_KP_3):
            setDelta(point(1,1));
            break;
        case(SDLK_SPACE):
            action = INP_SELECT;
            break;
        case(SDLK_t):
            action = INP_WEAPONFIRE;
            break;
        case(SDLK_x):
            action = INP_EXAMINE;
            break;
        case(SDLK_TAB):
            action = INP_CHANGETABFOCUS;
            break;
        case(SDLK_q):
        {
            if (isShiftOrCaps())
            {
                action = INP_QUIT;
            }
            break;
        }
        case(SDLK_RETURN):
            action = INP_TOGGLE;
            break;
        case(SDLK_d):
            action = INP_DEBUG;
            break;
        case(SDLK_KP_5):
        case(SDLK_PERIOD):
            action = INP_WAIT;
            break;
        case(SDLK_w):
        {
            if (isShiftOrCaps())
            {
                action = INP_WAITSPECIFY;
            }
            break;
        }
        case(SDLK_s):
        {
            if (isShiftOrCaps())
            {
                action = INP_SAVE;
            }
            break;
        }
        case(SDLK_r):
        {
            action = INP_LOAD;
            break;
        }
        case(SDLK_y):
            action = INP_YES;
            break;
        case(SDLK_n):
            action = INP_NO;
            break;
        case(SDLK_ESCAPE):
            action = INP_ESCAPE;
            break;
        case(SDLK_BACKSPACE):
            action = INP_BACKSPACE;
            break;
        default:
            break;
    }
}

bool input::isShiftOrCaps()
{
    SDL_Keymod mod = SDL_GetModState();
    return (mod & KMOD_SHIFT) || (mod & KMOD_CAPS);
}


void input::setDelta(point p)
{
    action = INP_DELTA;
    delta.set(p.x(),p.y());
}

void input::waitForKey(char c)
{
    bool hit_space = false;

    while( hit_space == false )
    {
        if( SDL_WaitEvent( &evt) )
        {
            if( evt.type == SDL_KEYDOWN)
            {
                if( (int)evt.key.keysym.sym == (int)c)
                {
                    hit_space = true;
                }
            }
        }
    }
}

bool input::checkForKey(char c)
{
    bool hit_key = false;

    while( hit_key == false )
    {
        if( SDL_WaitEvent( &evt) )
        {
            if( evt.type == SDL_KEYDOWN)
            {
                hit_key = true;
            }
        }
    }

    return evt.key.keysym.sym == (int)c;
}

SDL_Keycode input::getKeyPressed()
{
    //flushInput(); // <-- Add this
    bool hit_key = false;

    while( hit_key == false )
    {
        if( SDL_WaitEvent( &evt) )
        {
            if( evt.type == SDL_KEYDOWN)
            {
                hit_key = true;
            }
        }
    }

    return evt.key.keysym.sym;
}

point input::getDelta()
{
    return delta;
}

input_t input::getAction()
{
    return action;
}

void input::flushInput()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        // discard all pending events
    }
}
