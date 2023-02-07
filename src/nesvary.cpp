#include <memory>
#include <iostream>
#include <array>
#include <functional>
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include "ricoh2a03.hpp"
#include "nesvary.hpp"

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

using namespace std;

//array<uint8_t, 309> snake_code = {{
vector<uint8_t> snake_code = {{
    0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85,
    0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85,
    0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe,
    0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3,
    0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38, 0x06, 0xa5, 0xff, 0xc9,
    0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0, 0x1b, 0xc9, 0x61, 0xf0, 0x22, 0x60,
    0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0,
    0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02,
    0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06,
    0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07,
    0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06,
    0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c,
    0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02,
    0x4a, 0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9,
    0x20, 0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
    0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0,
    0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29,
    0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe, 0x91, 0x00, 0x60,
    0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
    0xea, 0xca, 0xd0, 0xfb, 0x60
}};

const char* nesvary_version()
{
    return EXPAND_AND_QUOTE(PROJECT_VERSION);
}

SDL_Color get_color(uint8_t byte) {
    SDL_Color color;
   
    switch(byte) {
        case 0:
            color = {0, 0, 0}; // black
            break;
        case 1:
            color = {255, 255, 255}; // white
            break;
        case 2:
        case 9:
            color = {192, 192, 192}; // grey
            break;
        case 3:
        case 10:
            color = {255, 0, 0}; // red
            break;
        case 4:
        case 11:
            color = {0, 255, 0}; // green
            break;
        case 5:
        case 12:
            color = {0, 0, 255}; // blue
            break;
        case 6:
        case 13:
            color = {255, 0, 255}; // magenta
            break;
        case 7:
        case 14:
            color = {255, 255, 0}; // yellow
            break;
        default:
            color = {0, 255, 255}; // cyan
            break;
    }
    return color;
}


bool read_screen_state(shared_ptr<Ricoh2a03> cpu, vector<uint8_t>& frame) {
    int frame_idx = 0;
    bool update = false;

    for(int i = 0x200; i < 0x600 + 1; ++i) {
        uint8_t color_idx = cpu->mem_read((uint16_t)i);
        SDL_Color color = get_color(color_idx);
        if(frame[frame_idx] != color.r ||
                frame[frame_idx + 1] != color.b ||
                frame[frame_idx + 2] != color.g) {
            frame[frame_idx] = color.r;
            frame[frame_idx + 1] = color.b;
            frame[frame_idx + 2] = color.g;
            update = true;
        }
        frame_idx += 3;
    }
    return update;
}

void handle_user_input(shared_ptr<Ricoh2a03> cpu) {
    SDL_Event event;
    SDL_PumpEvents();
    while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_WINDOWEVENT, SDL_KEYDOWN)) {

        switch(event.type) {
            case SDL_QUIT:
                exit(-1);
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_w:
                        cpu->mem_write(0xFF, 0x77);
                        break;
                    case SDLK_a:
                        cpu->mem_write(0xFF, 0x73);
                        break;
                    case SDLK_s:
                        cpu->mem_write(0xFF, 0x61);
                        break;
                    case SDLK_d:
                        cpu->mem_write(0xFF, 0x64);
                        break;
                    default:
                        continue;
                }
                break;
            case SDL_KEYUP:
                // do nothing on key released
                break;
            default:
                // unhandled
                continue;

        }
    }
}

int main(int argc, char *argv[]) {
    using namespace std;
    // Print version before doing anything else.
    cout << nesvary_version() << endl;

    // SDL2 stuff
    if(SDL_Init(SDL_INIT_VIDEO)) {
        cout << "SDL could not be initialized" << endl;
        cout << "SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake", 0, 0, 640, 640, SDL_WINDOW_SHOWN);
    if(!window) {
        cout << "Window could not be created!" << endl;
        cout << "SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        cout << "Renderer could not be created!" << endl;
        cout << "SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }
    
//    SDL_Surface* surface = SDL_GetWindowSurface(window);
//    if(!surface) {
//        cout << "Surface could not be retrieved from window!" << endl;
//        cout << "SDL_Error: " << SDL_GetError() << endl;
//        return -1;
//    } 
        
    SDL_Texture* texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 32, 32);
    if(!texture) {
        cout << "Texture could not be created!" << endl;
        cout << "SDL_Error: " << SDL_GetError() << endl;
        return -1;
    } 


    shared_ptr<Ricoh2a03> cpu = make_shared<Ricoh2a03>();
    cpu->load(snake_code);
    cpu->reset();

    vector<uint8_t> frame(3072);
    cpu->run_with_callback([cpu, &frame, texture, renderer](Ricoh2a03&) {
        // TODO
        // read user input and write it to mem[0xFF]
        // update mem[0xFE] with new Random Number
        // read mem mapped screen state
        // render screen state
        handle_user_input(cpu);
        cpu->mem_write(0xFE, (uint8_t)rand()%RAND_MAX + 1);

        if(read_screen_state(cpu, frame)) {
            SDL_UpdateTexture(texture, NULL, frame.data(), 3 * 32);
            SDL_RenderPresent(renderer);
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    });

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}
