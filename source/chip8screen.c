#include "chip8screen.h"
#include <assert.h>
#include <memory.h>

void chip8_screen_clear(struct chip8_screen* screen)
{
    memset(screen->pixels, 0, sizeof(screen->pixels));
}

/**
 * @brief Verify that the x and y coordinates are within the screen bounds.
 * 
 * @param x The x-axis pixel position.
 * @param y The y-axis pixel position.
 * @return Void.
 */
static void chip8_screen_in_bounds(int x, int y)
{
    assert( x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
}


/**
 * @brief Set the corresponding pixel on the screen.
 * 
 * @param screen Pointer to a chip8_screen struct.
 * @param x The x-axis pixel position.
 * @param y The y-axis pixel position.
 * @return Void.
 */
void chip8_screen_set(struct chip8_screen* screen, int x, int y)
{
    chip8_screen_in_bounds(x, y);
    screen->pixels[y][x] = true;
}


/**
 * @brief Check whether a pixel on the screen is set or not.
 * 
 * @param screen Pointer to a chip8_screen struct.
 * @param x The x-axis pixel position.
 * @param y The y-axis pixel position.
 * @return true The pixel is set.
 * @return false The pixel is not set.
 */
bool chip8_screen_is_set(struct chip8_screen* screen, int x, int y)
{
    chip8_screen_in_bounds(x, y);
    return screen->pixels[y][x];
}


/**
 * @brief Draw a sprite on the screen at the specified pixel.
 * 
 * @param screen Pointer to a chip8_screen struct.
 * @param x The x-axis pixel position.
 * @param y The y-axis pixel position.
 * @param sprite Memory address of the first byte of the sprite to draw.
 * @param size The length of the sprite (in pixels).
 * @return true There has been a pixel collision.
 * @return false No pixel collision.
 */
bool chip8_screen_draw_sprite(struct chip8_screen* screen, int x, int y, const char* sprite, int length)
{
    bool pixel_collision = false;

    for (int ly = 0 ; ly < length ; ly++)
    {
        char c = sprite[ly];
        for (int lx = 0 ; lx < 8 ; lx++)
        {
            if ( (c & (0x80 >> lx)) == 0)
            {
                continue;
            }

            if (screen->pixels[(y + ly) % CHIP8_HEIGHT][(x + lx) % CHIP8_WIDTH])
            {
                pixel_collision = true;
            }

            screen->pixels[(y + ly) % CHIP8_HEIGHT][(x + lx) % CHIP8_WIDTH] ^= true;
        }
    }
    return pixel_collision;
}