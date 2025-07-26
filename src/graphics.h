#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "globals.h"
#include "point.h"

class gfx_engine
{
    public:

        gfx_engine();

        ~gfx_engine();

        bool initSDL();

        void freeSDL();

        void addBitmapCharacter(chtype,point);

        void addBitmapString(std::string,color_pair,point);

        void addBitmapVerticalString(std::string,color_pair,point);

        void clearScreen();

        void updateScreen();

        void drawRectangle(color_type,point,point,bool);

        void renderTargetGE();

    private:

        SDL_Window *screen;

        SDL_Renderer *renderer;

        SDL_Texture *text_bitmap;

        SDL_Texture *renderTarget;
};

color_type getDimmedColor(color_type, Uint8, Uint8);

color_type getAnyColor(Uint8 r, Uint8 g, Uint8 b);

color_pair getInvertedColorpair(color_pair);

bool isColor(color_type, color_type);

extern gfx_engine gfx_obj;

#endif
