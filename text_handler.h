#ifndef TEXT_HANDLER_H
#define TEXT_HANDLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

bool drawText(SDL_Renderer *renderer, TTF_Font *font, const char *inputTxt, int x, int y, SDL_Color color);

#endif