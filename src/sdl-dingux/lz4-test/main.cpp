#include "lz4frame.h"
#include "lz4hc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <list>

using namespace std;

double _xs_getCurrentTime(){
    struct timeval t; memset(&t, 0, sizeof(t));
    gettimeofday(&t, 0);
    return ((t.tv_sec + t.tv_usec*1e-6) * 1000);
}

uint32_t getFileSize(const char* filePath){
    FILE* f = fopen(filePath, "rb");

    // Get File size
    fseek(f, 0, SEEK_END);
    long int fileSize = ftell(f);
    fclose(f); f = NULL;
    return static_cast<uint32_t>(fileSize);
}

uint32_t safeRead(FILE* f, uint8_t* data, uint32_t toRead){
    uint32_t total = 0;
    uint32_t got = 0;
    uint32_t tries = 0;
    
    /*
    const uint16_t mbSpeed = 8; // 0 unlimited / simulate slow read
    if(mbSpeed > 0) usleep(toRead / mbSpeed); // 1µs / o => 1Mo/sec
    */

    while(toRead > 0){
        got = fread(&data[total], sizeof(uint8_t), toRead, f);
        assert(got > 0);
        toRead-= got;
        total += got;

        tries++;
    }
    return tries;  
}

uint32_t safeWrite(FILE* f, const uint8_t* data, uint32_t toWrite){
    uint32_t total = 0;
    uint32_t written = 0;
    uint32_t tries = 0;

    while(toWrite > 0){
        written = fwrite(&data[total], sizeof(uint8_t), toWrite, f);
        assert(written > 0);
        toWrite -= written;
        total   += written;

        tries++; 
    }
    return tries;  
}

string compressFile(const char* filePath){

    string newFile = filePath; newFile += ".lz4";
    //remove(newFile.c_str());
    FILE * f = fopen(newFile.c_str(), "rb");
    if(f != NULL){
        // File exists no need to create it
        fclose(f); f = NULL;
        return newFile;
    }
    printf("Compressing %s...\n", newFile.c_str());

    // Read raw data
    const uint32_t fileSize = getFileSize(filePath);
    uint8_t* bufferIn = new uint8_t[fileSize];
    memset(bufferIn, 0, fileSize);
    f = fopen(filePath, "rb");
    safeRead(f, bufferIn, fileSize);
    fclose(f); f = NULL;

    LZ4F_preferences_t maxCompress; memset(&maxCompress, 0, sizeof(maxCompress));
    maxCompress.compressionLevel = LZ4HC_CLEVEL_MAX;
    maxCompress.favorDecSpeed = 1;
    const size_t bufferSize = LZ4F_compressFrameBound(fileSize, &maxCompress);

    uint8_t* bufferOut = new uint8_t[bufferSize];
    memset(bufferOut, 0, bufferSize);
    uint32_t compressed_size = LZ4F_compressFrame(bufferOut, bufferSize, bufferIn, fileSize, &maxCompress);

    delete[] bufferIn; bufferIn = NULL;

    // Write compressed data
    f = fopen(newFile.c_str(), "wb");

    // Write chunck size as header
    //fwrite(&compressed_size, sizeof(uint32_t), 1, f); 

    size_t written = fwrite(bufferOut, sizeof(uint8_t), compressed_size, f);
    assert(written == compressed_size);
    fclose(f); f = NULL;

    delete[] bufferOut; bufferOut = NULL;

    printf("Compression done !\n");
    return newFile;
}

// At end all bufferDec is used (== toDec == remainingSrc == 0)
// size is reduced as decompressed buffer is filled
void safeDecompress(LZ4F_dctx* dctx, const uint8_t* bufferDec, uint16_t toDec, uint8_t* decompressed, uint32_t& size){
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

        //printf("%u, %u\n", srcSize, dstSize);
        hint = LZ4F_decompress(dctx, dstBuffer, &dstSize, srcBuffer, &srcSize, NULL);
        //printf("%u, %u, %u<=\n", srcSize, dstSize, hint);
        
        remainingSrc -= srcSize;
        totalSrc += srcSize;

        remainingDst -= dstSize;
        totalDst += dstSize;
    }
    assert(remainingSrc == 0);
    size = remainingDst;
}

// Give Mo/sec form o and ms
double computeSpeed(uint32_t size, double durationMs){
    
    double speed = static_cast<double>(size) / durationMs; // => o / ms
    //speed = speed / 1000000 * 1000; // => Mo/sec
    return speed / 1000;
}

