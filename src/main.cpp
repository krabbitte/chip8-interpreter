#include <iostream>
#include "chip8.h"
#include <unistd.h>
#include "graphics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <chrono>

chip8 myChip8;
graphics SDL;

bool setUpGraphics();
void handleKeys(int keyFlag);

int main(int argc, const char * argv[]) {
    
    bool isquit = false;
        
    if(!setUpGraphics())
    {
        cout << "SDL setup failed. Exiting" << endl;
        return 0;
    }
    
    myChip8.initialize();
    myChip8.loadGame("PONG2");

    while(!isquit){
        
        usleep(2000);

        myChip8.emulateCycle();
         
        while(SDL_PollEvent(&SDL.event) != 0)
        {
            if(SDL.event.type == SDL_QUIT)
            {
                isquit = true;
            }
            else if(SDL.event.type == SDL_KEYUP || SDL.event.type == SDL_KEYDOWN)
            {
                handleKeys(SDL.event.type);
            }
        }
        
        if(myChip8.drawFlag)
        {
            SDL.drawScreen(myChip8.gfx);
            myChip8.drawFlag = false;
        }
        
    }
}

bool setUpGraphics()
{
    bool success = true;
    
    if(!SDL.init())
    {
        success = false;
    }
    
    return success;
}

void handleKeys(int keyFlag)
{
    bool downRelease = true;
    
    if (keyFlag == SDL_KEYUP)
    {
        downRelease = false;
    }
    else if(keyFlag == SDL_KEYDOWN)
    {
        downRelease = true;
    }
    
    (downRelease) ? cout << "YESSSS!!!!" << endl : cout << "NOooooooOOO!!!!" << endl << endl;
    
    switch(SDL.event.key.keysym.sym)
    {
        case SDLK_UP:
            myChip8.key[0] = downRelease;
            break;
        case SDLK_DOWN:
            myChip8.key[1] = downRelease;
            break;
        case SDLK_LEFT:
            myChip8.key[2] = downRelease;
            break;
        case SDLK_RIGHT:
            myChip8.key[3] = downRelease;
            break;
        case SDLK_q:
            myChip8.key[4] = downRelease;
            break;
        case SDLK_w:
            myChip8.key[5] = downRelease;
            break;
        case SDLK_e:
            myChip8.key[6] = downRelease;
            break;
        case SDLK_r:
            myChip8.key[7] = downRelease;
            break;
        case SDLK_a:
            myChip8.key[8] = downRelease;
            break;
        case SDLK_s:
            myChip8.key[9] = downRelease;
            break;
        case SDLK_d:
            myChip8.key[10] = downRelease;
            break;
        case SDLK_f:
            myChip8.key[11] = downRelease;
            break;
        case SDLK_z:
            myChip8.key[12] = downRelease;
            break;
        case SDLK_x:
            myChip8.key[13] = downRelease;
            break;
        case SDLK_c:
            myChip8.key[14] = downRelease;
            break;
        case SDLK_v:
            myChip8.key[15] = downRelease;
            break;
    }
}
