#include "agent.h"
#include "camera_work.h"
#include "text_handler.h"
#include "grid.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

const char *VALIDOPTS = "dbspglm";
const int AGENTSIZE = 4;
const int ORIGINALGRIDSIZE = 1000;
const int mainTabH = 900;
const int mainTabW = 1500;

int main(int argc, char *argv[])
{
    // funny variable to check if the user demands more than one model (which should be deemed illegal)
    int modelsCount = 0;

    // dark mode toggle
    bool darkMode = false;

    // blend mode toggle
    bool blendMode = false;

    // set default model of the walk
    walkModel currModel = MODE_PURE_WALK;

    // ============================= check flags =============================
    int opt;
    while ((opt = getopt(argc, argv, VALIDOPTS)) != -1) 
    {
        switch (opt) 
        {
            case 'd':
                darkMode = true;
                break;
            
            case 'b':
                blendMode = true;
                break;

            case 's':
                currModel = MODE_SAW;
                modelsCount++;
                break;
            
            case 'g':
                currModel = MODE_GAUSSIAN;
                modelsCount++;
                break;

            case 'l':
                currModel = MODE_LEVY;
                modelsCount++;
                break;
            
            case 'm':
                currModel = MODE_MEW;
                modelsCount++;
                break;

            case 'p':
                currModel = MODE_PEARSON;
                modelsCount++;
                break;

            case '?':
                printf("Invalid flag!\n");
                return 1;
        }
    }
    if (optind < argc)
    {
        printf("Error: unexpected argument '%s'!\n", argv[optind]);
        printf("Usage: %s [-d] [model-flag]\n", argv[0]);
        return 1;
    }

    if (modelsCount > 1)
    {
        printf("Error: can only simulate one model at a time\n");
        printf("Usage: %s [-d] [model-flag]\n", argv[0]);
        return 1;
    }
    
    // =============== initiate the SDL engine, open a window and prepare general stuff ===============
    SDL_Init(SDL_INIT_VIDEO);
    if (TTF_Init() < 0)
    {
        printf("Error: failed to call TTF_init()!\n");
        return 1;
    }
    TTF_Font *mainFont = TTF_OpenFont("Roboto-Regular.ttf", 24);
    if (mainFont == NULL)
    {
        printf("Error: failed to initiate main font!\n");
        return 1;
    }
    SDL_Window *mainTab = SDL_CreateWindow("random walk visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            mainTabW, mainTabH, 0);
    if (mainTab == NULL)
    {
        printf("Error: failed to generate main window!\n");
        return 1;
    }
    SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainTab, -1, SDL_RENDERER_PRESENTVSYNC);
    if (mainRenderer == NULL)
    {
        printf("Error: failed to generate mainRenderer!\n");
        SDL_DestroyWindow(mainTab);
        SDL_Quit();
        return 1;
    }
    if (blendMode)
    {
        SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
    }
    SDL_Texture *sidebarTexture = SDL_CreateTexture(mainRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET
                                                    , mainTabW / 5, mainTabH);
    if (sidebarTexture == NULL)
    {
        printf("Error: failed to generate sidebarTexture!\n");
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainTab);
        SDL_Quit();
        return 1;
    }
    
    SDL_Event mainEvent;

    srand(time(NULL));

    // ========================= initiate a single agent and its grid =========================
    grid agent0Grid;
    if (!initGrid(&agent0Grid, ORIGINALGRIDSIZE, ORIGINALGRIDSIZE))
    {
        printf("Error: failed to initiate a grid!\n");
        SDL_DestroyTexture(sidebarTexture);
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainTab);
        return 1;
    }
    randomWalkAgent agent0;
    SDL_Point startPos0 = {140, 100};
    if (!initAgent(&agent0, startPos0))
    {
        printf("Error: failed to initiate an agent!\n");
        freeGrid(&agent0Grid);
        SDL_DestroyTexture(sidebarTexture);
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainTab);
        return 1;
    }

    // =================== the main program loop ===================
    camera mainCam;
    SDL_Point origin = {0, 0};
    resetCamera(&mainCam, origin);
    bool isPausing = false;
    bool mainIsOn = true;
    while (mainIsOn)
    {
        // detect input from user
        while (SDL_PollEvent(&mainEvent))
        {
            switch (mainEvent.type) {
                // if the user click close, change isOn to false
                case SDL_QUIT:
                    mainIsOn = false;
                    break;

                case SDL_KEYDOWN:
                    // if the user click SPACE, reset camera settings
                    if (mainEvent.key.keysym.sym == SDLK_SPACE)
                    {
                        resetCamera(&mainCam, origin);
                    }

                    // if the user clicks "S", toggle the pause state
                    else if (mainEvent.key.keysym.sym == SDLK_s)
                    {
                        isPausing = !isPausing;
                    }

                    break;
            }

                // camera work
                updateViewport(&mainCam, &mainEvent);
        }

        // agent stuff
        if (!isPausing && !updateAgentPos(&agent0, currModel, &agent0Grid) && agent0.isAlive)
        {
            printf("Error: failed to realloc more memory for an agent's path!\n");
            freeGrid(&agent0Grid);
            SDL_DestroyTexture(sidebarTexture);
            SDL_DestroyRenderer(mainRenderer);
            SDL_DestroyWindow(mainTab);
            return 1;
        }

        // paint stuff
        SDL_RenderSetScale(mainRenderer, 1.0f, 1.0f);
        
        // background
        SDL_RenderSetViewport(mainRenderer, NULL);
        if (darkMode)
        {
            SDL_SetRenderDrawColor(mainRenderer, 10, 10, 10, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(mainRenderer, 240, 240, 240, 255);
        }

        // clear everything
        SDL_RenderClear(mainRenderer);

        // sidebar
        SDL_SetRenderTarget(mainRenderer, sidebarTexture);
        SDL_Rect sidebar = {
        0,
        0,
        mainTabW / 5,
        mainTabH
        };
        if (darkMode)
        {
            SDL_SetRenderDrawColor(mainRenderer, 40, 40, 40, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(mainRenderer, 200, 200, 200, 255);
        }
        SDL_RenderFillRect(mainRenderer, &sidebar);

        // sidebar text
        char stepText[64];
        char distText[64];
        int currentX = agent0.pAgentPath[agent0.stepsTaken].x;
        int currentY = agent0.pAgentPath[agent0.stepsTaken].y;
        double dist = hypot(currentX, currentY);
        if (!agent0.isAlive)
        {
            snprintf(stepText, sizeof(stepText), "Steps: %d (DEAD)", agent0.stepsTaken);
        }
        else if (!isPausing)
        {
            snprintf(stepText, sizeof(stepText), "Steps: %d", agent0.stepsTaken);
        }
        else
        {
            snprintf(stepText, sizeof(stepText), "Steps: %d (PAUSED)", agent0.stepsTaken);
        }
        snprintf(distText, sizeof(distText), "Dist to origin: %.2f", dist);
        SDL_Color textColor = darkMode ? (SDL_Color){220, 220, 220, 255} : (SDL_Color){50, 50, 50, 255};
        
        // steps taken
        if (!drawText(mainRenderer, mainFont, stepText, 20, 20, textColor))
        {
            printf("Error: failed to write stats onto sidebar!\n");
            return 1;
        }

        // distance to the origin
        if (!drawText(mainRenderer, mainFont, distText, 20, 50, textColor))
        {
            printf("Error: failed to write stats onto sidebar!\n");
            return 1;
        }

        // print the sidebarTexture onto the screen
        SDL_SetRenderTarget(mainRenderer, NULL);
        SDL_Rect sidebarDst = {0, 0, mainTabW / 5, mainTabH};
        SDL_RenderCopy(mainRenderer, sidebarTexture, NULL, &sidebarDst);

        // main background
        SDL_Rect mainCanvas = { mainTabW / 5, 0, mainTabW - mainTabW / 5, mainTabH };
        SDL_RenderSetViewport(mainRenderer, &mainCanvas);

        // agent0
        if (blendMode)
        {
            SDL_SetRenderDrawColor(mainRenderer, 255, 50, 50, 30);
        }
        else
        {
            SDL_SetRenderDrawColor(mainRenderer, 255, 50, 50, 255);
        }
        if (!renderAgent(mainRenderer, &agent0, &mainCam, AGENTSIZE))
        {
            printf("Error: failed to render agent and its path!\n");
            return 1;
        }

        // present the final stuff
        SDL_RenderPresent(mainRenderer);
    }

    // clean stuff and turn off the program
    freeGrid(&agent0Grid);
    free(agent0.pAgentPath);
    TTF_CloseFont(mainFont);
    TTF_Quit();
    SDL_DestroyTexture(sidebarTexture);
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainTab);
    SDL_Quit();
    return 0;
}