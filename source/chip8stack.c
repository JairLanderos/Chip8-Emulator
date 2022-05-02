#include "chip8stack.h"
#include "chip8.h"
#include <assert.h>

/**
 * @brief Verify that the SP has not exceeded the stack bounds.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @return Void.
 */
static void chip8_is_stack_in_bounds(struct chip8* chip8)
{
    assert(chip8->registers.SP < sizeof(chip8->stack.stack));
}


/**
 * @brief Insert an element at the top of the stack and increment SP by 1.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @param val The element to be pushed.
 * @return Void.
 */
void chip8_stack_push(struct chip8* chip8, unsigned short val)
{
    chip8->registers.SP += 1;
    chip8_is_stack_in_bounds(chip8);
    chip8->stack.stack[chip8->registers.SP] = val;
}


/**
 * @brief Remove the element at the top of the stack and decrement SP by 1.
 * 
 * @param chip8 Pointer to a chip8 struct.
 * @return unsigned short The element popped.
 */
unsigned short chip8_stack_pop(struct chip8* chip8)
{
    chip8_is_stack_in_bounds(chip8);
    unsigned short result = chip8->stack.stack[chip8->registers.SP];
    chip8->registers.SP -= 1;
    return result;
}