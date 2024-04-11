#!/bin/bash

# Aseguramos que hayan dos directorios por parámetro
if [ $# -ne 2 ]; then
  echo "Faltan argumentos <dir_1> <dir_2>"
  exit 1
fi

# Buscamos los ficheros del primer directorio (type f)
primerDirectorio=$(find "$1" -type f)

#Por cada fichero dentro del primer directorio...
for archivo in $primerDirectorio
do
  #Cambiamos el nombre de dir_1 por el del directorio2 
  #para poder preguntar si el archivo con tal dirección existe o no.

  if [ ! -e "${archivo/$1/$2}" ]
  then
    echo "${archivo}"  #Si no existe entonces lo mostramos por pantalla
  fi
done

