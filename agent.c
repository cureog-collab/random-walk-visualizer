#include "agent.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
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
    SDL_Point jump;
    int tempX = rand() % 3 - 1;
    jump.x = tempX;
    if (tempX != 0)
    {
        jump.y = 0;
    }
    else
    {
        jump.y = (rand() % 2 == 0) ? -1 : 1;
    }
    SDL_Point nextPos;
    nextPos.x = currPos.x + tempX;
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