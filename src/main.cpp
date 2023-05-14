#include <iostream>
#include <SDL.h>
#include <SDL_main.h>

void SDL_Fail(){
    std::cerr << SDL_GetError() << std::endl;
    exit(1);
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
        std::cout << "Window size:\t\t" << width << " x " << height << "\nBackbuffer size:\t" << bbwidth << " x " << bbheight << "\n";
        if (width != bbwidth){
            std::cout << "This is a highdpi environment.\n";
        }
    }
    
    std::cout << "Application started successfully!" << std::endl;
    
    // Get events. If you are making a game, you probably want SDL_PollEvent instead of SDL_WaitEvent.
    SDL_Event event;
    bool quit = false;
    while (!quit){
        while (SDL_WaitEvent(&event)){
            if (event.type == SDL_EVENT_QUIT)
                quit = true;
                break;
        }
    }

    // cleanup everything at the end
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Application quit successfully!" << std::endl;
    return 0;
}
