
#include <sys/time.h>
double _getCurrentTime(){
    struct timeval t; memset(&t, 0, sizeof(t));
    gettimeofday(&t, 0);
    return ((t.tv_sec + t.tv_usec*1e-6));
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

union StoredSize {
    uint32_t size;
    uint8_t data[4];
};

uint32_t _readSizeInStream(FILE* f){
    // Read size Little endian
    StoredSize storedSize; storedSize.size = 0;
    _safeRead(f, storedSize.data, sizeof(storedSize));
    return storedSize.size;
}

// Full Init (open file) is performed on first restore
void _openCacheFile(){

    // Execute on fist call (and if we have a file to open)
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
        printf("\n\nDUMP to %s cache file ------\n\n", _cacheFilePath);

    }else{
        printf("\n\n%s cache found ! RESTORE ------ \n\n", _cacheFilePath);
    }
    #endif

    free(_cacheFilePath); _cacheFilePath = NULL;
    
    #ifdef BENCHMARK
        _benchmarkFile = fopen("benchmark.txt", "w");
    #endif
}

void _restoreFromFile(FILE* cacheFile, uint8_t* buffer, uint32_t size, uint32_t compressedSize){
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
        
        _safeRead(cacheFile, bufferDec, toRead);
        total += toRead;
        remaining -= toRead;

        _safeDecompress(dctx, bufferDec, toRead, &buffer[totalDec], remainingDec);
        totalDec = size - remainingDec;
    }
    
    LZ4F_freeDecompressionContext(dctx); dctx = NULL;
}

#ifdef LINUX_PC

uint32_t _dumpToFile(FILE* cacheFile, uint8_t* buffer, uint32_t size){
    
     // Create maxCompress
    LZ4F_preferences_t maxCompress; memset(&maxCompress, 0, sizeof(maxCompress));
    maxCompress.compressionLevel = LZ4HC_CLEVEL_MAX;
    maxCompress.favorDecSpeed = 1;

    // Create buffer
    const size_t bufferSize = LZ4F_compressFrameBound(_restoreBufferSize, &maxCompress);
    uint8_t* bufferOut = new uint8_t[bufferSize];
    memset(bufferOut, 0, bufferSize);

    // Write size
    uint32_t compressedSize = LZ4F_compressFrame(bufferOut, bufferSize, buffer, size, &maxCompress);
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
    return compressedSize;
}

#endif
