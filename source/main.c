#include<stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "chip8keyboard.h"

/* This array contains the Chip-8 virtual keys */ 
const char keyboard_map[CHIP8_TOTAL_KEYS] = 
{
    SDLK_0, SDLK_1, SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char** argv)
{
    /* 
    Verify that a filename has been provided before running the program.
    If no filename is received, the program will be terminated.                    
    */
    if (argc < 2)
    {
        printf("You must provide a file to load \n");
        return -1;
    }

    /*
     Store the filename and then open it in binary read mode.
     If the program fails to open the file, then it will terminate.
    */
    const char* filename = argv[1];
    printf("The filename to load is: %s\n", filename);
    FILE* f = fopen(filename, "rb");
    if (!f)
    {
        printf("Failed to open the file");
        return -1;
    }

    /* Set the cursor at the end of the file */
    fseek(f, 0, SEEK_END);
    /* Get the position of the cursor (this will be the size of the file) */
    long size = ftell(f);
    /* Return the cursor to the beginning of the file */
    fseek(f, 0, SEEK_SET);

    /*
     Read the file and store its content in a buffer.
     If the program fails to read the file, it will terminate.
    */
    char buffer[size];
    int res = fread(buffer, size, 1, f);
    if (res != 1)
    {
        printf("Failed to read from file");
        return -1;
    }

    struct chip8 chip8;

    /* Initialize the chip8 instance */
    chip8_init(&chip8);

    /* Load the program (the file read above) into memory */
    chip8_load(&chip8, buffer, size);

    chip8_keyboard_set_map(&chip8.keyboard, keyboard_map);

    /* Initialize the SDL library */
    SDL_Init(SDL_INIT_EVERYTHING);

    /* Create a window displaying the Chip-8 screen with its dimesions scaled */
    SDL_Window* window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH * CHIP8_WINDOW_SCALE,
        CHIP8_HEIGHT * CHIP8_WINDOW_SCALE,
        SDL_WINDOW_SHOWN
    );

    /* Create a 2D rendering context for a window */
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_TEXTUREACCESS_TARGET
    );

    while(1)
    {
        SDL_Event event;

        /* Poll for currently pending events */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                /* Terminate the program */
                case SDL_QUIT:
                    goto out;
                break;

                /* A key has ben pressed */
                case SDL_KEYDOWN:
                {
                    char key = event.key.keysym.sym;
                    int virtual_key = chip8_keyboard_map(&chip8.keyboard, key);
                    if (virtual_key != -1)
                    {
                        chip8_keyboard_down(&chip8.keyboard, virtual_key);
                    }
                }
                break;

                /* A key has been released */
                case SDL_KEYUP:
                {
                    char key = event.key.keysym.sym;
                    int virtual_key = chip8_keyboard_map(&chip8.keyboard, key);
                    if (virtual_key != -1)
                    {
                        chip8_keyboard_up(&chip8.keyboard, virtual_key);
                    }
                }
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        

        for (int x = 0 ; x < CHIP8_WIDTH ; x++)
        {
            for (int y = 0 ; y < CHIP8_HEIGHT ; y++)
            {
                /* Check if a pixel has to be drawn */
                if ( chip8_screen_is_set(&chip8.screen, x, y) )
                {
                    /* Draw a rectangle symbolizing a pixel */
                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_SCALE;
                    r.y = y * CHIP8_WINDOW_SCALE;
                    r.w = CHIP8_WINDOW_SCALE;
                    r.h = CHIP8_WINDOW_SCALE;
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }
        
        /* Update the screen */
        SDL_RenderPresent(renderer);

        /* Delay the program according to the delay_timer register value */
        if (chip8.registers.delay_timer > 0)
        {
            Sleep(1);
            chip8.registers.delay_timer -= 1;
        }

        /* Beep for a certain time defined by the sound timer register value */
        if (chip8.registers.sound_timer > 0)
        {
            Beep(15000, 10 * chip8.registers.sound_timer);
            chip8.registers.sound_timer = 0;
        }

        /* Get the current opcode to execute */
        unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.PC);
        printf("%x\n", opcode);
        chip8.registers.PC += 2;
        chip8_exec(&chip8, opcode);
    }

out:
    SDL_DestroyWindow(window);

    return 0;
}