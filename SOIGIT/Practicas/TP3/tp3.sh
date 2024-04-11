#!/bin/bash


#Comprobar que el número de parámetros sea correcto. Si no son correctos, el programa
#deberá salir con un código de error.

if [ $# -ne 1 ];
then
    echo "Número de parámetros incorrexto <dic>"
    exit 1
fi

#Comprobar que el parámetro pasado como argumento existe y es un directorio. En caso
#de que no exista o no sea un directorio, el programa deberá salir con un código de error.

if [ ! -d $1 ];
then 
    echo $1 no es un directorio válido
    exit 1
fi

directorios=$(find $1)

find $1