#include <stdint.h>
#include <stdlib.h>

void xs_init(const char* romName);

bool xs_before(uint8_t* data = NULL, uint32_t size = 0);
void xs_after();