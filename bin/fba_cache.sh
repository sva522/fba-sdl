#!/bin/bash
rm -f *.x

for zip in ./*.zip; do
    if [ "$zip" != "./neogeo.zip" ]; then
        cacheFile=${zip%.*}.x
        ./fbasdl "$zip"
    fi
done

echo "RESTORE ---------------------------"    
for zip in ./*.zip; do
    if [ "$zip" != "./neogeo.zip" ]; then
        rm -f *.log
        ./fbasdl "$zip"
        cat *.log 2>/dev/null && rm -f *.log 
    fi
done
