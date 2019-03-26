
#ifdef LINUX_PC

bool _dumpBefore(const char* ressourceName){
    #ifdef LINUX_PC
    printf("Start dump %s ----\n", ressourceName);
    #endif
    // Do load so we can dump data after
    return true; 
}

void _dumpAfter(){
    
    double mo = static_cast<double>(_restoreBufferSize) / 1000000;
    printf("Dump and compressing data: %.2f Mo... ", mo); fflush(stdout);
    
    uint32_t compressedSize =_dumpToFile(_cacheFile, _restoreBuffer, _restoreBufferSize);

    mo = static_cast<double>(compressedSize) / 1000000;
    printf("(OK) (%.2f Mo)\n", mo);

    _total += _restoreBufferSize;
    _totalCompressed += compressedSize;   
}

#endif

bool _restoreBefore(const char* ressourceName, uint8_t* buffer, uint32_t size){

    #ifdef BENCHMARK
        _benchmark = _getCurrentTime();
    #endif

    uint32_t compressedSize = _readSizeInStream(_cacheFile);

    #ifdef LINUX_PC
        double mo = static_cast<double>(compressedSize) / 1000000;
        printf("Restoring %s %.2f Mo... ",ressourceName, mo); fflush(stdout);
    #endif
    
    
    _restoreFromFile(_cacheFile, buffer, size, compressedSize);
    
    #ifdef LINUX_PC
        mo = static_cast<double>(size) / 1000000;
        printf("(OK) (%.2f Mo)\n", mo);

        _total += size;
        _totalCompressed += compressedSize;
    #endif
    
    #ifdef BENCHMARK
        _benchmark = _getCurrentTime() - _benchmark;
        fprintf(_benchmarkFile, "Restore %.2f sec", _benchmark); fflush(_benchmarkFile);
        _benchmark = _getCurrentTime();
    #endif

    // Nothing still to restore
    if(feof(_cacheFile)) xs_free();
    return false; // restore done / => must not load
}

void _restoreAfter(){
    #ifdef BENCHMARK
    _benchmark = _getCurrentTime() - _benchmark;
    fprintf(_benchmarkFile, "Load %.2f sec", _benchmark); fflush(_benchmarkFile);
    #endif
}

bool _skipBefore(const char* ressourceName, uint32_t size){
    
    #ifdef LINUX_PC
    double mo = static_cast<double>(_total) / 1000000;
    printf("Skip ----------- %s (%2.f Mo)\n", ressourceName, mo);
    #endif
    
    // Perfom load
    return true; 
}

void _skipAfter(){
    #ifdef LINUX_PC
    printf("Skipped.\n"); 
    #endif
}
