#include <iostream>
#include <cstring>
#include <string.h>
#include <stack>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

class chip8 {
    
    unsigned short opcode;
    unsigned char memory[4096];
    
    unsigned char V[16];
    
    unsigned short I;
    unsigned short pc;
        
    stack<unsigned short> programStack;
    
    unsigned char delay_timer;
    unsigned char sound_timer;
    
    unsigned char chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
      };
    
    public :
    void initialize();
    bool loadGame(string game);
    void emulateCycle();

    bool drawFlag;
    uint32_t gfx[64 * 32];
    unsigned char key[16];
};

void chip8::initialize(){
            
    memset(&opcode, 0, sizeof opcode);
    memset(memory, 0, sizeof memory);
    memset(V, 0, sizeof V);

    I = 0;
    pc = 0x200;
    
    memset(gfx, 0, sizeof gfx);
    memset(&delay_timer, 0, sizeof delay_timer);
    memset(&sound_timer, 0, sizeof sound_timer);
    memset(key, 0, sizeof key);
    
    for(int i = 0; i < 80; i++)
        memory[i] = chip8_fontset[i];
}

bool chip8::loadGame(string game){
        
    string line;
    string path = "/Users/keithrabbitte/Documents/Programming/stupid/Hello_World/c8games/" + game;
    
    cout << "Loading: " << path << endl;
        
    ifstream rom(path, ios::binary | ios::in);
    
    if(!rom.is_open())
    {
        cout << "could not open" << endl;
        return false;
    }
    
    char c;
    int j = 512;
    for(int i = 0x200; rom.get(c); i++)
    {
        if(j >= 4096)
        {
            cout << "couldn't read" << endl;
            return false;
        }
        memory[i] = (uint8_t)c;
        j++;
    }
    
    return true;
}

