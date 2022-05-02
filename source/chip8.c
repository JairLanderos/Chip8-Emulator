#include "chip8.h"
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>


/*  
    This array contains the default character set.
    Every 5 values represents a character, from '0' to 'F'.
    This array will be stored at the beginning of memory
*/
const char chip8_default_character_set[] = 
{
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    0x90, 0x90, 0xf0, 0x10, 0x10,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0xf0, 0x10, 0x20, 0x40, 0x40,
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80
};

/**
 * @brief Initialize all members of the chip8 struct to 0 and load the default character set.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @return Void.
 */
void chip8_init(struct chip8* chip8)
{
    memset(chip8, 0, sizeof(struct chip8));
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
}


/**
 * @brief Load the program into memory.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @param buffer Address of the first program instruction.
 * @param size Size of the entire program.
 */
void chip8_load(struct chip8* chip8, const char* buffer, size_t size)
{
    assert( (CHIP8_PROGRAM_LOAD_ADDRESS + size) < CHIP8_MEMORY_SIZE );
    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buffer, size);
    chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;
}


/**
 * @brief Block the program until a key is pressed.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @return char The key that has been pressed.
 */
static char chip8_wait_for_key_press(struct chip8* chip8)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type != SDL_KEYDOWN)
            continue;

        char c = event.key.keysym.sym;
        char chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
        if (chip8_key != -1)
        {
            return chip8_key;
        }
    }
    return -1;
}


/**
 * @brief Extended version of @chip8_exec()
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @param opcode Operation code to execute.
 */
