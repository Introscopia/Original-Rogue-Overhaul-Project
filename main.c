#include "basics.h"
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_ttf.h>
#include "SDL2_gfx/SDL2_framerate.h"

#ifdef WIN32
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif


Uint32 time_passed = 0;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~O~~~~~~~~~~| M A I N |~~~~~~~~~~~O~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char *argv[]){

    //HWND hwnd_win = GetConsoleWindow();
    //ShowWindow(hwnd_win,SW_HIDE);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    int width = 640;
    int height = 480;
    bool loop = 1;

    Uint32 then, now, frames;
    FPSmanager fpsm;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }
    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }
    //SDL_MaximizeWindow( window );
    SDL_SetWindowTitle( window, "Rogue Overhaul" );
    SDL_GetWindowSize( window, &width, &height );

    srand (time(NULL));// prime the random number generator

    IMG_Init(IMG_INIT_PNG);

    const SDL_Color black = {0, 0, 0, 255};
    const SDL_Color white = {255, 255, 255, 255};
    //Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif


        


    
    SDL_initFramerate(&fpsm);
    SDL_setFramerate(&fpsm, 60);
    puts("<<Entering Loop>>");
    while ( loop ) { //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
        while( SDL_PollEvent(&event) ){
            switch (event.type) {
                case SDL_QUIT:
                    loop = 0;
                    break;
                case SDL_KEYDOWN:
                    //     if( event.key.keysym.sym == 'w' || event.key.keysym.sym == SDLK_UP )    
                    break;
                case SDL_KEYUP:
                         
                    break;
                case SDL_MOUSEMOTION:
                    //mouseX = event.motion.x;
                    //mouseY = event.motion.y;
                    /*if( SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE) ){
                        tx += event.motion.xrel;
                        ty += event.motion.yrel;
                    }*/
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_MOUSEBUTTONUP:
                    break;
                case SDL_MOUSEWHEEL:;
                    
                    break;
            }
        }
        SDL_SetRenderDrawColor( renderer, 20, 20, 20, 255);
        SDL_RenderClear( renderer );



        

        SDL_RenderPresent(renderer);
        //printf("%d\n", time_passed );
        time_passed = SDL_framerateDelay(&fpsm);

    }//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> / L O O P <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}