#!/bin/bash


#Realizar un script llamado ejercicio1.sh que tiene dos argumentos: un directorio y un valor
#numérico. El objetivo del script es analizar el tamaño en bytes de los archivos que hay dentro del
#directorio, ignorando todos los subdirectorios. El script deberá mostrar por pantalla el número de
#archivos que tengan un tamaño en bytes superior (estrictamente) al valor especificado, así como
#el valor total (en bytes) de estos archivos


if [ $# -ne 2 ]
then
    echo "Faltan argumentos: <dir> <size>"
    exit 1
fi

if [ ! -d $1 ]
then
    echo "El primer parámetro no es un directorio"
    exit 2
fi

sizeCar=$2

#Contiene toda la información en una linea ordenada de mayor a menor
#El primer elemento es el peso de la primera carpeta
objects=$(ls -l --sort=size | awk '{print $5,$9}')

totalBytes=0
numDocs=0

#Variable que me dirá si estoy en la secuencia del peso
peso=1
bytesNext=0


for i in $objects
do 
    # INST:  $i | awk '{print $1}' 
    #Quitará el elemento en la posición 1. Así obtengo cada valor de los
    #paquetes con sus respectivos nombres

    let resto=peso%2
    #Si es un peso de carpeta...
    if test $resto -eq 1;
        then
            #Lo primero es comprobar que la siguiente entrada aún está en el rango
            
            bytesNext=$i | awk '{print $1}'

            echo $bytesNext
            #SI el elemento es menor a lo pedido se acaba la secuencia.
            if [ $bytesNext < $sizeCar ];
                then
                    echo "break"
                    break
                fi

            #Incrementamos el peso total
            totalBytes+=$bytesNext
    
    #Sino, entonces es un documento nuevo dentro del rango
    else
        ((numDocs+=1))
    fi
    
    ((peso++)) #Cambia a ser par o impar dependiendo del número de iteración

done
#| awk '{print $(($num))}'