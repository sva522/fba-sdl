#include <stdint.h>

void xs_init(const char* romName);

bool xs_restore(uint8_t* data, uint32_t size);
void xs_dump();

void xs_free();

