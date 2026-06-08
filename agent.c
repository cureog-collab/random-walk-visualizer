#include "agent.h"
#include "camera_work.h"
#include "grid.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>

void chooseRandomDir(SDL_Point *jump);

const int POSSIBLEJUMPS = 4;
const int ADDITIONALCAP = 500;

// create a randomwalk agent
bool initAgent(randomWalkAgent *agent, SDL_Point startPos)
{
    agent->isAlive = true;
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
bool updateAgentPos(randomWalkAgent *agent, const walkModel currModel, grid *agentGrid)
{
    // check if the agent is still alive
    if (!agent->isAlive)
    {
        return false;
    }

    // get the current position of the agent
    SDL_Point currPos = agent->pAgentPath[agent->stepsTaken];
    int currCap = agent->currCapacity;
    // generate a jump value for the agent and evaluate its next position accordingly
    SDL_Point jump = {0, 0};
    SDL_Point nextPos;

    switch (currModel)
    {
        case MODE_PURE_WALK:
            chooseRandomDir(&jump);
            nextPos.x = currPos.x + jump.x;
            nextPos.y = currPos.y + jump.y;
            break;
        case MODE_SAW:
        {
            SDL_Point possibleJumps[] = {
                {0, -1},
                {0, 1},
                {-1, 0},
                {1, 0}
            };
            SDL_Point safeJumps[POSSIBLEJUMPS];
            uint safeCount = 0;

            // scan possible jumps to see if the agent is dead
            for (int i = 0; i < POSSIBLEJUMPS; i++)
            {
                SDL_Point virtualNextPos = {
                    currPos.x + possibleJumps[i].x,
                    currPos.y + possibleJumps[i].y                    
                };

                // if a possible jump is deemed unvisited
                if (!isVisited(agentGrid, virtualNextPos))
                {
                    safeJumps[safeCount] = possibleJumps[i];
                    safeCount++;
                }
            }

            // choose the correct action to do
            if (safeCount > 0)
            {
                jump = safeJumps[rand() % safeCount];
                nextPos.x = currPos.x + jump.x;
                nextPos.y = currPos.y + jump.y;
                markVisited(agentGrid, nextPos);
            }
            else
            {
                agent->isAlive = false;
                return false;
            }
            break;
        }
        case MODE_GAUSSIAN:
            // TODO
            printf("GAUSSIAN\n");
            break;
        case MODE_LEVY:
            // TODO
            printf("LEVY\n");
            break;
        case MODE_MEW:
            // TODO
            printf("MEW\n");
            break;
    }

    // check if the capacity of the agent is still big enough
    if (agent->stepsTaken + 1 == currCap)
    {
        SDL_Point *tempPPath = realloc(agent->pAgentPath, (currCap + ADDITIONALCAP) * sizeof(SDL_Point));
        if (tempPPath == NULL)
        {
            return false;
        }
        agent->pAgentPath = tempPPath;
        agent->currCapacity += ADDITIONALCAP;
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
    if (agent->isAlive)
    {
        SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 110, 110, 110, 255);
    }
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

// choose the direction randomly
void chooseRandomDir(SDL_Point *jump)
{
    int direction = rand() % 4; 
    switch (direction)
    {
        // up
        case 0:
            jump->y = -1;
            break;

        // down
        case 1:
            jump->y =  1;
            break;

        // left
        case 2:
            jump->x = -1;
            break;

        // right
        case 3:
            jump->x =  1;
            break;
    }
}