#include "agent.h"
#include "camera_work.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>

const int additionalCap = 500;

// create a randomwalk agent
bool initAgent(randomWalkAgent *agent, SDL_Point startPos)
{
    agent->currCapacity = 1000;
    agent->pAgentPath = malloc(agent->currCapacity * sizeof(SDL_Point));
    if (agent->pAgentPath == NULL)
    {
        return false;
    }
    agent->stepsTaken = 0;
    agent->pAgentPath[0] = startPos;
    return true;
}

// "walk" the agent at each delta time
bool updateAgentPos(randomWalkAgent *agent)
{
    // get the current position of the agent
    SDL_Point currPos = agent->pAgentPath[agent->stepsTaken];
    int currCap = agent->currCapacity;
    // generate a jump value for the agent and evaluate its next position accordingly
    SDL_Point jump = {0, 0};
    int direction = rand() % 4; 
    switch (direction)
    {
        case 0: jump.y = -1; break; // up
        case 1: jump.y =  1; break; // down
        case 2: jump.x = -1; break; // left
        case 3: jump.x =  1; break; // right
    }

    SDL_Point nextPos;
    nextPos.x = currPos.x + jump.x;
    nextPos.y = currPos.y + jump.y;

    // check if the capacity of the agent is still big enough
    if (agent->stepsTaken + 1 == currCap)
    {
        SDL_Point *tempPPath = realloc(agent->pAgentPath, (currCap + additionalCap) * sizeof(SDL_Point));
        if (tempPPath == NULL)
        {
            return false;
        }
        agent->pAgentPath = tempPPath;
        agent->currCapacity += additionalCap;
    }
    agent->pAgentPath[agent->stepsTaken + 1] = nextPos;
    agent->stepsTaken++;
    return true;
}

bool renderAgent(SDL_Renderer *renderer, const randomWalkAgent *agent, const camera *cam, const float AGENTSIZE)
{
    int totSteps = agent->stepsTaken;
    SDL_Point *tailPath = malloc((totSteps + 1) * sizeof(SDL_Point));
    if (tailPath == NULL)
    {
        printf("Error: failed to allocate memory for the tail!\n");
        return false;
    }

    for (int step = 0; step <= totSteps; step++)
    {
        SDL_Point stepPos = agent->pAgentPath[step];

        SDL_Point worldPt = {
            (int)(stepPos.x * AGENTSIZE + (AGENTSIZE / 2.0f)),
            (int)(stepPos.y * AGENTSIZE + (AGENTSIZE / 2.0f))
        };

        // re-position for camera settings
        tailPath[step] = cameraMorphPoint(cam, worldPt);
    }

    SDL_RenderDrawLines(renderer, tailPath, totSteps + 1);
    free(tailPath);


    // draw the agent head
    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    SDL_Point headLogicPos = agent->pAgentPath[totSteps];

    SDL_FRect worldHead = {
        headLogicPos.x * AGENTSIZE,
        headLogicPos.y * AGENTSIZE,
        AGENTSIZE,
        AGENTSIZE
    };

    // resize for camera settings
    SDL_Rect renderHead = cameraMorphRect(cam, worldHead);
    SDL_RenderFillRect(renderer, &renderHead);

    return true;
}