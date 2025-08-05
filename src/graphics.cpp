#include "graphics.h"

gfx_engine gfx_obj;

gfx_engine::gfx_engine()
{
    screen = NULL;
    renderer = NULL;
    text_bitmap = NULL;
}

gfx_engine::~gfx_engine()
{
    freeSDL();
}

// initialize SDL2
bool gfx_engine::initSDL()
{
    // initialize all of SDL2's utilities/mechanisms etc...
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        return false;
    }

    // initialize SDL_Window instance screen
    screen = SDL_CreateWindow("Cosmic War Adventure", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREENWID*3, SCREENHGT*3, SDL_WINDOW_SHOWN);

    if (screen == NULL)
    {
        return false;
    }

    // initialize the renderer
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == NULL)
    {
        return false;
    }
    /*
    renderTarget = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREENWID, SCREENHGT);

    if (renderTarget == NULL)
    {
        return false;
    }
    */
    SDL_RenderSetScale(renderer, 3.0f, 3.0f);

    text_bitmap = IMG_LoadTexture(renderer, "8x8font.png");
    std::cout << SDL_GetError() << std::endl;

    if (text_bitmap == NULL)
    {
        std::cout << "not load font";
        return false;
    }

    //SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN);

    return true;
}


// free/destroy graphics ptr objects
void gfx_engine::freeSDL()
{
    if (text_bitmap != NULL)
    {
        SDL_DestroyTexture(text_bitmap);
        text_bitmap = NULL;
    }

    if (renderTarget != NULL)
    {
        SDL_DestroyTexture(renderTarget);
        renderTarget = NULL;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (screen != NULL)
    {
        SDL_DestroyWindow(screen);
        screen = NULL;
    }

    std::cout << "In destructor\n";

    SDL_Quit();
}

// print a string (sval) to the screen one bitmap character at a time
// at location (x,y) with color col.
void gfx_engine::addBitmapString(std::string string_val, color_pair colp, point p)
{
    chtype ct;

    for (int i = 0; i < (int)string_val.size(); ++i)
    {
        ct.ascii = static_cast<unsigned char>(string_val[i]);
        ct.color = colp;
        addBitmapCharacter(ct,point(p.x()+i,p.y()));
    }
}

void gfx_engine::addBitmapVerticalString(std::string string_val, color_pair colp, point p)
{
    chtype ct;

    for (int i = 0; i < (int)string_val.size(); ++i)
    {
        ct.ascii = static_cast<unsigned char>(string_val[i]);
        ct.color = colp;
        addBitmapCharacter(ct,point(p.x(),p.y()+i));
    }
}

// render portion of font_bitmap to screen with color col
void gfx_engine::addBitmapCharacter(chtype ch, point p)
{
    // boundary checking
    if (ch.ascii < 0 || ch.ascii > 255)
    {
        return;
    }

    SDL_Rect loc;
    SDL_Rect crop;

    loc.x = p.x() * TILEWID;
    loc.y = p.y() * TILEHGT;

    // crop out bitmap tile given aval, TILEWID, and TILEHGT
    // BITMAPROWS and BITMAPCOLS should stay at 16
    crop.x = (ch.ascii % BITMAPCOLS) * TILEWID;
    crop.y = (int)(ch.ascii / BITMAPCOLS) * TILEHGT;

    crop.w = loc.w = TILEWID;
    crop.h = loc.h = TILEHGT;

    renderTargetGE();

    //Render background highlight to screen (via SDL2 rectangle function)
    SDL_SetRenderDrawColor(renderer,ch.color.bg.r,ch.color.bg.g,ch.color.bg.b,0);
    SDL_RenderFillRect(renderer,&loc);

    //Render foreground character to screen (via parsing the bitmap font)
    SDL_SetTextureColorMod(text_bitmap,ch.color.fg.r,ch.color.fg.g,ch.color.fg.b);
    SDL_RenderCopy(renderer,text_bitmap,&crop,&loc);
}

void gfx_engine::drawRectangle(color_type c, point loc, point area, bool filled)
{
    renderTargetGE();

    SDL_Rect rect = {(int)loc.x(),(int)loc.y(),(int)area.x(),(int)area.y()};
    SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,0);

    if (filled)
    {
        SDL_RenderFillRect(renderer, &rect);
    }
    else
    {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

// clear screen to black
void gfx_engine::clearScreen()
{
    //renderTargetGE();
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
}

// update display
void gfx_engine::updateScreen()
{
    //SDL_SetRenderTarget(renderer, NULL);
    //SDL_RenderCopy(renderer, renderTarget, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void gfx_engine::renderTargetGE()
{
    //SDL_SetRenderTarget(renderer, renderTarget);
}

color_type getDimmedColor(color_type c, Uint8 f, Uint8 o)
{
    if (f == 0)
    {
        return c;
    }

    c.r = (Uint8)(c.r / f) + o;
    c.g = (Uint8)(c.g / f) + o;
    c.b = (Uint8)(c.b / f) + o;

    return c;
}

color_type getAnyColor(Uint8 r, Uint8 g, Uint8 b)
{
    color_type c = {r,g,b};
    return c;
}

color_pair getInvertedColorpair(color_pair col_p)
{
    color_pair ret_val;
    ret_val.fg = col_p.bg;
    ret_val.bg = col_p.fg;
    return ret_val;
}

bool isColor(color_type c1, color_type c2)
{
    return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

