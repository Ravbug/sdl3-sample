#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <cmath>
#include <string_view>
#include <filesystem>

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* messageTex;
    SDL_FRect messageDest;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};

SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // init the library, here we make a window so we only need the Video capabilities.
    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        return SDL_Fail();
    }
    
    // init TTF
    if (not TTF_Init()) {
        return SDL_Fail();
    }
    
    // create a window
    SDL_Window* window = SDL_CreateWindow("Window", 352, 430, SDL_WINDOW_RESIZABLE);
    if (not window){
        return SDL_Fail();
    }
    
    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (not renderer){
        return SDL_Fail();
    }
    
    // load the font
#if __ANDROID__
    std::filesystem::path basePath = "";   // on Android we do not want to use basepath. Instead, assets are available at the root directory.
#else
    auto basePathPtr = SDL_GetBasePath();
     if (not basePathPtr){
        return SDL_Fail();
    }
     const std::filesystem::path basePath = basePathPtr;
#endif

    const auto fontPath = basePath / "Inter-VariableFont.ttf";
    TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), 36);
    if (not font) {
        return SDL_Fail();
    }

    // render the font to a surface
    const std::string_view text = "Hello SDL!";
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.data(), text.length(), { 255,255,255 });

    // make a texture from the surface
    SDL_Texture* messageTex = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    // get the on-screen dimensions of the text. this is necessary for rendering it
    auto texprops = SDL_GetTextureProperties(messageTex);
    SDL_FRect text_rect{
            .x = 0,
            .y = 0,
            .w = float(SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
            .h = float(SDL_GetNumberProperty(texprops, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))
    };

    // init SDL Mixer
    auto audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (not audioDevice) {
        return SDL_Fail();
    }
    if (not Mix_OpenAudio(audioDevice, NULL)) {
        return SDL_Fail();
    }

    // load the music
    auto musicPath = basePath / "the_entertainer.ogg";
    auto music = Mix_LoadMUS(musicPath.string().c_str());
    if (not music) {
        return SDL_Fail();
    }

    // play the music (does not loop)
    Mix_PlayMusic(music, 0);
    
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

    // set up the application data
    *appstate = new AppContext{
       .window = window,
       .renderer = renderer,
       .messageTex = messageTex,
       .messageDest = text_rect
    };
    
    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
    auto* app = (AppContext*)appstate;
    
    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* app = (AppContext*)appstate;

    // draw a color
    auto time = SDL_GetTicks() / 1000.f;
    auto red = (std::sin(time) + 1) / 2.0 * 255;
    auto green = (std::sin(time / 2) + 1) / 2.0 * 255;
    auto blue = (std::sin(time) * 2 + 1) / 2.0 * 255;
    
    SDL_SetRenderDrawColor(app->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);
    SDL_RenderTexture(app->renderer, app->messageTex, NULL, &app->messageDest);
    SDL_RenderPresent(app->renderer);


    return app->app_quit;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (AppContext*)appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    SDL_Log("Application quit successfully!");
}
