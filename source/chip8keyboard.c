#include "chip8keyboard.h"
#include <assert.h>

static void chip8_keyboard_in_bounds(int key)
{
    assert(key >= 0 && key < CHIP8_TOTAL_KEYS);
}

/**
 * @brief Define the map of the keyboard.
 * 
 * @param keyboard Pointer to a chip8_keyboard struct.
 * @param map Array containing the keyboard mapping.
 */
void chip8_keyboard_set_map(struct chip8_keyboard* keyboard, const char* map)
{
    keyboard->keyboard_map = map;
}

/**
 * @brief Map the physical keyboard with the Chip-8 virtual keyboard.
 * 
 * @param keyboard Pointer to a chip8_keyboard struct.
 * @param key Character corresponding to the physical key.
 * @return int The mapped Chip-8 virtual key.
 */
int chip8_keyboard_map(struct chip8_keyboard* keyboard, char key)
{
    for (int i = 0 ; i < CHIP8_TOTAL_KEYS ; i++)
    {
        if (keyboard->keyboard_map[i] == key)
        {
            return i;
        }
    }

    return -1;
}


/**
 * @brief Indicate that a key of the Chip-8 virtual keyboard has been pressed.
 * 
 * @param keyboard Pointer to a chip8_keyboard struct.
 * @param key Virtual key.
 * @return Void.
 */
void chip8_keyboard_down(struct chip8_keyboard* keyboard, int key)
{
    chip8_keyboard_in_bounds(key);
    keyboard->keyboard[key] = true;
}


/**
 * @brief Indicate that a key of the Chip-8 virtual keyboard has been released.
 * 
 * @param keyboard Pointer to a chip8_keyboard struct.
 * @param key Virtual key.
 * @return Void.
 */
void chip8_keyboard_up(struct chip8_keyboard* keyboard, int key)
{
    chip8_keyboard_in_bounds(key);
    keyboard->keyboard[key] = false;
}


/**
 * @brief Verify if a Chip-8 virtual key is pressed.
 * 
 * @param keyboard Pointer to a chip8_keyboard struct.
 * @param key Virtual key.
 * @return true The key is pressed.
 * @return false The key is not pressed.
 */
bool chip8_keyboard_is_down(struct chip8_keyboard* keyboard, int key)
{
    return keyboard->keyboard[key];
}