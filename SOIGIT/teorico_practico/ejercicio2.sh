#!/bin/bash

# Comprobar si el número de argumentos es correcto
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 directorio" >&2
  exit 1
fi

# Comprobar si el directorio existe
if [ ! -d "$1" ]; then
  echo "El directorio $1 no existe." >&2
  exit 1
fi

# Utilizar find para buscar todos los subdirectorios
# Procesar cada subdirectorio individualmente y calcular su tamaño total
# Mostrar el resultado en un formato legible para el usuario
for dir in $(find "$1" -type d); do
  du -bs "$dir" | awk -v dir="$dir" '{print $1"\t"dir}'
done
