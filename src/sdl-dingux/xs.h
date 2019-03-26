#include <stdint.h>
#include <stdlib.h>

void xs_init(const char* romName);

bool xs_before(const char* ressourceName, uint8_t* data = NULL, uint32_t size = 0);
void xs_after();

void xs_free();

void ticktock(const char* ressourceName = NULL);