uint32_t loadRaw(const char* filePath, uint8_t* buffer, uint32_t fileSize){
    FILE* f = fopen("mslug3.x", "rb");

    // Single read
    uint32_t tries = safeRead(f, buffer, fileSize);

    // Close file and print benchmark
    fclose(f); f = NULL; // fflush
    return tries;
}

const uint16_t decBufferSize = UINT16_MAX; // 65535 => ~= 65Ko

// Start value
struct ThreadArg {
    uint8_t* buffer;
    const uint32_t fileSize;   
};

struct ThreadBuffer {
    uint8_t* data;
    uint16_t size; 
};

pthread_mutex_t accessList;
std::list<ThreadBuffer> workList;

void* threadDecompression(void* arg){
    // Get Args
    ThreadArg* args = reinterpret_cast<ThreadArg*>(arg);
    uint8_t* buffer           = args->buffer;
    const uint32_t fileSize   = args->fileSize;

    ThreadBuffer thBuffer = { NULL, 0};
  
    LZ4F_dctx* dctx = NULL;
    LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);

    uint32_t remainingDec = fileSize;
    uint32_t totalDec = 0;

    while(remainingDec > 0){
        
        pthread_mutex_lock(&accessList);
            while(workList.empty()){
                pthread_mutex_unlock(&accessList);
                usleep(100);
                pthread_mutex_lock(&accessList);
            }
            thBuffer = workList.front(); workList.pop_front();      
        pthread_mutex_unlock(&accessList);

        safeDecompress(dctx, thBuffer.data, thBuffer.size, &buffer[totalDec], remainingDec);

        totalDec = fileSize - remainingDec;
        delete[] thBuffer.data;
    }
    
    LZ4F_freeDecompressionContext(dctx); dctx = NULL;

    pthread_exit(NULL);
    return NULL;
}

uint32_t threadedDecompression(const char* filePath, uint8_t* buffer, const uint32_t compressedSize, const uint32_t fileSize, uint16_t& maxQueueLenght){
    
    // Init shared var
    assert(pthread_mutex_init(&accessList,NULL) == 0);
    pthread_mutex_lock(&accessList); bool locked = true; // Lock it from start

    // Init buffer
    uint8_t* bufferDec = NULL;
    ThreadBuffer thBuffer = { NULL, 0};

    // Init args
    ThreadArg arg = {
        buffer,        
        fileSize,
    };
        
    // Create thread
    pthread_t threadDecompInfo; //memset(&threadDecomp, 0, sizeof(threadDecomp);
    
    if(pthread_create(&threadDecompInfo, NULL, threadDecompression, &arg) == -1) {
	        perror("pthread_create");
	        return EXIT_FAILURE;
    }
    
    // Open compressed file
    FILE* f = fopen(filePath, "rb");
    
    uint32_t remaining = compressedSize;
    uint32_t total = 0;
    uint16_t toRead = 0;

    uint32_t tries = 0;
    uint32_t nbErrTries = 1;
    maxQueueLenght = 0;
    uint16_t queueLength = 0xFF;
    while(remaining > 0){
        
        // Compute next to read
        toRead =  decBufferSize;
        if(remaining < decBufferSize) toRead = remaining;
        // Create buffer for reading
        bufferDec = new uint8_t[toRead];     

        // Work...
        tries = safeRead(f, bufferDec, toRead);

        // ... done.
        if(tries > 1) nbErrTries += tries - 1;
        total += toRead;
        remaining -= toRead;

        // Sync
        thBuffer.data = bufferDec;
        thBuffer.size = toRead;
        
        if(!locked){ pthread_mutex_lock(&accessList); locked = true; }
            workList.push_back(thBuffer);
            queueLength = static_cast<uint16_t>(workList.size());
            /*
            // Limit memory usage;
            while(queueLength > 50){ // => 3,2 Mo
                pthread_mutex_unlock(&accessList); locked = false;
                usleep(100);
                pthread_mutex_lock(&accessList);
                queueLength = static_cast<uint16_t>(workList.size());
            }*/
            maxQueueLenght = max(maxQueueLenght, queueLength);
        pthread_mutex_unlock(&accessList); locked = false;
    }
    // Close file
    fclose(f); f = NULL;

    pthread_join (threadDecompInfo, NULL); // Last time to join
    return nbErrTries;
}

