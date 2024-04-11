#!/bin/bash

if [ ! -d $1 ]
then
    echo "No es un directorio válido"
fi


find "$1" -name "*.$2" -type f -exec ls -s {} \; | sort -rn | head -5 | awk '{print $2}'

exit 0