#!/bin/bash


if [ $# -ne 1 ];
then
    echo "Nombre incorrecto de parametros $0 <dir>"
    exit 1
fi

if [ -d $1 ]
then 
    echo "El parametro es un directorio, no es Valido como entrada"
fi

if [ ! -f $1 ]; 
then
    echo "El fichero $1 no existe"
    exit 1
fi

directorios=$(cat $1)


noExiste=0
existe=0

#Recorremos todas las direcciones 
for i in $directorios 
do
    #Preguntamos si es un directorio o un fichero existente...
    if [ -f $i -o -d $i ];
    then
        existe=$(($existe+1))  #Incrementa 
    else
        noExiste=$(($noExiste+1)) #/
    fi
done

echo "Existen: $existe"
echo "No existen: $noExiste"
exit 0