#!/bin/bash

# Verificar que se han proporcionado los argumentos necesarios
if [ $# -ne 3 ]; then
  echo "Uso: $0 <extension> <cadena_a_buscar> <cadena_de_sustitución>"
  exit 1
fi

extension="$1"
search="$2"
change="$3"

# Buscamos los archivos con la extension dada
documents=$(find . -type f -name "*.${extension}")

for archivo in $documents
do
#En cada archivo buscamos la palabra que buscamos y contamos cuantas veces aparece con el wordcount
#(Para evitar coger la linea entera con el grep usamos la instr. "-o" y así solo coge la palabra)
  numPalabras=$(grep -o "${search}" "${archivo}" | wc -l)
  #Lo mostramos por pantalla
  echo "${archivo} ${numPalabras}"

  # Cambiamos los valores y escribimos de nuevo ya que el sed no lo reescribe...
  sed -i "s/${search}/${change}/g" "${archivo}" >> "${archivo}"

done
