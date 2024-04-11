#!/bin/bash

# Comprovamos que sea correcto el numero de parametros
if [ $# -ne 2 ]
then
    echo "Hace falta el número correxto de argumentos <UserName> <Filtro>"
    exit 1
fi

# Comprobamos que el segundo parametro tiene un input correcto.
if [ $2 != "VSZ" -a $2 != "RSS" ];
then
    echo El segundo argumento no es válido
    exit 1
fi

# Con ps aux mostramos los procesos
# Luego hacemos un grep $1 para filtrar por el usuario.
# Con el comando awk conseguimos las columans que nos interesan
# Hacemos un grep para quitar los procesos "ps", "grep"... Que se quedaran a pesar de que no exista usuario
# Por ultimo ordenamos los procesos y nos quedamos con las 5 primeros lineas. 

# Ordenaoms de manera distinta segun el segundo parametro
if [ "$2" = "VSZ" ]; then

    instrucciones="$(ps aux | grep $1 | awk '{print $5, $6, $11}' | grep -v -e "ps" -e "grep" -e "awk" -e "/bin/bash"| sort -n -r -k 1 | head -5)"

elif [ "$2" = "RSS" ]; then

    instrucciones="$(ps aux | grep $1 | awk '{print $5, $6, $11}' | grep -v -e "ps" -e "grep" -e "awk" -e "/bin/bash" | sort -n -r -k 2 | head -5)"
fi

# Si instrucciones esta vacio significa que el usuario no existe. Por lo que imprimimos un mensaje de error
if [ ! -z "$instrucciones" ];
then

    echo "$instrucciones"
    exit 0

else
    echo "$1 no tiene asociado ningún proceso"
    exit 1
fi