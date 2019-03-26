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

#ifdef BENCHMARK
double _benchmark = 0;
FILE* _benchmarkFile = NULL;
#endif

#include "xs_utils.cpp"
#include "xs_steps.cpp"

void xs_init(const char* romName){

    // init members vars  -------
    _cacheFile = NULL;
    _cacheFilePath = NULL;

    #ifdef LINUX_PC
        _dump = false;

        _restoreBuffer = NULL;
        _restoreBufferSize = 0;

        _total = 0;
        _totalCompressed = 0;
    #endif
    #ifdef BENCHMARK
        _benchmark = 0;
        _benchmarkFile = NULL;
    #endif
    // -------------------------

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

void xs_free(){

    if(_cacheFile){
        fclose(_cacheFile); _cacheFile = NULL;
        
        #ifdef LINUX_PC
        double mo = static_cast<double>(_total) / 1000000;
        double moComp = static_cast<double>(_totalCompressed) / 1000000;
        if(_dump){
            printf("End of dump %.2f Mo to %.2f Mo.\n\n",  mo, moComp);
            
        }else{ // restore
            printf("End of restore %.2f Mo from %.2f Mo.\n\n", mo, moComp);  
     
        }
        exit(0);
        #endif
    }
    
    #ifdef BENCHMARK
        if(_benchmarkFile){ fclose(_benchmarkFile); _benchmarkFile = NULL; }
    #endif
}

// return true if restore is not aivailable and Neload SHOULD be used
// return false if restore can be performed and and NeoLoad should NOT be used
bool xs_before(const char* ressourceName, uint8_t* buffer, uint32_t size){
    // Open file if necessary
    _openCacheFile(); 
    
    // Record work to do ----
    #ifdef LINUX_PC
    static bool firstCall = true; if(firstCall){ firstCall = false; printf("\n"); } 
    
     // Record for after (dump or restore)
    _restoreBuffer = buffer;
    _restoreBufferSize = size;
    // -----------------------
    
    // SKIP
    if(size == 0) return _skipBefore(ressourceName, size);
    
    // DUMP
    if(_dump) return _dumpBefore(ressourceName);
    #endif // ----------------------------

    // NORMAL (Cannot restore => load) 
    if(!_cacheFile) return true;

    // RESTORE
    return _restoreBefore(ressourceName, buffer, size);
}

void xs_after(){
    #ifdef LINUX_PC
    
    // SKIP
    if(_restoreBufferSize == 0){ _skipAfter(); return;}

    // DUMP
    if(_dump && _restoreBufferSize) _dumpAfter();

    // Record work done
    _restoreBuffer = NULL;
    _restoreBufferSize = 0;
    #endif
    
    // RESTORE
    _restoreAfter();
}
