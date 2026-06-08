#include "grid.h"

#include <SDL2/SDL_rect.h>
#include <stdlib.h>

bool initGrid(grid *g, int width, int height)
{
    g->visited = calloc(width * height, sizeof(bool));
    if (g->visited == NULL)
    {
        // printf("Error: failed to initiate grid\n");
        return false;
    }
    g->box.w = width;
    g->box.h = height;
    g->box.x = width / 2;
    g->box.y = height / 2;
    return true;
}

void freeGrid(grid *g)
{
    if (g->visited != NULL)
    {
        free(g->visited);
        g->visited = NULL;
    }
}

bool isVisited(const grid *g, const SDL_Point pos)
{
    // calculate the pos on grid
    SDL_Point gridPos= {
        pos.x + g->box.x,
        pos.y + g->box.y
    };

    // check if the particle is still confined within the grid
    if (gridPos.x >= g->box.w || gridPos.y >= g->box.h || gridPos.x < 0 || gridPos.y < 0)
    {
        return true;
    }

    // if the particle is still confined, check the grid->visited table
    int gridIndex = gridHash(gridPos, g->box.w);
    return g->visited[gridIndex];
}
void markVisited(grid *g, const SDL_Point pos)
{
    // calculate the pos on grid
    SDL_Point gridPos= {
        pos.x + g->box.x,
        pos.y + g->box.y
    };
    
    // check if the particle is still confined within the grid
    if (gridPos.x >= g->box.w || gridPos.y >= g->box.h || gridPos.x < 0 || gridPos.y < 0)
    {
        return;
    }

    // if the particle is still confined, mark the node as visited
    int gridIndex = gridHash(gridPos, g->box.w);
    g->visited[gridIndex] = true;
}

int gridHash(const SDL_Point pos, int width)
{
    return (pos.y * width + pos.x);
}