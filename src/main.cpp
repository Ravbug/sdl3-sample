#include <iostream>
#include <SDL.h>
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
//On UWP, we need to not have SDL_main otherwise we'll get a linker error
#define SDL_MAIN_HANDLED
#endif
#include <SDL_main.h>
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

void SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    exit(1);
}

static bool app_quit = false;

void main_loop() {
    // Get events. If you are making a game, you probably want SDL_PollEvent instead of SDL_WaitEvent.
    // you cannot use WaitEvent on Emscripten, because you cannot block the main thread there.

#if __EMSCRIPTEN__
    #define SDL_GetEvent SDL_PollEvent
#else
    #define SDL_GetEvent SDL_WaitEvent
#endif

    SDL_Event event;
    while (SDL_GetEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            app_quit = true;
        break;
    }
}

// Note: your main function __must__ take this form, otherwise on nonstandard platforms (iOS, etc), your app will not launch.
int main(int argc, char* argv[]){
    
    // init the library, here we make a window so we only need the Video capabilities.
    if (SDL_Init(SDL_INIT_VIDEO)){
        SDL_Fail();
    }
    
    // create a window
    SDL_Window* window = SDL_CreateWindow("Window", 352, 430, SDL_WINDOW_RESIZABLE);
    if (!window){
        SDL_Fail();
    }
    
    // print some information about the window
    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }
    
    SDL_Log("Application started successfully!");
    
#if __EMSCRIPTEN__
    // on Emscripten, we cannot have an infinite loop in main. Instead, we must
    // tell emscripten to call our main loop.
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (!app_quit) {
        main_loop();
    }
#endif

    // cleanup everything at the end
#if !__EMSCRIPTEN__
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_Log("Application quit successfully!");
#endif
    return 0;
}
