#include "text_handler.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

bool drawText(SDL_Renderer *renderer, TTF_Font *font, const char *inputTxt, int x, int y, SDL_Color color)
{
    // quit the function if the input text is nil
    if (inputTxt == NULL || inputTxt[0] == '\0')
    {
        return false;
    }

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, inputTxt, color);
    if (textSurface == NULL)
    {
        return false;
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL)
    {
        SDL_FreeSurface(textSurface);
        return false;
    }
    SDL_Rect textPos = {
        x,
        y,
        textSurface->w,
        textSurface->h
    };

    // write the final text onto the screen
    SDL_RenderCopy(renderer, textTexture, NULL, &textPos);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return true;
}