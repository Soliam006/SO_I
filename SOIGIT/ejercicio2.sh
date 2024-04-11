#!/bin/bash

if [ $# -ne 2 ]
then
    echo "Faltan argumentos: <dir> <size>"
    exit 1
fi


palabra1=$1
palabra2=$2
size1=${#palabra1}

#El caso más obvio sería cuando no tuvieran el mismo tamaño
if [ $size1 != ${#palabra2} ]
then
    echo "No son Anagramas"
    exit 1
fi

#Recorreré toda la primera palabra
for ((i=0; i<$size1; i++))
do
    #Y buscaré una letra igual a la de la primera
    for ((j=0; j<$size1; j++))
    do
        #Si son iguales, elimino la letra de la segunda palabra
        if [ ${palabra1:i:1} = ${palabra2:j:1} ]
        then
            palabra2=${palabra2:0:j}${palabra2:j+1:size1-j}
            break
        fi
    done
done

#Si la segunda palabra tiene tamaño 0, es porque se encontró 
#todas las letras en la otra palabra, por lo tanto es Anagrama
if [ ${#palabra2} -eq 0 ]
then 
    echo "Son Anagramas"
else
    echo "No son Anagramas"
fi