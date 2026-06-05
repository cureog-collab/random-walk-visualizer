#include "agent.h"
#include "text_handler.h"
#include <stdio.h>

const char *VALIDOPTS = "i";
const int mainTabH = 900;
const int mainTabW = 1500;

int main(int argc, char *argv[])
{

    // dark mode toggle for the user
    bool darkMode = true;
    int opt;
    while ((opt = getopt(argc, argv, VALIDOPTS)) != -1) 
    {
        switch (opt) 
        {
            case 'i':
                darkMode = false;
                break;
            // the code is a bit chunky, but it's convenient for further expansion
            case '?':
                printf("Invalid flag!\n");
                return 1;
        }
    }
    if (optind < argc)
    {
        printf("Error: unexpected argument '%s'!\n", argv[optind]);
        printf("Usage: %s [-i]\n", argv[0]);
        return 1;
    }
    
    // initiate the SDL engine, open a window and prepare general stuff
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

    SDL_Window *mainTab = SDL_CreateWindow("random walk engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            mainTabW, mainTabH, 0);
    if (mainTab == NULL)
    {
        printf("Error: failed to generate main window!\n");
        return 1;
    }
    SDL_Renderer *mainRenderer = SDL_CreateRenderer(mainTab, -1, SDL_RENDERER_ACCELERATED);
    if (mainRenderer == NULL)
    {
        printf("Error: failed to generate mainRenderer!\n");
        SDL_DestroyWindow(mainTab);
        SDL_Quit();
        return 1;
    }
    SDL_Event mainEvent;

    // for test: initiate a single randomwalk agent
    randomWalkAgent agent0;
    SDL_Point startPos0 = {900, 450};
    if (!initAgent(&agent0, startPos0))
    {
        printf("Error: failed to initiate an agent!\n");
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainTab);
        return 1;
    }

    bool mainIsOn = true;
    while (mainIsOn)
    {
        // detect input from user
        while (SDL_PollEvent(&mainEvent))
        {
            // if the user click close, change isOn to false
            if (mainEvent.type == SDL_QUIT)
            {
                mainIsOn = false;
            }
        }

        // TODO: main functions
        if (!updateAgentPos(&agent0))
        {
            printf("Error: failed to realloc more memory for an agent's path!\n");
            SDL_DestroyRenderer(mainRenderer);
            SDL_DestroyWindow(mainTab);
            return 1;            
        }

        // paint stuff
        // main background
        if (darkMode)
        {
            SDL_SetRenderDrawColor(mainRenderer, 10, 10, 10, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(mainRenderer, 240, 240, 240, 255);
        }
        // main sidebar
        SDL_RenderClear(mainRenderer);
        SDL_Rect sidebar;
        sidebar.x = 0;
        sidebar.y = 0;
        sidebar.h = mainTabH;
        sidebar.w = mainTabW / 5;
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
        snprintf(stepText, sizeof(stepText), "Steps: %d", agent0.stepsTaken);
        SDL_Color textColor = darkMode ? (SDL_Color){255, 255, 255, 255} : (SDL_Color){0, 0, 0, 255};
        drawText(mainRenderer, mainFont, stepText, 20, 20, textColor);
        // agent
        SDL_SetRenderDrawColor(mainRenderer, 255, 50, 50, 255);
        SDL_RenderDrawLines(mainRenderer, agent0.pAgentPath, agent0.stepsTaken + 1);
        SDL_RenderPresent(mainRenderer);
    }
    // clean stuff and turn off the program
    free(agent0.pAgentPath);
    TTF_CloseFont(mainFont);
    TTF_Quit();
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainTab);
    SDL_Quit();
    return 0;
}