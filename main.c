#include "agent.h"
#include "text_handler.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <unistd.h>

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

    // for test: initiate a single randomwalk agent
    randomWalkAgent agent0;
    SDL_Point startPos0 = {900, 450};
    if (!initAgent(&agent0, startPos0))
    {
        printf("Error: failed to initiate an agent!\n");
        SDL_DestroyTexture(sidebarTexture);
        SDL_DestroyRenderer(mainRenderer);
        SDL_DestroyWindow(mainTab);
        return 1;
    }

    // the main program loop
    float zoom = 1.0f;
    float cameraX = 0;
    float cameraY = 0;
    bool isDragging = false;
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

                // if user hold 'ctrl' + scroll mouse, zoom accordingly
                case SDL_MOUSEWHEEL:
                    if (SDL_GetModState() & KMOD_CTRL)
                    {
                        if (mainEvent.wheel.y > 0)
                        {
                            zoom += 0.05f;
                        }
                        else if (mainEvent.wheel.y < 0)
                        {
                            zoom = (zoom == 0.1f) ? 0.1f : zoom - 0.05f;
                        }
                    }
                
                // camera panning
                case SDL_MOUSEBUTTONDOWN:
                    isDragging = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    isDragging = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (isDragging)
                    {
                        cameraX += mainEvent.motion.xrel;
                        cameraY += mainEvent.motion.yrel;
                    }
            }
        }

        // agent stuff
        if (!updateAgentPos(&agent0))
        {
            printf("Error: failed to realloc more memory for an agent's path!\n");
            SDL_DestroyTexture(sidebarTexture);
            SDL_DestroyRenderer(mainRenderer);
            SDL_DestroyWindow(mainTab);
            return 1;            
        }

        // paint stuff
        SDL_RenderSetScale(mainRenderer, 1.0f, 1.0f);
        // clear everything
        SDL_RenderSetViewport(mainRenderer, NULL);
        if (darkMode) {
            SDL_SetRenderDrawColor(mainRenderer, 10, 10, 10, 255);
        } else {
            SDL_SetRenderDrawColor(mainRenderer, 240, 240, 240, 255);
        }
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
        snprintf(stepText, sizeof(stepText), "Steps: %d", agent0.stepsTaken);
        SDL_Color textColor = darkMode ? (SDL_Color){255, 255, 255, 255} : (SDL_Color){0, 0, 0, 255};
        if (!drawText(mainRenderer, mainFont, stepText, 20, 20, textColor))
        {
            printf("Error: failed to write stats onto sidebar!\n");
            return 1;
        }

        // print the sidebarTexture onto the screen
        SDL_SetRenderTarget(mainRenderer, NULL);
        SDL_Rect sidebarDst = {0, 0, mainTabW / 5, mainTabH};
        SDL_RenderCopy(mainRenderer, sidebarTexture, NULL, &sidebarDst);

        // main background
        SDL_Rect mainCanvas = {
            mainTabW / 5,
            0,
            mainTabW - mainTabW / 5,
            mainTabH

        };
        SDL_RenderSetViewport(mainRenderer, &mainCanvas);
        SDL_RenderSetScale(mainRenderer, zoom, zoom);

        // agent0
        SDL_SetRenderDrawColor(mainRenderer, 255, 50, 50, 255);
        SDL_RenderDrawLines(mainRenderer, agent0.pAgentPath, agent0.stepsTaken + 1);

        // present the final stuff
        SDL_RenderPresent(mainRenderer);
    }

    // clean stuff and turn off the program
    free(agent0.pAgentPath);
    TTF_CloseFont(mainFont);
    TTF_Quit();
    SDL_DestroyTexture(sidebarTexture);
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainTab);
    SDL_Quit();
    return 0;
}