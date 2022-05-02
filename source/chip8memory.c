#include "chip8memory.h"
#include<assert.h>

/**
 * @brief Verifies that the received index is within the memory bounds.
 * 
 * @param index The index that will be verified
 * @return Void.
 */
static void chip8_is_memory_in_bounds(int index)
{
    assert(index >= 0 && index < CHIP8_MEMORY_SIZE);
}



/**
 * @brief Store the value in memory at the byte indicated by index.
 * 
 * @param memory Pointer to a chip8_memory struct.
 * @param index  An index to access the desired memory byte.
 * @param value The value that will be stored.
 * @return Void.
 */
void chip8_memory_set(struct chip8_memory* memory, int index, unsigned char value)
{
    chip8_is_memory_in_bounds(index);
    memory->memory[index] = value;
}



/**
 * @brief Get the value stored in the memory byte indicated by index and return it.
 * 
 * @param memory Pointer to a chip8_memory struct.
 * @param index An index to indicate the byte of memory from which its value will be returned.
 * @return unsigned char The corresponding value.
 */
unsigned char chip8_memory_get(struct chip8_memory* memory, int index)
{
    chip8_is_memory_in_bounds(index);
    return memory->memory[index];
}



/**
 * @brief Get the the value of the two memory bytes from the number indicated by the index.
 * 
 * @param memory Pointer to a chip8_memory struct.
 * @param index An index to indicate the two bytes of memory from which its values will be returned
 * @return unsigned short The corresponding values.
 */
unsigned short chip8_memory_get_short(struct chip8_memory* memory, int index)
{
    unsigned char byte1 = chip8_memory_get(memory, index);
    unsigned char byte2 = chip8_memory_get(memory, index + 1);
    return byte1 << 8 | byte2;
}