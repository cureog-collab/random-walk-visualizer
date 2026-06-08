#ifndef GRID_H
#define GRID_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>

#include <stdbool.h>

typedef struct {
    // each node on the grid will either be visted or unvisited
    bool *visited;

    // size and location of the grid
    SDL_Rect box;
} grid;

bool initGrid(grid *g, int width, int height);
void freeGrid(grid *g);

bool isVisited(const grid *g, const SDL_Point pos);
void markVisited(grid *g, const SDL_Point pos);

int gridHash(const SDL_Point pos, int width);

#endif