uint32_t loadCompressed(const char* filePath, uint8_t* buffer, uint32_t compressedSize, uint32_t fileSize){
    
    // Open compressed file
    FILE* f = fopen(filePath, "rb");
        
    // Create buffer decompression
    uint8_t bufferDec[decBufferSize];
    //memset(bufferDec, 0, decBufferSize);

    // Create dictionnary
    LZ4F_dctx* dctx = NULL;
    LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);

    uint32_t remaining = compressedSize;
    uint32_t total = 0;
    uint32_t remainingDec = fileSize;
    uint32_t totalDec = 0;
    uint16_t toRead = 0;

    uint32_t tries = 0;
    uint32_t nbErrTries = 1;
    
    while(remaining > 0){
        toRead =  decBufferSize;
        if(remaining < decBufferSize) toRead = remaining;
        
        tries = safeRead(f, bufferDec, toRead);
        if(tries > 1) nbErrTries += tries - 1;

        total += toRead;
        remaining -= toRead;

        safeDecompress(dctx, bufferDec, toRead, &buffer[totalDec], remainingDec);
        totalDec = fileSize - remainingDec;
    }
    
    LZ4F_freeDecompressionContext(dctx); dctx = NULL;

    // Close file
    fclose(f); f = NULL; // fflush
    return nbErrTries;
}

int main(int argc, char* argv[]){

    (void)argv; //unused

    // benchmark vars
    double durationMs = 0;
    double durationBeginMs = 0;

    // Compress file if necessary
    const char* filePath = "mslug3.x";
    string compressedFilePath = compressFile(filePath);

    // Get file size
    const uint32_t fileSize = getFileSize(filePath);
    const uint32_t compressedSize = getFileSize(compressedFilePath.c_str());
    double mo = static_cast<double>(fileSize) / 1000000;
    printf("Raw file size is %.2f Mo\n", mo);
    mo = static_cast<double>(compressedSize) / 1000000;
    printf("Compressed file size is %.2f Mo\n\n", mo);

    //Buffers
    uint8_t* buffer = new uint8_t[fileSize];
    uint8_t* keep = NULL;
    uint32_t tries = 0;

    for(uint8_t i = 0; i < 10; ++i){

        // Raw Data ------------------------
        memset(buffer, 0, fileSize);
        durationBeginMs = _xs_getCurrentTime();
        
        tries = loadRaw(filePath, buffer, fileSize);
  
        durationMs = _xs_getCurrentTime() - durationBeginMs; durationBeginMs = 0;
        printf("Loading Raw        file, in: %.2f ms speed:%.2f Mo/sec tries:%u\n", durationMs, computeSpeed(fileSize, durationMs), tries);

        if(argc > 1 && !keep){ // Will perfom a check
            keep = buffer;
            buffer = new uint8_t[fileSize];
        }

        // Compressed Data ----------------------------------------------------------------------------------------------------------------        
     
        // Reset buffer
        memset(buffer, 0, fileSize);
        durationBeginMs = _xs_getCurrentTime();

        tries = loadCompressed(compressedFilePath.c_str(), buffer, compressedSize, fileSize);
        
        durationMs = _xs_getCurrentTime() - durationBeginMs; durationBeginMs = 0;
        //      Loading Raw        file, in:
        printf("Loading Compressed file, in: %.2f ms speed:%.2f Mo/sec tries:%u\n", durationMs, computeSpeed(fileSize, durationMs), tries);

        if(keep){ // Check to perform
            printf("Check %s !\n", memcmp(buffer, keep, fileSize) == 0 ? "OK" : "ERROR");
        }
        
        
        // Thread Decompress ----------------------------------------------------------------------------------------------------------------
        memset(buffer, 0, fileSize);
        uint16_t maxQueueLenght = 0;
        durationBeginMs = _xs_getCurrentTime();
        
        tries = threadedDecompression(compressedFilePath.c_str(), buffer, compressedSize, fileSize, maxQueueLenght);
        
        durationMs = _xs_getCurrentTime() - durationBeginMs; durationBeginMs = 0;
        //      Loading Raw        file, in:
        printf("Thread decompress  file, in: %.2f ms speed:%.2f Mo/sec tries:%u maxQueueLenght=%d\n",
                durationMs, computeSpeed(fileSize, durationMs), tries, maxQueueLenght);
        if(keep){ // Check to perform
            printf("Check %s !\n", memcmp(buffer, keep, fileSize) == 0 ? "OK" : "ERROR");
            
        }
    }

    delete[] keep; keep = NULL;
    delete[] buffer; buffer = NULL;
    return 0;
}

