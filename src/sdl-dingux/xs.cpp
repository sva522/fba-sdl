#include "xs.h"

#include <stdlib.h> //memset
#include <stdio.h> // printf
#include <string.h> // strdup
#include <assert.h> // assert

#include "lz4.h"   // lz4
#include "lz4hc.h" // lz4 max compress 
#include "lz4frame.h" // lz4 interoperable

FILE* _cacheFile = NULL;

// try to restore this
#ifdef LINUX_PC
uint8_t* _restoreBuffer = NULL;
uint32_t _restoreBufferSize = 0;
#endif

#include "xs_utils.cpp"

void xs_init(const char* romName){

    // init members vars  -------
    _cacheFile = NULL;

    #ifdef LINUX_PC
        _restoreBuffer = NULL;
        _restoreBufferSize = 0;
    #endif
    // -------------------------

    // Build cacheFilePath from romName
    char* cacheFilePath = strdup(romName);

    char* p = strrchr(cacheFilePath, '.');
    if(p && strlen(p) > 1){
        strcpy(p, ".x"); p = NULL;
        
        #ifdef LINUX_PC
            // try to open cacheFile to dump
            _cacheFile = fopen(cacheFilePath, "wb"); 
        #else
            // try to open cacheFile for restore
            _cacheFile = fopen(cacheFilePath, "rb");
        #endif
    }
    
    // No .zip or .7z format => No .x to load
    free(cacheFilePath);
}

// return true if restore is not aivailable and Neload SHOULD be used
// return false if restore can be performed and and NeoLoad should NOT be used
bool xs_before(uint8_t* buffer, uint32_t size){
    
    // DUMP
    #ifdef LINUX_PC
    
     // Record work to do ----
    _restoreBuffer = buffer;
    _restoreBufferSize = size;
    
    printf("\nStart dump ----\n");
    return true;
    #endif // ----------------------------

    // NORMAL (Cannot restore => load) 
    if(!_cacheFile) return true;

    // RESTORE
    _restore(_cacheFile, buffer, size);
    return false;
}

void xs_after(){
    #ifdef LINUX_PC
    // DUMP
    printf("Dump and compressing data: %.2f Mo... ", toMo(_restoreBufferSize)); fflush(stdout);
    uint32_t compressedSize =_dump(_cacheFile, _restoreBuffer, _restoreBufferSize);
    printf("(OK) (%.2f Mo)\n", toMo(compressedSize));
    #endif
    
    if(_cacheFile){
        fclose(_cacheFile); _cacheFile = NULL;
    }
    
    #ifdef LINUX_PC
    exit(0);
    #endif
}
