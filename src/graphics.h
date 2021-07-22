#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdint.h>

class graphics {
    
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    //The window we are rendering to
    SDL_Window* window = NULL;
    
    //The window surface
    SDL_Surface* screenSurface = NULL;
    
    //The window renderer
    SDL_Renderer* gRenderer = NULL;
    
    //The image on scree
    SDL_Surface* gameSurface = NULL;
    
    //Current displayed image
    SDL_Texture* gTexture = NULL;


    public:
    bool init();
    bool drawScreen(uint32_t* gfx);
    void close();
    
    //Event handler
    SDL_Event event;
};

bool graphics::init()
{
    //Initialization flag
    bool success = true;
    
    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL couldn't initialize, SDL Error: " << SDL_GetError() << endl;
        success = false;
    }
    else
    {
        window = SDL_CreateWindow( "Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        
        if(window == NULL)
        {
            cout << "Window could not be created, SDL Error: " << SDL_GetError() << endl;
            success = false;
        }
        else
        {
         
            //Create renderer
            gRenderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                cout << "Renderer could not be created " << SDL_GetError() << endl;
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                
                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !(IMG_Init( imgFlags ) & imgFlags ) )
                {
                    cout << "SDL image could not initialize " << IMG_GetError();
                    success = false;
                }
                
                //Get window surface
                screenSurface = SDL_GetWindowSurface(window);
            }
        }
    }
    
    return success;
}


bool graphics::drawScreen(uint32_t* gfx)
{
    bool success = true;
    gfx[2048/2 + 32] = 0;
    
    gameSurface = SDL_CreateRGBSurfaceFrom(gfx, 64, 32, 32, 64*4, 0, 0, 0, 0);
    
    if(gameSurface == NULL)
    {
        cout << "Could not create surface, SDL Error: " << SDL_GetError() << " fuck" << endl;
        success = false;
    }
    else
    {
        gTexture = SDL_CreateTextureFromSurface(gRenderer, gameSurface);
        
        //Clear Screen
        SDL_RenderClear( gRenderer );
        
        //Render texture to screen
        SDL_RenderCopy( gRenderer, gTexture, NULL, NULL);
        
        //Update Screen
        SDL_RenderPresent( gRenderer );
    }
    
    return success;
}

void graphics::close()
{
    //Free loaded images
    SDL_FreeSurface(gameSurface);
    gameSurface = NULL;
    
    //Destroy window
    SDL_DestroyWindow(window);
    window = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