void chip8::emulateCycle(){
    opcode = (memory[pc] << 8) | memory[pc+1];
    
    pc += 2;
    
    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x00FF){
                case 0x00E0:
                    //clear screen
                    cout << "clearing screen " << pc << endl;
                    for(int i = 0; i < 64*32; i++)
                    {
                        gfx[i] = 0;
                    }
                    break;
                case 0x00EE:
                    //return from subroutine
                    cout << "subroutine " << pc << endl;
                    pc = programStack.top();
                    programStack.pop();
                    cout << "returning from subroutine " << pc << endl;
                    
                    break;
                default:
                    cout << "Unknown opcode [0x0000]:" << hex << opcode << endl;
                    break;
            }
            break;
        case 0x1000:
            //jump to line
            pc = opcode & 0x0FFF;
            cout << "jump instruction: " << hex << (opcode & 0xFFF) << endl;
            break;
        case 0x2000:
            //run subroutine
            cout << "calling subroutine: " << hex << pc << endl;
            programStack.push(pc);
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            //skip the next instruction
            cout << "skip instruction: 1 " << hex << pc << endl;
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                pc += 2;
            }
            break;
        case 0x4000:
            cout << "skip instruction: 2 " << hex << pc << endl;
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
                pc += 2;
            }
            break;
        case 0x5000:
            cout << "skip instruction: 3 " << hex << pc << endl;
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 2;
            }
            break;
        case 0x6000:
            cout << "assigning: " << hex << pc << endl;
            V[(opcode & 0x0F00) >> 8] = opcode & 0x0FF;
            break;
        case 0x7000:
            cout << "assigning: " << hex << pc << endl;
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;
        case 0x8000:
        {
            int vx = (opcode & 0x0F00) >> 8;
            int vy = (opcode & 0x00F0) >> 4;
            
            switch(opcode & 0x000F){
                case 0x0000:
                    V[vx] = V[vy];
                    break;
                case 0x0001:
                    V[vx] = V[vx] | V[vy];
                    break;
                case 0x0002:
                    V[vx] = V[vx] & V[vy];
                    break;
                case 0x0003:
                    V[vx] = V[vx] ^ V[vy];
                    break;
                case 0x0004:
                    V[15] = (V[vx] + V[vy] > 255 ? 1 : 0);
                    
                    V[vx] = (V[vx] + V[vy]) & 0x00FF;
                    break;
                case 0x0005:
                    V[15] = (V[vx] > V[vy] ? 1 : 0);

                    V[vx] = (V[vx] - V[vy]) & 0x00FF;
                    break;
                case 0x0006:
                    V[15] = (V[vx] & 0x0001);

                    V[vx] >>= 1;
                    break;
                case 0x0007:
                    V[15] = (V[vy] > V[vx] ? 1 : 0);
                    
                    V[vx] = (V[vy] - V[vx]) & 0x00FF;
                    break;
                case 0x000E:
                    V[15] = (((V[vx] & 0x80) == 0x80) ? 1 : 0);

                    V[vx] <<= 1;
                    break;
                default:
                    cout << "unknown opcode" << endl;
                }
            }
            cout << "one of the adding ones" << endl;
            break;
        case 0x9000:
            cout << "skipping instruction: " << hex << pc << endl;
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]){
                pc += 2;
            }
            break;
        case 0xA000:
            cout << "setting index and skipping: " << hex << pc << endl;
            I = opcode & 0x0FFF;
            break;
        case 0xB000:
            cout << "setting program counter to: " << hex << pc << endl;
            pc = V[0] + (opcode & 0x0FFF);
            break;
        case 0xC000:
            V[(opcode & 0x0F00) >> 8]  = (rand() % 256) & ((opcode & 0x00FF));
            cout << "setting to random" << hex << V[(opcode & 0x0F00) >> 8] << endl;
            break;
        case 0xD000:
            {
                int x = V[(opcode & 0x0F00) >> 8];
                int y = V[(opcode & 0x00F0) >> 4];
                int n = opcode & 0x000F;

                    V[15] = 0;

                    for (int i = 0; i < n; i++)
                    {
                        unsigned char mem = memory[I + i];

                        for (int j = 0; j < 8; j++)
                        {
                            unsigned char pixel = ((mem >> (7 - j)) & 0x01);
                            int index = x + j + (y + i) * 64;

                            if (index > 2047) continue;

                            if (pixel == 1 && gfx[index] != 0) V[15] = 1;

                            gfx[index] = (gfx[index] != 0 && pixel == 0) || (gfx[index] == 0 && pixel == 1) ? 0xffffffff : 0;
                        }
                    }
                                
                drawFlag = true;
                cout << "drawing sprite: " << " " << hex << pc << endl;
            }
            break;
        case 0xE000:
            switch(opcode & 0x00FF){
                case 0x009E:
                    cout << "skipping if key" << endl;
                    if(key[V[(opcode & 0x0F00) >> 8]])
                    {
                        pc += 2;
                    }
                    break;
                case 0x00A1:
                    cout << "skipping if not key" << endl;
                    if(!key[V[(opcode & 0x0F00) >> 8]])
                    {
                        pc += 2;
                    }
                    break;
                default:
                    cout << "unknown opcode" << endl;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    cout << "setting Vx to delay timer " << endl;
                    break;
                case 0x000A:
                    cout << "waiting for input";
                    cin >> V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x0015:
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    cout << "setting delay timer to Vx" << endl;
                    break;
                case 0x0018:
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    cout << "setting sound timer to Vx" << endl;
                    break;
                case 0x001E:
                    I += V[(opcode & 0x0F00) >> 8];
                    cout << "setting I to Vx" << endl;
                    break;
                case 0x0029:
                    I = V[(opcode & 0x0F00) >> 8] * 5;
                    cout << "setting I to sprite" << endl;
                    break;
                case 0x0033:
                    cout << "BCD crap" << endl;
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    break;
                case 0x0055:
                    cout << "reg dumping " << endl;
                    for(int i = 0; i < (opcode & 0x0F00) >> 8; i++)
                    {
                        memory[I + i] = V[i];
                    }
                    break;
                case 0x0065:
                    cout << "reg filling" << endl;
                    for(int i = 0; i < (opcode & 0x0F00) >> 8; i++)
                    {
                        V[i] = memory[I + i];
                    }
                    break;
            }
            break;
    }
        
    if (delay_timer > 0){
        --delay_timer;
    }
    
    if(sound_timer > 0){
        if(sound_timer == 1){
            printf("Beep!\n");
            --sound_timer;
        }
    }
}
