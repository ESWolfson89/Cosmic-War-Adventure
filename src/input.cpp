// input

#include "input.h"

input::input()
{
    delta.set(0,0);
    action = INP_NONE;
}

void input::promptAction()
{
    action = INP_NONE;

    do
    {
        SDL_WaitEvent(&evt);
    } while (evt.type != SDL_KEYDOWN);
}

void input::setAction()
{
    promptAction();

    switch(evt.key.keysym.sym)
    {
    case(SDLK_y):
    case(SDLK_KP_7):
        setDelta(point(-1,-1));
        break;
    case(SDLK_k):
    case(SDLK_KP_8):
    case(SDLK_UP):
        setDelta(point(0,-1));
        break;
    case(SDLK_u):
    case(SDLK_KP_9):
        setDelta(point(1,-1));
        break;
    case(SDLK_h):
    case(SDLK_KP_4):
    case(SDLK_LEFT):
        setDelta(point(-1,0));
        break;
    case(SDLK_l):
    case(SDLK_KP_6):
    case(SDLK_RIGHT):
        setDelta(point(1,0));
        break;
    case(SDLK_b):
    case(SDLK_KP_1):
        setDelta(point(-1,1));
        break;
    case(SDLK_j):
    case(SDLK_KP_2):
    case(SDLK_DOWN):
        setDelta(point(0,1));
        break;
    case(SDLK_n):
    case(SDLK_KP_3):
        setDelta(point(1,1));
        break;
    case(SDLK_SPACE):
        action = INP_SELECT;
        break;
    case(SDLK_t):
        action = INP_WEAPONFIRE;
        break;
    case(SDLK_TAB):
        action = INP_CHANGETABFOCUS;
        break;
    case(SDLK_q):
        action = INP_QUIT;
        break;
    case(SDLK_p):
        action = INP_SAVE;
        break;
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
    case(SDLK_0):
        action = INP_WAITSPECIFY;
        break;
    default:
        break;
    }
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
