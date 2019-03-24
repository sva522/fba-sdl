#include "xs.h"

#include <stdlib.h> //memset
#include <stdio.h> // printf
#include <string.h> // strdup
#include <assert.h> // assert

#include "lz4.h"
#include "lz4hc.h"
#include "lz4frame.h"

FILE* _cacheFile = NULL;
char* _cacheFilePath = NULL;

// try to restore this
#ifdef LINUX_PC
bool _dump = false;

uint8_t* _restoreBuffer = NULL;
uint32_t _restoreBufferSize = 0;

uint32_t _total = 0;
uint32_t _totalCompressed = 0;
#endif

void xs_free(){

    if(_cacheFile){
        fclose(_cacheFile); _cacheFile = NULL;
        
        #ifdef LINUX_PC
        double mo = static_cast<double>(_total) / 1000000;
        double moComp = static_cast<double>(_totalCompressed) / 1000000;
        if(_dump){
            printf("End of dump %.2f Mo to %.2f Mo.\n\n",  mo, moComp);
            
        }else{ // restore
            printf("End of restore %.2f Mo from %.2fMo.\n\n", mo, moComp);  
     
        }
        exit(0);
        #endif
    }
}

// Full Init (open file) is performed on first restore
void _xs_init(){

    // execute on fist call
    static bool firstCall = true;
    if(!firstCall || !_cacheFilePath) return; firstCall = false;

    // try to open cacheFile for restore
    _cacheFile = fopen(_cacheFilePath, "rb");
    
    #ifdef LINUX_PC
    printf("\n"); 

    // Open for restore failed ?
    if(!_cacheFile){
        _dump = true;

        // Cannot restore => so dump !
        _cacheFile = fopen(_cacheFilePath, "wb");
        if(!_cacheFile){
            printf("Cannot open %s in write mode.\n", _cacheFilePath);
            xs_free();
        }
        printf("\n\nDUMP to %s cache file ---\n\n", _cacheFilePath);

    }else{
        printf("\n\n%s cache found ! RESTORE --- \n\n", _cacheFilePath);
    }
    #endif

    free(_cacheFilePath); _cacheFilePath = NULL;
}

void xs_init(const char* romName){

    // init members vars
    _cacheFile = NULL;
    _cacheFilePath = NULL;

    #ifdef LINUX_PC
        _dump = false;

        _restoreBuffer = NULL;
        _restoreBufferSize = 0;

        _total = 0;
        _totalCompressed = 0;
    #endif

    // Build cacheFilePath from romName
    _cacheFilePath = strdup(romName);

    char* p = strrchr(_cacheFilePath, '.');
    if(p && strlen(p) > 1){
        strcpy(p, ".x"); p = NULL;
    }else{
        // No .zip or .7z format => No .x to load
        free(_cacheFilePath); _cacheFilePath = NULL;
    }         
}

union StoredSize {
    uint32_t size;
    uint8_t data[4];
}; 

void _safeWrite(FILE* f, const uint8_t* data, uint32_t toWrite){
    uint32_t total = 0;
    uint32_t written = 0;

    while(toWrite > 0){
        written = fwrite(&data[total], sizeof(uint8_t), toWrite, f);
        
        toWrite -= written;
        total   += written;

        #ifdef LINUX_PC
        assert(written > 0);
        #endif
    } 
}

void xs_dump(){
    #ifdef LINUX_PC

    // We need to dump
    if(_dump && _cacheFile && _restoreBuffer && _restoreBufferSize){
        double mo = static_cast<double>(_restoreBufferSize) / 1000000;
        printf("Dump and compressing data: %.2f Mo... ", mo); fflush(stdout);
        
        // Create maxCompress
        LZ4F_preferences_t maxCompress; memset(&maxCompress, 0, sizeof(maxCompress));
        maxCompress.compressionLevel = LZ4HC_CLEVEL_MAX;
        maxCompress.favorDecSpeed = 1;

        // Create buffer
        const size_t bufferSize = LZ4F_compressFrameBound(_restoreBufferSize, &maxCompress);
        uint8_t* bufferOut = new uint8_t[bufferSize];
        memset(bufferOut, 0, bufferSize);

        // Write size
        uint32_t compressedSize = LZ4F_compressFrame(bufferOut, bufferSize, _restoreBuffer, _restoreBufferSize, &maxCompress);
        if (LZ4F_isError(compressedSize)) { // compressedSize may retreive error code
            printf("Compression error: %s\n", LZ4F_getErrorName(compressedSize));
            xs_free();
        }

        // Write chunck size as header // little endian
        StoredSize storedSize; storedSize.size = compressedSize;
        _safeWrite(_cacheFile, storedSize.data, sizeof(storedSize));
        _safeWrite(_cacheFile, bufferOut, compressedSize);

        delete[] bufferOut; bufferOut = NULL;
        fflush(_cacheFile);

        mo = static_cast<double>(compressedSize) / 1000000;
        printf("(OK) (%.2f Mo)\n", mo);

        _total += _restoreBufferSize;
        _totalCompressed += compressedSize;
    }

    // Was dump
    _restoreBuffer = NULL;
    _restoreBufferSize = 0;

    #endif
}

