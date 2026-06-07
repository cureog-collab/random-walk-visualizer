#ifndef AGENT_H
#define AGENT_H

#include "camera_work.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    SDL_Point *pAgentPath;
    int stepsTaken;
    int currCapacity;
} randomWalkAgent;

bool initAgent(randomWalkAgent *agent, SDL_Point startPos);
bool updateAgentPos(randomWalkAgent *agent);
bool renderAgent(SDL_Renderer *renderer, const randomWalkAgent *agent, const camera *cam, const float AGENTSIZE);

#endif
