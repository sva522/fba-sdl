
Final Burn Alpha SDL for GCW-Zero
---------------------------------

FBA SDL for RS97 is based on FBA 0.2.97.35
with proof of concept XtraSmall (XSXS) mod for caching NeoGeo ROMs

1.           Introduction

The repo : https://github.com/sva522/fba-sdl is proof of concept of a caching mod on FBA-SDL running on RS97 handled console. I did not provide this code as a clean modification ready to become a pull request. It is written as a hack which purpose is to contain only "//XSXS" eXtra Small minimum local modifications which reduces risk of regression.

It’s based on FBA 0.2.97.35 for GCW0. Its purpose is to load "heavy” (>70Mo) Neogeo rom games without struggling because of lack of RAM memory

It was tested on RS97 handled console v3.0 with Custom firmware opendingux RetroFW 1.2 and Metal Slug3 which is one of the heavier rom. (90Mo uncompressed).

2.           Finding the solution

RetroFW 1.2 provide swap support. In this case games does not crash during loading. But loading can lasts dozen of minutes before being ready to play. Swap is not really a solution. Users can improve their device performance by changing µSD by a more powerful one. But this improvement is very limited. RS97 internal bus bandwidth is quite small and read/write speed is quite slow. In this case, device quickly struggle using swap.

 

The solution is to provide a caching mod. ROM will be loading and preprocessed on a regular computer so that memory can be dumped and reload quicky later on handled console.

3.           Previous caching Mod

Game loading is faster on stock firmware. Original FBA for dingux (in 2009) provided uses .fba rom files which size match with uncompressed rom files. A "fba_cache” tool  was provided to create cache files from .zip roms and avoid lack of RAM issues. Unfortunately, this version cannot run on custom firmware. I was unable to find original source. There is a FBA320 github repo which does not contains .fba support and is less advanced version of FBA-sdl (forked from FBA320).

4.           Caching Mod Design

My goal is to create a FBA caching system. Roms will be preprocessed running FBA on linux. When game is loaded, resources are :

	1.    Loaded from zip file

	2.    "Decrypted "

	3.    Decoded

This mod is active only on two first step. Last one does not require so much cpu time.

Use make to compile on Linux. Running fba-sdl mslug.zip will load game, dump loaded resources and exit. As a result a .x cache file is created. In this file each resource is “highly" compressed with a very light and fast last-gen compression algorithm lz4. There is a benchmark in lz4-test.

When exe is run on console. If a .x file with same name than zip (or 7z) is found, it is used. If file does not exist, exe run normally with NO MODIFICATION.

Yes it means, you keep the .x by side with the original .zip. But we both know space is not an issue on your µSD. Furthermore, this mechanism is useful only big NeoGeo games and .x cache files are already compressed.

5.           Build

 - Use make f Makefile.RS97 to build modded fba-sdl for RS97. I used RetroFW toolchain.
 - #FBA_DEBUG define is used to filter code which will not be executed on mips release.
 - Precompiled exe can be retrieve directly from repo.
 - Dependencies are SDL, SDL_mixer, libao

6.           Result

As a result loading mslug3 : Before 12 min, Now with a 41 Mo .x file => 45 sec !!!