void _error(const char* msg){
    printf("%s\n");

    const char* errorFilePath = "fba-error.log";
    remove(errorFilePath);
    FILE* f = fopen(errorFilePath, "wb");

    fprintf(f, "%s\n", msg);
    fclose(f); f = NULL;

    exit(1);
}

// At end all bufferDec is used (== toDec == remainingSrc == 0)
// size is reduced as decompressed buffer is filled
void _safeDecompress(LZ4F_dctx* dctx, const uint8_t* bufferDec, uint16_t toDec, uint8_t* decompressed, uint32_t& size){
    // Call vars
    void* dstBuffer = NULL;
    size_t dstSize = size;
    const void* srcBuffer = NULL;
    size_t srcSize = 0;   

    // Loop vars
    uint16_t totalSrc = 0;
    uint32_t totalDst = 0;
    uint16_t remainingSrc = toDec;
    uint32_t remainingDst = size;
    size_t hint = 0;

    while(remainingSrc > 0){
        dstSize = remainingDst;
        
        //srcSize = remainingSrc;
        // Can use hint ? Not available on first call
        if(hint > 0 && hint <= remainingSrc) srcSize = hint;
        else  /* hint == 0 on first call */  srcSize = remainingSrc;
        
        srcBuffer = &bufferDec[totalSrc];
        dstBuffer = &decompressed[totalDst];

        hint = LZ4F_decompress(dctx, dstBuffer, &dstSize, srcBuffer, &srcSize, NULL);
        // hint may retreive error code
        if (LZ4F_isError(hint)) {
            //LZ4F_getErrorName(hint));
            _error("Decompression error !");
        }
        
        remainingSrc -= srcSize;
        totalSrc += srcSize;

        remainingDst -= dstSize;
        totalDst += dstSize;
    }
    assert(remainingSrc == 0);
    size = remainingDst;
}

void _safeRead(FILE* f, uint8_t* data, uint32_t toRead){
    uint32_t total = 0;
    uint32_t got = 0;

    while(toRead > 0){
        got = fread(&data[total], sizeof(uint8_t), toRead, f);
        
        toRead -= got;
        total  += got;

        #ifdef LINUX_PC
        assert(got > 0);
        #endif
    }  
}

// return true if restore is not aivailable and Neload SHOULD be used
// return false if restore can be performed and and NeoLoad should NOT be used
bool xs_restore(uint8_t* data, uint32_t size){
    _xs_init(); //open file if necessary

    #ifdef LINUX_PC
    static bool firstCall = true;
    if(firstCall){ firstCall = false; printf("\n"); }

    if(_dump) {
        // It will be dump instead
        _restoreBuffer = data;
        _restoreBufferSize = size;

        printf("Start dump ----\n");
        return true;
    }
    #endif

    // Cannot restore => load 
    if(!_cacheFile) return true; 

    // Read size Little endian
    StoredSize storedSize; storedSize.size = 0;
    _safeRead(_cacheFile, storedSize.data, sizeof(storedSize));
    uint32_t compressedSize = storedSize.size;

    #ifdef LINUX_PC
        double mo = static_cast<double>(compressedSize) / 1000000;
        printf("Restoring %.2f Mo... ", mo); fflush(stdout);
    #endif


    // Create buffer decompression
    const uint16_t decBufferSize = UINT16_MAX; // 65535 => ~= 65Ko
    uint8_t bufferDec[decBufferSize];
    memset(bufferDec, 0, decBufferSize);

    // Create dictionnary
    LZ4F_dctx* dctx = NULL;
    LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);

    uint32_t remaining = compressedSize;
    uint32_t total = 0;
    uint32_t remainingDec = size;
    uint32_t totalDec = 0;
    uint16_t toRead = 0;

    while(remaining > 0){
        toRead =  decBufferSize;
        if(remaining < decBufferSize) toRead = remaining;
        
        _safeRead(_cacheFile, bufferDec, toRead);
        total += toRead;
        remaining -= toRead;

        _safeDecompress(dctx, bufferDec, toRead, &data[totalDec], remainingDec);
        totalDec = size - remainingDec;
    }
    
    LZ4F_freeDecompressionContext(dctx); dctx = NULL;
    #ifdef LINUX_PC
        mo = static_cast<double>(size) / 1000000;
        printf("(OK) (%.2f Mo)\n", mo);

        _total += size;
        _totalCompressed += compressedSize;
    #endif

    // Nothing still to restore
    if(feof(_cacheFile)) xs_free();

    return false; // restore done / => must not load
}

//     ; void* dup = malloc(xs_spriteSize); memcpy(dup, NeoSpriteROM[nNeoActiveSlot], xs_spriteSize); memset(NeoSpriteROM[nNeoActiveSlot], 0, xs_spriteSize);
//NeoLoad
//    printf("Check: %s\n", memcmp(dup, NeoSpriteROM[nNeoActiveSlot], xs_spriteSize) == 0 ? "OK" : "NOK"); free(dup); dup = NULL;

// if(xs_restore(pDest, nSpriteSize))
// xs_dump();
