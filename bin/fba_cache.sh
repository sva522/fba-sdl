#!/bin/bash
rm -f *.x

for zip in ./*.zip; do
    if [ "$zip" != "./neogeo.zip" ]; then
        cacheFile=${zip%.*}.x
        ./fbasdl "$zip"
    fi
done