static void chip8_exec_extended(struct chip8* chip8, unsigned short opcode)
{
    unsigned short nnn = opcode & 0x0fff;
    unsigned char x = (opcode & 0x0f00) >> 8;
    unsigned char y = (opcode & 0x00f0) >> 4;
    unsigned char kk = opcode & 0x00ff;
    unsigned char n = opcode & 0x000f;
    unsigned short res = 0;

    switch(opcode & 0xf000)
    {
        /* JP addr: Jump to location nnn (0x1nnn) */
        case 0x1000:
            chip8->registers.PC = nnn;
        break;

        /* CALL addr: Call subroutine at nnn (0x2nnn) */
        case 0x2000:
            chip8_stack_push(chip8, chip8->registers.PC);
            chip8->registers.PC = nnn; 
        break;

        /* SE Vx, byte: Skip next instruction if Vx = kk (0x3xkk)*/
        case 0x3000:
            if (chip8->registers.V[x] == kk)
            {
                chip8->registers.PC += 2;
            }
        break;

        /* SNE Vx, byte: Skip next instruction if Vx != kk (0x3xkk) */
        case 0x4000:
            if (chip8->registers.V[x] != kk)
            {
                chip8->registers.PC += 2;
            }
        break;

        /* SE Vx, Vy: Skip next instruction if Vx = Vy (0x5xy0) */
        case 0x5000:
            if (chip8->registers.V[x] == chip8->registers.V[y])
            {
                chip8->registers.PC += 2;
            }
        break;

        /* LD Vx, byte: Set Vx = kk (0x6xkk) */
        case 0x6000:
            chip8->registers.V[x] = kk;
        break;

        /* ADD Vx, byte: Set Vx = Vx + kk (0x7xkk) */
        case 0x7000:
            chip8->registers.V[x] += kk;
        break;

        case 0x8000:
            switch (opcode & 0x000f)
            {
                /* LD Vx, Vy: Set Vx = Vy (0x8xy0) */
                case 0x00:
                    chip8->registers.V[x] = chip8->registers.V[y];
                break;

                /* OR Vx, Vy: Set Vx = Vx OR Vy (0x8xy1) */
                case 0x01:
                    chip8->registers.V[x] |= chip8->registers.V[y];
                break;

                /* AND Vx, Vy: Set Vx = Vx AND Vy (0x8xy2) */
                case 0x02:
                    chip8->registers.V[x] &= chip8->registers.V[y];
                break;

                /* XOR Vx, Vy: Set Vx = Vx XOR Vy (0x8xy3) */
                case 0x03:
                    chip8->registers.V[x] ^= chip8->registers.V[y];
                break;

                /* ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry (0x8xy4) */
                case 0x04:
                    res = chip8->registers.V[x] + chip8->registers.V[y];
                    chip8->registers.V[0x0f] = 0x00;
                    if (res > 0xff)
                    {
                        chip8->registers.V[0x0f] = 0x01;
                    }
                    chip8->registers.V[x] = res;
                break;

                /* SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow (0x8xy5) */
                case 0x05:
                    chip8->registers.V[0x0f] = 0x00;
                    if (chip8->registers.V[x] > chip8->registers.V[y])
                    {
                        chip8->registers.V[0x0f] = 0x01;
                    }
                    chip8->registers.V[x] -= chip8->registers.V[y];
                break;

                /* SHR Vx, {, Vy}: Set Vx = Vx SHR 1 (0x8xy6) */
                case 0x06:
                    chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x01;
                    chip8->registers.V[x] /= 2;
                break;

                /* SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow (0x8xy7) */
                case 0x07:
                    chip8->registers.V[0x0f] = chip8->registers.V[y] > chip8->registers.V[x];
                    chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
                break;

                /* SHL Vx, {, Vy}: Set Vx = Vx SHL 1 (0x8xy8) */
                case 0x08:
                    chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x80;
                    chip8->registers.V[x] *= 2;
                break;
            }
        break;

        /* SNE Vx, Vy: Skip next instruction if Vx != Vy (0x9xy0) */
        case 0x9000:
            if (chip8->registers.V[x] != chip8->registers.V[y])
            {
                chip8->registers.PC += 2;
            }
        break;

        /* LD I, addr: Set I = nnn (0xAnnn) */
        case 0xA000:
            chip8->registers.I = nnn;
        break;

        /* JP V0, addr: Jump to location nnn + V0 (0xBnnn) */
        case 0xB000:
            chip8->registers.PC = nnn + chip8->registers.V[0x00];
        break;

        /* RND Vx, byte: Set Vx = random byte AND kk (0xCxkk) */
        case 0XC000:
            srand(clock());
            chip8->registers.V[x] = (rand() % 255) & kk;
        break;

        /* DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision (0xDxyn) */
        case 0xD000:
        {
            const char* sprite = (const char*) &chip8->memory.memory[chip8->registers.I];
            chip8->registers.V[0x0f] = chip8_screen_draw_sprite(&chip8->screen,
                                                                chip8->registers.V[x],
                                                                chip8->registers.V[y],
                                                                sprite,
                                                                n);
        }
        break;

        case 0xE000:
        {
            switch (opcode & 0x00ff)
            {
                /* SKP Vx: Skip next instruction if the key with the value of Vx is pressed (0xEx9E) */
                case 0x9e:
                    if ( chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x]) )
                    {
                        chip8->registers.PC += 2;
                    }
                break;

                /* SKNP Vx: Skip next instruction if the key with the value Vx is not pressed (0xExA1) */
                case 0xa1:
                    if ( !chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x]) )
                    {
                        chip8->registers.PC += 2;
                    }
                break;
            }
        }
        break;

        case 0xF000:
        {
            switch (opcode & 0x00ff)
            {
                /* LD Vx, DT: Set Vx = delay timer value (0xFx07) */
                case 0x07:
                    chip8->registers.V[x] = chip8->registers.delay_timer;
                break;

                /* LD Vx, K: Wait for a key press, store the value of the key in Vx (0xFx0A) */
                case 0x0a:
                {
                    char pressed_key = chip8_wait_for_key_press(chip8);
                    chip8->registers.V[x] = pressed_key;
                }
                break;

                /* LD DT, Vx: Set delay timer = Vx (0xFx15) */
                case 0x15:
                    chip8->registers.delay_timer = chip8->registers.V[x];
                break;

                /* LD ST, Vx: Set sound timer = Vx (0xFx18) */
                case 0x18:
                    chip8->registers.sound_timer = chip8->registers.V[x];
                break;

                /* ADD I, Vx: Set I = I + Vx (0xFx1E) */
                case 0x1e:
                    chip8->registers.I += chip8->registers.V[x];
                break;

                /* LD F, Vx: Set I = location of sprite for digit Vx (0xFx29) */
                case 0x29:
                    chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
                break;

                /* LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2 (0xFx33) */
                case 0x33:
                {
                    unsigned char hundreds = chip8->registers.V[x] / 100;
                    unsigned char tens = chip8->registers.V[x] / 10 % 10;
                    unsigned char units = chip8->registers.V[x] % 10;
                    chip8_memory_set(&chip8->memory, chip8->registers.I, hundreds);
                    chip8_memory_set(&chip8->memory, chip8->registers.I + 1, tens);
                    chip8_memory_set(&chip8->memory, chip8->registers.I + 2, units);
                }
                break;

                /* LD [I], Vx: Store the registers V0 through Vx in memory starting at location I (0xFx55) */
                case 0x55:
                    for (int i = 0 ; i <= x ; i++)
                    {
                        chip8_memory_set(&chip8->memory, chip8->registers.I + i, chip8->registers.V[i]);
                    }
                break;

                /* LD Vx, [I]: Read registers V0 through Vx from memory starting at location I (0xFx65) */
                case 0x65:
                    for (int i = 0 ; i <= x ; i++)
                    {
                        chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
                    }
                break;
            }
        }
        break;

    }
}

/**
 * @brief Execute the instruction specified by the opcode.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @param opcode Operation code to execute.
 */
void chip8_exec(struct chip8* chip8, unsigned short opcode)
{
    switch(opcode)
    {
        /* CLS: Clear the display */
        case 0x00E0:
            chip8_screen_clear(&chip8->screen);
        break;

        /* RET: Return from a subroutine */
        case 0x00EE:
            chip8->registers.PC = chip8_stack_pop(chip8);
        break;

        default:
            chip8_exec_extended(chip8, opcode);
    }
}