#!/bin/bash

#Necesito dos entradas...
if [ $# -ne 2 ]
then 
    echo "Faltan Argumentos <dir> <caracters>"
fi

#Si la entrada no es un directorio...
if [ ! -d $1 ]
then
    echo $1 "no es un Directorio válido"
fi


nomsFitxers=$(ls -l | awk '{print $9}')

for nom in $nomsFitxers
do 
    if [ ${nom#$2} ]
done